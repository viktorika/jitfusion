/*
 * @Author: victorika
 * @Date: 2026-05-07 16:14:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 16:18:56
 */
#include "compiled_artifact.h"

#include <array>
#include <cstdint>
#include <utility>

namespace jitfusion {

namespace {

// Magic numbers at the head and tail of every artifact. The tail is
// not strictly required for parsing correctness (lengths inside the
// blob are explicit), but it is a cheap way to detect truncation that
// happens to leave a "coincidentally plausible" length prefix before
// the cut.
constexpr std::array<char, 4> kArtifactMagic{'J', 'F', 'C', 'A'};
constexpr std::array<char, 4> kArtifactFooterMagic{'J', 'F', 'C', 'E'};

// ---------------------------------------------------------------------------
// Little-endian write helpers.
//
// We deliberately do NOT use memcpy of native ints. Reasons:
//   1. The artifact is a wire format; explicit byte-by-byte assembly
//      documents the layout at the code level and makes a big-endian
//      host (hypothetical, not supported today) trivially correct.
//   2. The modern compilers we target recognize this pattern and lower
//      it to a single store on little-endian hosts, so there is no
//      measurable runtime cost.
// ---------------------------------------------------------------------------
void AppendU8(std::string& out, std::uint8_t v) { out.push_back(static_cast<char>(v)); }

void AppendU32LE(std::string& out, std::uint32_t v) {
  for (int i = 0; i < 4; ++i) {
    out.push_back(static_cast<char>((v >> (i * 8)) & 0xff));
  }
}

void AppendU64LE(std::string& out, std::uint64_t v) {
  for (int i = 0; i < 8; ++i) {
    out.push_back(static_cast<char>((v >> (i * 8)) & 0xff));
  }
}

void AppendLenPrefixedStr(std::string& out, std::string_view s) {
  AppendU32LE(out, static_cast<std::uint32_t>(s.size()));
  out.append(s.data(), s.size());
}

void AppendBytes(std::string& out, const char* data, std::size_t n) { out.append(data, n); }

// ---------------------------------------------------------------------------
// Cursor-style reader.
//
// All Read* methods return Status so that a malformed blob surfaces a
// single precise error via JF_RETURN_NOT_OK rather than crashing on
// out-of-bounds access. The cursor (pos_) only advances on success, so
// error messages can report the offset at which parsing stopped.
// ---------------------------------------------------------------------------
class ArtifactReader {
 public:
  explicit ArtifactReader(std::string_view buf) : buf_(buf), pos_(0) {}

  Status ReadU8(std::uint8_t* out) {
    if (pos_ + 1 > buf_.size()) {
      return Status::InvalidArgument("compiled artifact: unexpected EOF reading u8 at offset ", pos_);
    }
    *out = static_cast<std::uint8_t>(buf_[pos_++]);
    return Status::OK();
  }

  Status ReadU32LE(std::uint32_t* out) {
    if (pos_ + 4 > buf_.size()) {
      return Status::InvalidArgument("compiled artifact: unexpected EOF reading u32 at offset ", pos_);
    }
    std::uint32_t v = 0;
    for (int i = 0; i < 4; ++i) {
      v |= static_cast<std::uint32_t>(static_cast<std::uint8_t>(buf_[pos_ + i])) << (i * 8);
    }
    pos_ += 4;
    *out = v;
    return Status::OK();
  }

  Status ReadU64LE(std::uint64_t* out) {
    if (pos_ + 8 > buf_.size()) {
      return Status::InvalidArgument("compiled artifact: unexpected EOF reading u64 at offset ", pos_);
    }
    std::uint64_t v = 0;
    for (int i = 0; i < 8; ++i) {
      v |= static_cast<std::uint64_t>(static_cast<std::uint8_t>(buf_[pos_ + i])) << (i * 8);
    }
    pos_ += 8;
    *out = v;
    return Status::OK();
  }

  Status ReadLenPrefixedStr(std::string* out) {
    std::uint32_t len = 0;
    JF_RETURN_NOT_OK(ReadU32LE(&len));
    if (pos_ + len > buf_.size()) {
      return Status::InvalidArgument("compiled artifact: truncated string (want ", len, " bytes, have ",
                                     buf_.size() - pos_, ")");
    }
    out->assign(buf_.data() + pos_, len);
    pos_ += len;
    return Status::OK();
  }

