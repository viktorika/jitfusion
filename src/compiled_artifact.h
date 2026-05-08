/*
 * @Author: victorika
 * @Date: 2026-05-07 16:13:51
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 16:43:30
 */
/*
 * Compiled-artifact (JFCA) serialization format.
 *
 * This module owns everything byte-level about the format that
 * ExecEngine::SaveCompiled / LoadCompiled round-trip through: magic
 * numbers, version, layout of core fields, the extension area, and
 * the Read/Write primitives used by both sides.
 *
 * Design goals:
 *   1. Self-contained blob. A reader with only the bytes in hand must
 *      be able to reject a foreign file (header magic), reject a blob
 *      produced by an incompatible version (format_version), and
 *      reject a blob produced on a different target (llvm_version,
 *      target_triple, cpu_name) — all before attempting to load the
 *      embedded object file.
 *   2. Relocatable object payload. The object file bytes are inert
 *      data; resolution of undefined external symbols (C functions
 *      registered in FunctionRegistry) happens entirely at
 *      LoadCompiled time. This mirrors how a normal linker resolves
 *      relocations when loading a .o file.
 *   3. Forward-compatible extension area. New optional metadata can
 *      be attached via tagged records at the end of the artifact.
 *      Old readers silently skip tags they do not recognize. If a
 *      change is not backward-compatible at the bytes level (e.g.
 *      the object payload is compressed), that is NOT an extension:
 *      it must bump format_version instead.
 *
 * Explicitly out of scope:
 *   - "Required extensions" (older readers must bail). Any change
 *     important enough to demand that behavior is by definition
 *     core-incompatible — bump format_version.
 *   - "Minimum reader version". Same reasoning: format_version
 *     mismatch already handles this cleanly.
 *   - ExecEngine / LLVM JIT concerns. This file does NOT include
 *     LLVM headers; it is pure bytes-in / bytes-out so it compiles
 *     fast and can be unit-tested without spinning up a JIT.
 *
 * Layout (little-endian integers where applicable):
 *   magic[4]           = 'J','F','C','A'
 *   format_version     uint32                     (kArtifactFormatVersion)
 *   llvm_version       len-prefixed utf8 str      (LLVM_VERSION_STRING at save time)
 *   target_triple      len-prefixed utf8 str      (sys::getProcessTriple())
 *   cpu_name           len-prefixed utf8 str      (sys::getHostCPUName())
 *   mode               uint8                      (0 = single, 1 = batch)
 *   fp_math_mode       uint8                      (FPMathMode enum value)
 *   num_entries        uint32                     (>= 1)
 *   per-entry repeated num_entries times:
 *     ret_type         uint8                      (ValueType)
 *   obj_size           uint64
 *   obj_bytes          raw bytes                  (the relocatable .o)
 *   extension_count    uint32                     (0 in v2)
 *   per-extension repeated extension_count times:
 *     tag              uint32                     (ArtifactExtensionTag)
 *     payload_len      uint32
 *     payload          raw bytes
 *   footer_magic[4]    = 'J','F','C','E'
 *
 * Notable non-field:
 *   Entry symbol names are NOT stored in the blob. They follow a
 *   deterministic scheme tied to `format_version` — currently "entry"
 *   for single mode and "entry" + std::to_string(i) for batch entry i
 *   — known to both SaveCompiled (via the codegen that produced the
 *   object) and LoadCompiled (which hardcodes the lookup names).
 *   Changing the scheme is a format-breaking event: either gate the
 *   new scheme behind a bumped format_version, or introduce an
 *   extension-area record that supplies explicit symbol names.
 */
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "exec_options.h"  // jitfusion::FPMathMode
#include "status.h"
#include "type.h"

namespace jitfusion {

// The on-wire FP math mode IS the public FPMathMode enum. They used to
// be two separate types to avoid pulling LLVM into this serde TU, but
// the public headers no longer include LLVM, so the split is obsolete.
// FPMathMode's underlying type and enumerator values are pinned to the
// on-wire layout; a static_assert pair in compiled_artifact.cc guards
// this.

enum class ArtifactMode : std::uint8_t {
  kSingle = 0,
  kBatch = 1,
};

// Extension tag registry. New optional metadata features allocate a
// new tag here and append to ArtifactHeader::extensions; readers that
// do not recognize a tag skip the record silently.
//
// Rules when adding a tag:
//   1. Assign the next unused integer; never reuse a retired tag —
//      readers in the wild may still expect the old meaning.
//   2. Document payload layout next to the tag.
//   3. The feature must be purely additive / advisory. A change that
//      an old reader MUST notice (e.g. object bytes got compressed)
//      is not an extension; bump kArtifactFormatVersion instead.
enum class ArtifactExtensionTag : std::uint32_t {
  // No tags defined in v2.
  // kExampleMetadata = 1,  // payload: <describe layout here>
};

struct ArtifactExtension {
  std::uint32_t tag;    // raw tag; cast to ArtifactExtensionTag if known
  std::string payload;  // opaque bytes, length implicit
};

// Structured view of everything in a JFCA artifact except the
// object payload itself. The object payload is kept separate so
// that Deserialize can return a zero-copy std::string_view into
// the caller's buffer (object bytes can be MB-sized; avoiding a
// copy matters).
struct ArtifactHeader {
  // === Core, always present ===
  std::string llvm_version;
  std::string target_triple;
  std::string cpu_name;
  ArtifactMode mode{ArtifactMode::kSingle};
  FPMathMode fp_math_mode{FPMathMode::kFast};
  // Per-entry return types; size doubles as the "num_entries" field
  // on the wire (always >= 1). Entry symbol names are intentionally
  // NOT stored — see the "Notable non-field" comment in the file
  // header. In single mode per_entry_ret_types has size 1.
  std::vector<ValueType> per_entry_ret_types;

  // === Forward-compat extension area ===
  std::vector<ArtifactExtension> extensions;
};

// Current format version written by SerializeArtifact. Bump on any
// change that breaks byte-level compatibility with existing readers.
// Purely additive changes via the extension area do NOT require a
// version bump.
constexpr std::uint32_t kArtifactFormatVersion = 2;

// Serialize `header` + `object_bytes` into a self-contained JFCA
// blob. Appends to `*out` (does not clear), matching std::string's
// usual output-parameter convention and letting callers pre-reserve.
//
// The only failure mode today is a programming error (e.g. empty
// per_entry_ret_types, or single-mode with != 1 entry); runtime
// inputs cannot otherwise cause failure.
Status SerializeArtifact(const ArtifactHeader& header, std::string_view object_bytes, std::string* out);

// Parse a JFCA blob. On success `*out_header` receives the header
// fields (including unknown extensions copied verbatim), and
// `*out_object_bytes` is a view into `blob` pointing at the object
// payload — so `blob` must outlive downstream use of the view.
//
// Returns Status::InvalidArgument with a human-readable message on
// any kind of rejection: wrong magic, unsupported format_version,
// truncation, trailing bytes, etc. The message is prefixed with
// "compiled artifact: " so it is easy to grep out in logs.
//
// Environment-compatibility checks (matching LLVM version / target
// triple / CPU name against the current process) are NOT performed
// here — those are policy decisions that belong at the ExecEngine
// level. Deserialize only guarantees the bytes parsed cleanly.
Status DeserializeArtifact(std::string_view blob, ArtifactHeader* out_header, std::string_view* out_object_bytes);

// Apply the host-compatibility checks shared by ExecEngine::LoadCompiled
// and BatchExecEngine::LoadCompiled.
//
// The three `host_*` strings must be supplied by the caller (usually
// the engine, which pulls them from llvm::sys::getProcessTriple() and
// friends). We keep LLVM out of this translation unit on purpose, so
// the artifact serde stays unit-testable without spinning up a JIT.
//
// Each check can be independently suppressed via `opts`. The `mode`
// check is also performed here but is not user-suppressible: a mode
// mismatch means the caller reached for the wrong engine class, and
// loading would fail at symbol lookup anyway with a much worse error.
//
// `expected_mode` is the mode the calling engine expects (kSingle for
// ExecEngine, kBatch for BatchExecEngine); the error message mentions
// which engine the caller should have used instead.
//
// Returns Status::InvalidArgument on mismatch, with a message prefixed
// "compiled artifact: " so it joins the other artifact errors in logs.
Status CheckHostCompatibility(const ArtifactHeader& header, const LoadCompiledOptions& opts,
                              std::string_view host_llvm_version, std::string_view host_target_triple,
                              std::string_view host_cpu_name, ArtifactMode expected_mode);

}  // namespace jitfusion