  Status ReadMagic(const std::array<char, 4>& expected, const char* label) {
    if (pos_ + 4 > buf_.size()) {
      return Status::InvalidArgument("compiled artifact: unexpected EOF reading ", label, " magic");
    }
    for (int i = 0; i < 4; ++i) {
      if (buf_[pos_ + i] != expected[i]) {
        return Status::InvalidArgument("compiled artifact: wrong ", label,
                                       " magic (not a JitFusion compiled artifact?)");
      }
    }
    pos_ += 4;
    return Status::OK();
  }

  Status ReadBytesView(std::size_t n, std::string_view* out) {
    if (pos_ + n > buf_.size()) {
      return Status::InvalidArgument("compiled artifact: truncated blob (want ", n, " bytes, have ", buf_.size() - pos_,
                                     ")");
    }
    *out = std::string_view(buf_.data() + pos_, n);
    pos_ += n;
    return Status::OK();
  }

  Status ReadBytesCopy(std::size_t n, std::string* out) {
    std::string_view v;
    JF_RETURN_NOT_OK(ReadBytesView(n, &v));
    out->assign(v.data(), v.size());
    return Status::OK();
  }

  [[nodiscard]] std::size_t remaining() const { return buf_.size() - pos_; }

 private:
  std::string_view buf_;
  std::size_t pos_{};
};

// Pin the numeric values of FPMathMode / ArtifactMode to the on-wire
// encoding. If someone later reorders these enums, the static_asserts
// catch it at compile time instead of at Load time on a user's machine.
static_assert(static_cast<std::uint8_t>(FPMathMode::kStrict) == 0, "FPMathMode::kStrict must serialize as 0");
static_assert(static_cast<std::uint8_t>(FPMathMode::kFast) == 1, "FPMathMode::kFast must serialize as 1");
static_assert(static_cast<std::uint8_t>(ArtifactMode::kSingle) == 0, "ArtifactMode::kSingle must serialize as 0");
static_assert(static_cast<std::uint8_t>(ArtifactMode::kBatch) == 1, "ArtifactMode::kBatch must serialize as 1");

}  // namespace

Status SerializeArtifact(const ArtifactHeader& header, std::string_view object_bytes, std::string* out) {
  if (out == nullptr) {
    return Status::InvalidArgument("SerializeArtifact: out must not be null");
  }
  if (header.per_entry_ret_types.empty()) {
    return Status::InvalidArgument("[internal] SerializeArtifact: header has 0 entries");
  }
  if (header.mode == ArtifactMode::kSingle && header.per_entry_ret_types.size() != 1) {
    return Status::InvalidArgument("[internal] SerializeArtifact: single-mode artifact must have exactly 1 entry, got ",
                                   header.per_entry_ret_types.size());
  }

  // Pre-size. The object bytes dominate; a small over-reserve amortizes
  // the header appends. We undersize by a few bytes rather than
  // over-reserving — std::string growth is exponential anyway.
  out->reserve(out->size() + 128 + object_bytes.size());

  AppendBytes(*out, kArtifactMagic.data(), kArtifactMagic.size());
  AppendU32LE(*out, kArtifactFormatVersion);
  AppendLenPrefixedStr(*out, header.llvm_version);
  AppendLenPrefixedStr(*out, header.target_triple);
  AppendLenPrefixedStr(*out, header.cpu_name);
  AppendU8(*out, static_cast<std::uint8_t>(header.mode));
  AppendU8(*out, static_cast<std::uint8_t>(header.fp_math_mode));

  const auto num_entries = static_cast<std::uint32_t>(header.per_entry_ret_types.size());
  AppendU32LE(*out, num_entries);
  for (std::uint32_t i = 0; i < num_entries; ++i) {
    AppendU8(*out, static_cast<std::uint8_t>(header.per_entry_ret_types[i]));
  }

  AppendU64LE(*out, static_cast<std::uint64_t>(object_bytes.size()));
  AppendBytes(*out, object_bytes.data(), object_bytes.size());

  const auto ext_count = static_cast<std::uint32_t>(header.extensions.size());
  AppendU32LE(*out, ext_count);
  for (const auto& ext : header.extensions) {
    AppendU32LE(*out, ext.tag);
    AppendU32LE(*out, static_cast<std::uint32_t>(ext.payload.size()));
    AppendBytes(*out, ext.payload.data(), ext.payload.size());
  }

  AppendBytes(*out, kArtifactFooterMagic.data(), kArtifactFooterMagic.size());
  return Status::OK();
}

Status DeserializeArtifact(std::string_view blob, ArtifactHeader* out_header, std::string_view* out_object_bytes) {
  if (out_header == nullptr || out_object_bytes == nullptr) {
    return Status::InvalidArgument("DeserializeArtifact: output pointers must not be null");
  }

  ArtifactReader r(blob);
  JF_RETURN_NOT_OK(r.ReadMagic(kArtifactMagic, "header"));

  std::uint32_t format_ver = 0;
  JF_RETURN_NOT_OK(r.ReadU32LE(&format_ver));
  if (format_ver != kArtifactFormatVersion) {
    // Strict equality, matching the industry norm for JIT artifact
    // caches (ccache, rustc incremental, V8 codecache, Python .pyc).
    // Forward-compat for purely additive changes lives in the
    // extension area; anything else warrants a version bump.
    return Status::InvalidArgument("compiled artifact: format version ", format_ver,
                                   " not supported (this build expects ", kArtifactFormatVersion,
                                   "). The artifact was produced by an incompatible jitfusion version.");
  }

  JF_RETURN_NOT_OK(r.ReadLenPrefixedStr(&out_header->llvm_version));
  JF_RETURN_NOT_OK(r.ReadLenPrefixedStr(&out_header->target_triple));
  JF_RETURN_NOT_OK(r.ReadLenPrefixedStr(&out_header->cpu_name));

  std::uint8_t mode_byte = 0;
  JF_RETURN_NOT_OK(r.ReadU8(&mode_byte));
  if (mode_byte != static_cast<std::uint8_t>(ArtifactMode::kSingle) &&
      mode_byte != static_cast<std::uint8_t>(ArtifactMode::kBatch)) {
    return Status::InvalidArgument("compiled artifact: unknown mode byte ", static_cast<int>(mode_byte));
  }
  out_header->mode = static_cast<ArtifactMode>(mode_byte);

  std::uint8_t fp_byte = 0;
  JF_RETURN_NOT_OK(r.ReadU8(&fp_byte));
  if (fp_byte != static_cast<std::uint8_t>(FPMathMode::kStrict) &&
      fp_byte != static_cast<std::uint8_t>(FPMathMode::kFast)) {
    return Status::InvalidArgument("compiled artifact: unknown fp_math_mode byte ", static_cast<int>(fp_byte));
  }
  out_header->fp_math_mode = static_cast<FPMathMode>(fp_byte);

  std::uint32_t num_entries = 0;
  JF_RETURN_NOT_OK(r.ReadU32LE(&num_entries));
  if (num_entries == 0) {
    return Status::InvalidArgument("compiled artifact: num_entries is 0");
  }
  if (out_header->mode == ArtifactMode::kSingle && num_entries != 1) {
    return Status::InvalidArgument("compiled artifact: single-mode artifact with num_entries=", num_entries);
  }
  out_header->per_entry_ret_types.resize(num_entries);
  for (std::uint32_t i = 0; i < num_entries; ++i) {
    std::uint8_t rt_byte = 0;
    JF_RETURN_NOT_OK(r.ReadU8(&rt_byte));
    out_header->per_entry_ret_types[i] = static_cast<ValueType>(rt_byte);
  }

  std::uint64_t obj_size = 0;
  JF_RETURN_NOT_OK(r.ReadU64LE(&obj_size));
  JF_RETURN_NOT_OK(r.ReadBytesView(static_cast<std::size_t>(obj_size), out_object_bytes));

  std::uint32_t ext_count = 0;
  JF_RETURN_NOT_OK(r.ReadU32LE(&ext_count));
  out_header->extensions.clear();
  out_header->extensions.reserve(ext_count);
  for (std::uint32_t i = 0; i < ext_count; ++i) {
    ArtifactExtension ext;
    JF_RETURN_NOT_OK(r.ReadU32LE(&ext.tag));
    std::uint32_t plen = 0;
    JF_RETURN_NOT_OK(r.ReadU32LE(&plen));
    JF_RETURN_NOT_OK(r.ReadBytesCopy(plen, &ext.payload));
    // We do NOT attempt to interpret `ext.tag`. Unknown tags are
    // preserved verbatim in the returned header so that a caller
    // forwarding the blob (without modification) can still round-trip
    // it. This matches protobuf's "unknown fields preservation" idea,
    // which is what makes forward compatibility actually work in
    // practice rather than just in spec.
    out_header->extensions.emplace_back(std::move(ext));
  }

  JF_RETURN_NOT_OK(r.ReadMagic(kArtifactFooterMagic, "footer"));
  if (r.remaining() != 0) {
    return Status::InvalidArgument("compiled artifact: ", r.remaining(), " trailing bytes after footer");
  }
  return Status::OK();
}

}  // namespace jitfusion
