// A Bison parser, made by GNU Bison 3.8.2.45-25b3d-dirty.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2022 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


/**
 ** \file parser.hh
 ** Define the athena::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_PARSER_HH_INCLUDED
# define YY_YY_PARSER_HH_INCLUDED
// "%code requires" blocks.
#line 13 "parser.yy"

  #pragma once
  #include <string>
  #include "exec_engine.h"
  #include "ast_builder.h"

  typedef void* yyscan_t;
  #ifndef yylex
  #define yylex athena_yylex
  #endif

#line 61 "parser.hh"

# include <cassert>
# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "location.hh"
#include <typeinfo>
#ifndef YY_ASSERT
# include <cassert>
# define YY_ASSERT assert
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

#line 10 "parser.yy"
namespace athena {
#line 202 "parser.hh"




  /// A Bison parser.
  class Parser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
      , yytypeid_ (YY_NULLPTR)
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
      : yytypeid_ (&typeid (T))
    {
      YY_ASSERT (sizeof (T) <= size);
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {
      YY_ASSERT (!yytypeid_);
    }

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == typeid (T));
      YY_ASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == typeid (T));
      YY_ASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == *that.yytypeid_);
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
      yytypeid_ = YY_NULLPTR;
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // statement
      char dummy1[sizeof (Statement)];

      // "double"
      char dummy2[sizeof (double)];

      // "float"
      char dummy3[sizeof (float)];

      // "int16"
      char dummy4[sizeof (int16_t)];

      // "int32"
      char dummy5[sizeof (int32_t)];

      // "int64"
      char dummy6[sizeof (int64_t)];

      // "int8"
      char dummy7[sizeof (int8_t)];

      // "identifier"
      // "string"
      char dummy8[sizeof (std::string)];

      // expr
      // term
      // literal
      // list
      // function
      // infix_function
      // named_function
      // arg
      // boolean
      char dummy9[sizeof (std::unique_ptr<jitfusion::ExecNode>)];

      // f64_list
      char dummy10[sizeof (std::vector<double>)];

      // f32_list
      char dummy11[sizeof (std::vector<float>)];

      // i16_list
      char dummy12[sizeof (std::vector<int16_t>)];

      // i32_list
      char dummy13[sizeof (std::vector<int32_t>)];

      // i64_list
      char dummy14[sizeof (std::vector<int64_t>)];

      // i8_list
      char dummy15[sizeof (std::vector<int8_t>)];

      // string_list
      char dummy16[sizeof (std::vector<std::string>)];

      // args
      char dummy17[sizeof (std::vector<std::unique_ptr<jitfusion::ExecNode>>)];

      // u16_list
      char dummy18[sizeof (std::vector<uint16_t>)];

      // u32_list
      char dummy19[sizeof (std::vector<uint32_t>)];

      // u64_list
      char dummy20[sizeof (std::vector<uint64_t>)];

      // u8_list
      char dummy21[sizeof (std::vector<uint8_t>)];

      // "uint16"
      char dummy22[sizeof (uint16_t)];

      // "uint32"
      char dummy23[sizeof (uint32_t)];

      // "uint64"
      char dummy24[sizeof (uint64_t)];

      // "uint8"
      char dummy25[sizeof (uint8_t)];

      // program
      char dummy26[sizeof (void*)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };

    /// Whether the content is built: if defined, the name of the stored type.
    const std::type_info *yytypeid_;
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        TOK_YYEMPTY = -2,
    TOK_YYEOF = 0,                 // "end of file"
    TOK_YYerror = 1,               // error
    TOK_YYUNDEF = 2,               // "invalid token"
    TOK_IF = 3,                    // "if"
    TOK_SWITCH = 4,                // "switch"
    TOK_IN = 5,                    // "in"
    TOK_NOT = 6,                   // "not"
    TOK_AND = 7,                   // "and"
    TOK_OR = 8,                    // "or"
    TOK_TRUE = 9,                  // "true"
    TOK_FALSE = 10,                // "false"
    TOK_MINUS = 11,                // "-"
    TOK_PLUS = 12,                 // "+"
    TOK_STAR = 13,                 // "*"
    TOK_SLASH = 14,                // "/"
    TOK_LPAREN = 15,               // "("
    TOK_RPAREN = 16,               // ")"
    TOK_MODOLO = 17,               // "%"
    TOK_BITWISE_OR = 18,           // "|"
    TOK_BITWISE_XOR = 19,          // "^"
    TOK_BITWISE_AND = 20,          // "&"
    TOK_BITWISE_NOT = 21,          // "~"
    TOK_BITWISE_LEFT = 22,         // "<<"
    TOK_BITWISE_RIGHT = 23,        // ">>"
    TOK_EQUAL = 24,                // "=="
    TOK_ASSIGNMENT = 25,           // "="
    TOK_NOT_EQUAL = 26,            // "!="
    TOK_LESS_THAN = 27,            // "<"
    TOK_LESS_THAN_OR_EQUAL_TO = 28, // "<="
    TOK_GREATER_THAN = 29,         // ">"
    TOK_GREATER_THAN_OR_EQUAL_TO = 30, // ">="
    TOK_COMMA = 31,                // ","
    TOK_LBRACKET = 32,             // "["
    TOK_RBRACKET = 33,             // "]"
    TOK_SEMI = 34,                 // ";"
    TOK_ENTRY_ARG = 35,            // "entry_arg"
    TOK_EXEC_CTX = 36,             // "exec_ctx"
    TOK_OUTPUT = 37,               // "output"
    TOK_IDENTIFIER = 38,           // "identifier"
    TOK_I8 = 39,                   // "int8"
    TOK_I16 = 40,                  // "int16"
    TOK_I32 = 41,                  // "int32"
    TOK_I64 = 42,                  // "int64"
    TOK_U8 = 43,                   // "uint8"
    TOK_U16 = 44,                  // "uint16"
    TOK_U32 = 45,                  // "uint32"
    TOK_U64 = 46,                  // "uint64"
    TOK_F32 = 47,                  // "float"
    TOK_F64 = 48,                  // "double"
    TOK_STRING = 49,               // "string"
    TOK_NEG = 50                   // NEG
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 51, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_IF = 3,                                // "if"
        S_SWITCH = 4,                            // "switch"
        S_IN = 5,                                // "in"
        S_NOT = 6,                               // "not"
        S_AND = 7,                               // "and"
        S_OR = 8,                                // "or"
        S_TRUE = 9,                              // "true"
        S_FALSE = 10,                            // "false"
        S_MINUS = 11,                            // "-"
        S_PLUS = 12,                             // "+"
        S_STAR = 13,                             // "*"
        S_SLASH = 14,                            // "/"
        S_LPAREN = 15,                           // "("
        S_RPAREN = 16,                           // ")"
        S_MODOLO = 17,                           // "%"
        S_BITWISE_OR = 18,                       // "|"
        S_BITWISE_XOR = 19,                      // "^"
        S_BITWISE_AND = 20,                      // "&"
        S_BITWISE_NOT = 21,                      // "~"
        S_BITWISE_LEFT = 22,                     // "<<"
        S_BITWISE_RIGHT = 23,                    // ">>"
        S_EQUAL = 24,                            // "=="
        S_ASSIGNMENT = 25,                       // "="
        S_NOT_EQUAL = 26,                        // "!="
        S_LESS_THAN = 27,                        // "<"
        S_LESS_THAN_OR_EQUAL_TO = 28,            // "<="
        S_GREATER_THAN = 29,                     // ">"
        S_GREATER_THAN_OR_EQUAL_TO = 30,         // ">="
        S_COMMA = 31,                            // ","
        S_LBRACKET = 32,                         // "["
        S_RBRACKET = 33,                         // "]"
        S_SEMI = 34,                             // ";"
        S_ENTRY_ARG = 35,                        // "entry_arg"
        S_EXEC_CTX = 36,                         // "exec_ctx"
        S_OUTPUT = 37,                           // "output"
        S_IDENTIFIER = 38,                       // "identifier"
        S_I8 = 39,                               // "int8"
        S_I16 = 40,                              // "int16"
        S_I32 = 41,                              // "int32"
        S_I64 = 42,                              // "int64"
        S_U8 = 43,                               // "uint8"
        S_U16 = 44,                              // "uint16"
        S_U32 = 45,                              // "uint32"
        S_U64 = 46,                              // "uint64"
        S_F32 = 47,                              // "float"
        S_F64 = 48,                              // "double"
        S_STRING = 49,                           // "string"
        S_NEG = 50,                              // NEG
        S_YYACCEPT = 51,                         // $accept
        S_program = 52,                          // program
        S_statement = 53,                        // statement
        S_expr = 54,                             // expr
        S_term = 55,                             // term
        S_literal = 56,                          // literal
        S_list = 57,                             // list
        S_i8_list = 58,                          // i8_list
        S_i16_list = 59,                         // i16_list
        S_i32_list = 60,                         // i32_list
        S_i64_list = 61,                         // i64_list
        S_u8_list = 62,                          // u8_list
        S_u16_list = 63,                         // u16_list
        S_u32_list = 64,                         // u32_list
        S_u64_list = 65,                         // u64_list
        S_f32_list = 66,                         // f32_list
        S_f64_list = 67,                         // f64_list
        S_string_list = 68,                      // string_list
        S_function = 69,                         // function
        S_infix_function = 70,                   // infix_function
        S_named_function = 71,                   // named_function
        S_args = 72,                             // args
        S_arg = 73,                              // arg
        S_boolean = 74                           // boolean
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_statement: // statement
        value.move< Statement > (std::move (that.value));
        break;

      case symbol_kind::S_F64: // "double"
        value.move< double > (std::move (that.value));
        break;

      case symbol_kind::S_F32: // "float"
        value.move< float > (std::move (that.value));
        break;

      case symbol_kind::S_I16: // "int16"
        value.move< int16_t > (std::move (that.value));
        break;

      case symbol_kind::S_I32: // "int32"
        value.move< int32_t > (std::move (that.value));
        break;

      case symbol_kind::S_I64: // "int64"
        value.move< int64_t > (std::move (that.value));
        break;

      case symbol_kind::S_I8: // "int8"
        value.move< int8_t > (std::move (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_STRING: // "string"
        value.move< std::string > (std::move (that.value));
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_list: // list
      case symbol_kind::S_function: // function
      case symbol_kind::S_infix_function: // infix_function
      case symbol_kind::S_named_function: // named_function
      case symbol_kind::S_arg: // arg
      case symbol_kind::S_boolean: // boolean
        value.move< std::unique_ptr<jitfusion::ExecNode> > (std::move (that.value));
        break;

      case symbol_kind::S_f64_list: // f64_list
        value.move< std::vector<double> > (std::move (that.value));
        break;

      case symbol_kind::S_f32_list: // f32_list
        value.move< std::vector<float> > (std::move (that.value));
        break;

      case symbol_kind::S_i16_list: // i16_list
        value.move< std::vector<int16_t> > (std::move (that.value));
        break;

      case symbol_kind::S_i32_list: // i32_list
        value.move< std::vector<int32_t> > (std::move (that.value));
        break;

      case symbol_kind::S_i64_list: // i64_list
        value.move< std::vector<int64_t> > (std::move (that.value));
        break;

      case symbol_kind::S_i8_list: // i8_list
        value.move< std::vector<int8_t> > (std::move (that.value));
        break;

      case symbol_kind::S_string_list: // string_list
        value.move< std::vector<std::string> > (std::move (that.value));
        break;

      case symbol_kind::S_args: // args
        value.move< std::vector<std::unique_ptr<jitfusion::ExecNode>> > (std::move (that.value));
        break;

      case symbol_kind::S_u16_list: // u16_list
        value.move< std::vector<uint16_t> > (std::move (that.value));
        break;

      case symbol_kind::S_u32_list: // u32_list
        value.move< std::vector<uint32_t> > (std::move (that.value));
        break;

      case symbol_kind::S_u64_list: // u64_list
        value.move< std::vector<uint64_t> > (std::move (that.value));
        break;

      case symbol_kind::S_u8_list: // u8_list
        value.move< std::vector<uint8_t> > (std::move (that.value));
        break;

      case symbol_kind::S_U16: // "uint16"
        value.move< uint16_t > (std::move (that.value));
        break;

      case symbol_kind::S_U32: // "uint32"
        value.move< uint32_t > (std::move (that.value));
        break;

      case symbol_kind::S_U64: // "uint64"
        value.move< uint64_t > (std::move (that.value));
        break;

      case symbol_kind::S_U8: // "uint8"
        value.move< uint8_t > (std::move (that.value));
        break;

      case symbol_kind::S_program: // program
        value.move< void* > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, Statement&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const Statement& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, double&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const double& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, float&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const float& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, int16_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const int16_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, int32_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const int32_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, int64_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const int64_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, int8_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const int8_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::string&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::string& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::unique_ptr<jitfusion::ExecNode>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::unique_ptr<jitfusion::ExecNode>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<double>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<double>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<float>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<float>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<int16_t>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<int16_t>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<int32_t>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<int32_t>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<int64_t>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<int64_t>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<int8_t>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<int8_t>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<std::string>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<std::string>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<std::unique_ptr<jitfusion::ExecNode>>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<std::unique_ptr<jitfusion::ExecNode>>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<uint16_t>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<uint16_t>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<uint32_t>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<uint32_t>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<uint64_t>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<uint64_t>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<uint8_t>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<uint8_t>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, uint16_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const uint16_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, uint32_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const uint32_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, uint64_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const uint64_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, uint8_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const uint8_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, void*&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const void*& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }

#if 201103L <= YY_CPLUSPLUS
      /// Copy assignment.
      basic_symbol& operator= (const basic_symbol& that)
      {
        Base::operator= (that);
        switch (this->kind ())
    {
      case symbol_kind::S_statement: // statement
        value.copy< Statement > (that.value);
        break;

      case symbol_kind::S_F64: // "double"
        value.copy< double > (that.value);
        break;

      case symbol_kind::S_F32: // "float"
        value.copy< float > (that.value);
        break;

      case symbol_kind::S_I16: // "int16"
        value.copy< int16_t > (that.value);
        break;

      case symbol_kind::S_I32: // "int32"
        value.copy< int32_t > (that.value);
        break;

      case symbol_kind::S_I64: // "int64"
        value.copy< int64_t > (that.value);
        break;

      case symbol_kind::S_I8: // "int8"
        value.copy< int8_t > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_STRING: // "string"
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_list: // list
      case symbol_kind::S_function: // function
      case symbol_kind::S_infix_function: // infix_function
      case symbol_kind::S_named_function: // named_function
      case symbol_kind::S_arg: // arg
      case symbol_kind::S_boolean: // boolean
        value.copy< std::unique_ptr<jitfusion::ExecNode> > (that.value);
        break;

      case symbol_kind::S_f64_list: // f64_list
        value.copy< std::vector<double> > (that.value);
        break;

      case symbol_kind::S_f32_list: // f32_list
        value.copy< std::vector<float> > (that.value);
        break;

      case symbol_kind::S_i16_list: // i16_list
        value.copy< std::vector<int16_t> > (that.value);
        break;

      case symbol_kind::S_i32_list: // i32_list
        value.copy< std::vector<int32_t> > (that.value);
        break;

      case symbol_kind::S_i64_list: // i64_list
        value.copy< std::vector<int64_t> > (that.value);
        break;

      case symbol_kind::S_i8_list: // i8_list
        value.copy< std::vector<int8_t> > (that.value);
        break;

      case symbol_kind::S_string_list: // string_list
        value.copy< std::vector<std::string> > (that.value);
        break;

      case symbol_kind::S_args: // args
        value.copy< std::vector<std::unique_ptr<jitfusion::ExecNode>> > (that.value);
        break;

      case symbol_kind::S_u16_list: // u16_list
        value.copy< std::vector<uint16_t> > (that.value);
        break;

      case symbol_kind::S_u32_list: // u32_list
        value.copy< std::vector<uint32_t> > (that.value);
        break;

      case symbol_kind::S_u64_list: // u64_list
        value.copy< std::vector<uint64_t> > (that.value);
        break;

      case symbol_kind::S_u8_list: // u8_list
        value.copy< std::vector<uint8_t> > (that.value);
        break;

      case symbol_kind::S_U16: // "uint16"
        value.copy< uint16_t > (that.value);
        break;

      case symbol_kind::S_U32: // "uint32"
        value.copy< uint32_t > (that.value);
        break;

      case symbol_kind::S_U64: // "uint64"
        value.copy< uint64_t > (that.value);
        break;

      case symbol_kind::S_U8: // "uint8"
        value.copy< uint8_t > (that.value);
        break;

      case symbol_kind::S_program: // program
        value.copy< void* > (that.value);
        break;

      default:
        break;
    }
;
        location = that.location;
        return *this;
      }

      /// Move assignment.
      basic_symbol& operator= (basic_symbol&& that)
      {
        Base::operator= (std::move (that));
        switch (this->kind ())
    {
      case symbol_kind::S_statement: // statement
        value.move< Statement > (std::move (that.value));
        break;

      case symbol_kind::S_F64: // "double"
        value.move< double > (std::move (that.value));
        break;

      case symbol_kind::S_F32: // "float"
        value.move< float > (std::move (that.value));
        break;

      case symbol_kind::S_I16: // "int16"
        value.move< int16_t > (std::move (that.value));
        break;

      case symbol_kind::S_I32: // "int32"
        value.move< int32_t > (std::move (that.value));
        break;

      case symbol_kind::S_I64: // "int64"
        value.move< int64_t > (std::move (that.value));
        break;

      case symbol_kind::S_I8: // "int8"
        value.move< int8_t > (std::move (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_STRING: // "string"
        value.move< std::string > (std::move (that.value));
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_list: // list
      case symbol_kind::S_function: // function
      case symbol_kind::S_infix_function: // infix_function
      case symbol_kind::S_named_function: // named_function
      case symbol_kind::S_arg: // arg
      case symbol_kind::S_boolean: // boolean
        value.move< std::unique_ptr<jitfusion::ExecNode> > (std::move (that.value));
        break;

      case symbol_kind::S_f64_list: // f64_list
        value.move< std::vector<double> > (std::move (that.value));
        break;

      case symbol_kind::S_f32_list: // f32_list
        value.move< std::vector<float> > (std::move (that.value));
        break;

      case symbol_kind::S_i16_list: // i16_list
        value.move< std::vector<int16_t> > (std::move (that.value));
        break;

      case symbol_kind::S_i32_list: // i32_list
        value.move< std::vector<int32_t> > (std::move (that.value));
        break;

      case symbol_kind::S_i64_list: // i64_list
        value.move< std::vector<int64_t> > (std::move (that.value));
        break;

      case symbol_kind::S_i8_list: // i8_list
        value.move< std::vector<int8_t> > (std::move (that.value));
        break;

      case symbol_kind::S_string_list: // string_list
        value.move< std::vector<std::string> > (std::move (that.value));
        break;

      case symbol_kind::S_args: // args
        value.move< std::vector<std::unique_ptr<jitfusion::ExecNode>> > (std::move (that.value));
        break;

      case symbol_kind::S_u16_list: // u16_list
        value.move< std::vector<uint16_t> > (std::move (that.value));
        break;

      case symbol_kind::S_u32_list: // u32_list
        value.move< std::vector<uint32_t> > (std::move (that.value));
        break;

      case symbol_kind::S_u64_list: // u64_list
        value.move< std::vector<uint64_t> > (std::move (that.value));
        break;

      case symbol_kind::S_u8_list: // u8_list
        value.move< std::vector<uint8_t> > (std::move (that.value));
        break;

      case symbol_kind::S_U16: // "uint16"
        value.move< uint16_t > (std::move (that.value));
        break;

      case symbol_kind::S_U32: // "uint32"
        value.move< uint32_t > (std::move (that.value));
        break;

      case symbol_kind::S_U64: // "uint64"
        value.move< uint64_t > (std::move (that.value));
        break;

      case symbol_kind::S_U8: // "uint8"
        value.move< uint8_t > (std::move (that.value));
        break;

      case symbol_kind::S_program: // program
        value.move< void* > (std::move (that.value));
        break;

      default:
        break;
    }
;
        location = std::move (that.location);
        return *this;
      }
#endif

      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_statement: // statement
        value.template destroy< Statement > ();
        break;

      case symbol_kind::S_F64: // "double"
        value.template destroy< double > ();
        break;

      case symbol_kind::S_F32: // "float"
        value.template destroy< float > ();
        break;

      case symbol_kind::S_I16: // "int16"
        value.template destroy< int16_t > ();
        break;

      case symbol_kind::S_I32: // "int32"
        value.template destroy< int32_t > ();
        break;

      case symbol_kind::S_I64: // "int64"
        value.template destroy< int64_t > ();
        break;

      case symbol_kind::S_I8: // "int8"
        value.template destroy< int8_t > ();
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_STRING: // "string"
        value.template destroy< std::string > ();
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_list: // list
      case symbol_kind::S_function: // function
      case symbol_kind::S_infix_function: // infix_function
      case symbol_kind::S_named_function: // named_function
      case symbol_kind::S_arg: // arg
      case symbol_kind::S_boolean: // boolean
        value.template destroy< std::unique_ptr<jitfusion::ExecNode> > ();
        break;

      case symbol_kind::S_f64_list: // f64_list
        value.template destroy< std::vector<double> > ();
        break;

      case symbol_kind::S_f32_list: // f32_list
        value.template destroy< std::vector<float> > ();
        break;

      case symbol_kind::S_i16_list: // i16_list
        value.template destroy< std::vector<int16_t> > ();
        break;

      case symbol_kind::S_i32_list: // i32_list
        value.template destroy< std::vector<int32_t> > ();
        break;

      case symbol_kind::S_i64_list: // i64_list
        value.template destroy< std::vector<int64_t> > ();
        break;

      case symbol_kind::S_i8_list: // i8_list
        value.template destroy< std::vector<int8_t> > ();
        break;

      case symbol_kind::S_string_list: // string_list
        value.template destroy< std::vector<std::string> > ();
        break;

      case symbol_kind::S_args: // args
        value.template destroy< std::vector<std::unique_ptr<jitfusion::ExecNode>> > ();
        break;

      case symbol_kind::S_u16_list: // u16_list
        value.template destroy< std::vector<uint16_t> > ();
        break;

      case symbol_kind::S_u32_list: // u32_list
        value.template destroy< std::vector<uint32_t> > ();
        break;

      case symbol_kind::S_u64_list: // u64_list
        value.template destroy< std::vector<uint64_t> > ();
        break;

      case symbol_kind::S_u8_list: // u8_list
        value.template destroy< std::vector<uint8_t> > ();
        break;

      case symbol_kind::S_U16: // "uint16"
        value.template destroy< uint16_t > ();
        break;

      case symbol_kind::S_U32: // "uint32"
        value.template destroy< uint32_t > ();
        break;

      case symbol_kind::S_U64: // "uint64"
        value.template destroy< uint64_t > ();
        break;

      case symbol_kind::S_U8: // "uint8"
        value.template destroy< uint8_t > ();
        break;

      case symbol_kind::S_program: // program
        value.template destroy< void* > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// The user-facing name of this symbol.
      const char *name () const YY_NOEXCEPT
      {
        return Parser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Copy assignment.
      by_kind& operator= (const by_kind& that);

      /// Move assignment.
      by_kind& operator= (by_kind&& that);
#endif

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_YYEOF
                   || (token::TOK_YYerror <= tok && tok <= token::TOK_OUTPUT)
                   || tok == token::TOK_NEG);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, double v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const double& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_F64);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, float v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const float& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_F32);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, int16_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const int16_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_I16);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, int32_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const int32_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_I32);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, int64_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const int64_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_I64);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, int8_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const int8_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_I8);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, std::string v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const std::string& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_IDENTIFIER
                   || tok == token::TOK_STRING);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, uint16_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const uint16_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_U16);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, uint32_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const uint32_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_U32);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, uint64_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const uint64_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_U64);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, uint8_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const uint8_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOK_U8);
#endif
      }
    };

    /// Build a parser object.
    Parser (yyscan_t scanner_yyarg, ProgramAstBuilder &builder_yyarg);
    virtual ~Parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    Parser (const Parser&) = delete;
    /// Non copyable.
    Parser& operator= (const Parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static const char *symbol_name (symbol_kind_type yysymbol);

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYEOF (location_type l)
      {
        return symbol_type (token::TOK_YYEOF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYEOF (const location_type& l)
      {
        return symbol_type (token::TOK_YYEOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::TOK_YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::TOK_YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::TOK_YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::TOK_YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IF (location_type l)
      {
        return symbol_type (token::TOK_IF, std::move (l));
      }
#else
      static
      symbol_type
      make_IF (const location_type& l)
      {
        return symbol_type (token::TOK_IF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SWITCH (location_type l)
      {
        return symbol_type (token::TOK_SWITCH, std::move (l));
      }
#else
      static
      symbol_type
      make_SWITCH (const location_type& l)
      {
        return symbol_type (token::TOK_SWITCH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IN (location_type l)
      {
        return symbol_type (token::TOK_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_IN (const location_type& l)
      {
        return symbol_type (token::TOK_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NOT (location_type l)
      {
        return symbol_type (token::TOK_NOT, std::move (l));
      }
#else
      static
      symbol_type
      make_NOT (const location_type& l)
      {
        return symbol_type (token::TOK_NOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AND (location_type l)
      {
        return symbol_type (token::TOK_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_AND (const location_type& l)
      {
        return symbol_type (token::TOK_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OR (location_type l)
      {
        return symbol_type (token::TOK_OR, std::move (l));
      }
#else
      static
      symbol_type
      make_OR (const location_type& l)
      {
        return symbol_type (token::TOK_OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TRUE (location_type l)
      {
        return symbol_type (token::TOK_TRUE, std::move (l));
      }
#else
      static
      symbol_type
      make_TRUE (const location_type& l)
      {
        return symbol_type (token::TOK_TRUE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FALSE (location_type l)
      {
        return symbol_type (token::TOK_FALSE, std::move (l));
      }
#else
      static
      symbol_type
      make_FALSE (const location_type& l)
      {
        return symbol_type (token::TOK_FALSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MINUS (location_type l)
      {
        return symbol_type (token::TOK_MINUS, std::move (l));
      }
#else
      static
      symbol_type
      make_MINUS (const location_type& l)
      {
        return symbol_type (token::TOK_MINUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PLUS (location_type l)
      {
        return symbol_type (token::TOK_PLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_PLUS (const location_type& l)
      {
        return symbol_type (token::TOK_PLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STAR (location_type l)
      {
        return symbol_type (token::TOK_STAR, std::move (l));
      }
#else
      static
      symbol_type
      make_STAR (const location_type& l)
      {
        return symbol_type (token::TOK_STAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SLASH (location_type l)
      {
        return symbol_type (token::TOK_SLASH, std::move (l));
      }
#else
      static
      symbol_type
      make_SLASH (const location_type& l)
      {
        return symbol_type (token::TOK_SLASH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LPAREN (location_type l)
      {
        return symbol_type (token::TOK_LPAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_LPAREN (const location_type& l)
      {
        return symbol_type (token::TOK_LPAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RPAREN (location_type l)
      {
        return symbol_type (token::TOK_RPAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_RPAREN (const location_type& l)
      {
        return symbol_type (token::TOK_RPAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MODOLO (location_type l)
      {
        return symbol_type (token::TOK_MODOLO, std::move (l));
      }
#else
      static
      symbol_type
      make_MODOLO (const location_type& l)
      {
        return symbol_type (token::TOK_MODOLO, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BITWISE_OR (location_type l)
      {
        return symbol_type (token::TOK_BITWISE_OR, std::move (l));
      }
#else
      static
      symbol_type
      make_BITWISE_OR (const location_type& l)
      {
        return symbol_type (token::TOK_BITWISE_OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BITWISE_XOR (location_type l)
      {
        return symbol_type (token::TOK_BITWISE_XOR, std::move (l));
      }
#else
      static
      symbol_type
      make_BITWISE_XOR (const location_type& l)
      {
        return symbol_type (token::TOK_BITWISE_XOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BITWISE_AND (location_type l)
      {
        return symbol_type (token::TOK_BITWISE_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_BITWISE_AND (const location_type& l)
      {
        return symbol_type (token::TOK_BITWISE_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BITWISE_NOT (location_type l)
      {
        return symbol_type (token::TOK_BITWISE_NOT, std::move (l));
      }
#else
      static
      symbol_type
      make_BITWISE_NOT (const location_type& l)
      {
        return symbol_type (token::TOK_BITWISE_NOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BITWISE_LEFT (location_type l)
      {
        return symbol_type (token::TOK_BITWISE_LEFT, std::move (l));
      }
#else
      static
      symbol_type
      make_BITWISE_LEFT (const location_type& l)
      {
        return symbol_type (token::TOK_BITWISE_LEFT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BITWISE_RIGHT (location_type l)
      {
        return symbol_type (token::TOK_BITWISE_RIGHT, std::move (l));
      }
#else
      static
      symbol_type
      make_BITWISE_RIGHT (const location_type& l)
      {
        return symbol_type (token::TOK_BITWISE_RIGHT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EQUAL (location_type l)
      {
        return symbol_type (token::TOK_EQUAL, std::move (l));
      }
#else
      static
      symbol_type
      make_EQUAL (const location_type& l)
      {
        return symbol_type (token::TOK_EQUAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ASSIGNMENT (location_type l)
      {
        return symbol_type (token::TOK_ASSIGNMENT, std::move (l));
      }
#else
      static
      symbol_type
      make_ASSIGNMENT (const location_type& l)
      {
        return symbol_type (token::TOK_ASSIGNMENT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NOT_EQUAL (location_type l)
      {
        return symbol_type (token::TOK_NOT_EQUAL, std::move (l));
      }
#else
      static
      symbol_type
      make_NOT_EQUAL (const location_type& l)
      {
        return symbol_type (token::TOK_NOT_EQUAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LESS_THAN (location_type l)
      {
        return symbol_type (token::TOK_LESS_THAN, std::move (l));
      }
#else
      static
      symbol_type
      make_LESS_THAN (const location_type& l)
      {
        return symbol_type (token::TOK_LESS_THAN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LESS_THAN_OR_EQUAL_TO (location_type l)
      {
        return symbol_type (token::TOK_LESS_THAN_OR_EQUAL_TO, std::move (l));
      }
#else
      static
      symbol_type
      make_LESS_THAN_OR_EQUAL_TO (const location_type& l)
      {
        return symbol_type (token::TOK_LESS_THAN_OR_EQUAL_TO, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GREATER_THAN (location_type l)
      {
        return symbol_type (token::TOK_GREATER_THAN, std::move (l));
      }
#else
      static
      symbol_type
      make_GREATER_THAN (const location_type& l)
      {
        return symbol_type (token::TOK_GREATER_THAN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GREATER_THAN_OR_EQUAL_TO (location_type l)
      {
        return symbol_type (token::TOK_GREATER_THAN_OR_EQUAL_TO, std::move (l));
      }
#else
      static
      symbol_type
      make_GREATER_THAN_OR_EQUAL_TO (const location_type& l)
      {
        return symbol_type (token::TOK_GREATER_THAN_OR_EQUAL_TO, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COMMA (location_type l)
      {
        return symbol_type (token::TOK_COMMA, std::move (l));
      }
#else
      static
      symbol_type
      make_COMMA (const location_type& l)
      {
        return symbol_type (token::TOK_COMMA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LBRACKET (location_type l)
      {
        return symbol_type (token::TOK_LBRACKET, std::move (l));
      }
#else
      static
      symbol_type
      make_LBRACKET (const location_type& l)
      {
        return symbol_type (token::TOK_LBRACKET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RBRACKET (location_type l)
      {
        return symbol_type (token::TOK_RBRACKET, std::move (l));
      }
#else
      static
      symbol_type
      make_RBRACKET (const location_type& l)
      {
        return symbol_type (token::TOK_RBRACKET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SEMI (location_type l)
      {
        return symbol_type (token::TOK_SEMI, std::move (l));
      }
#else
      static
      symbol_type
      make_SEMI (const location_type& l)
      {
        return symbol_type (token::TOK_SEMI, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ENTRY_ARG (location_type l)
      {
        return symbol_type (token::TOK_ENTRY_ARG, std::move (l));
      }
#else
      static
      symbol_type
      make_ENTRY_ARG (const location_type& l)
      {
        return symbol_type (token::TOK_ENTRY_ARG, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EXEC_CTX (location_type l)
      {
        return symbol_type (token::TOK_EXEC_CTX, std::move (l));
      }
#else
      static
      symbol_type
      make_EXEC_CTX (const location_type& l)
      {
        return symbol_type (token::TOK_EXEC_CTX, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OUTPUT (location_type l)
      {
        return symbol_type (token::TOK_OUTPUT, std::move (l));
      }
#else
      static
      symbol_type
      make_OUTPUT (const location_type& l)
      {
        return symbol_type (token::TOK_OUTPUT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IDENTIFIER (std::string v, location_type l)
      {
        return symbol_type (token::TOK_IDENTIFIER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_IDENTIFIER (const std::string& v, const location_type& l)
      {
        return symbol_type (token::TOK_IDENTIFIER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_I8 (int8_t v, location_type l)
      {
        return symbol_type (token::TOK_I8, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_I8 (const int8_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_I8, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_I16 (int16_t v, location_type l)
      {
        return symbol_type (token::TOK_I16, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_I16 (const int16_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_I16, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_I32 (int32_t v, location_type l)
      {
        return symbol_type (token::TOK_I32, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_I32 (const int32_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_I32, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_I64 (int64_t v, location_type l)
      {
        return symbol_type (token::TOK_I64, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_I64 (const int64_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_I64, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_U8 (uint8_t v, location_type l)
      {
        return symbol_type (token::TOK_U8, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_U8 (const uint8_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_U8, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_U16 (uint16_t v, location_type l)
      {
        return symbol_type (token::TOK_U16, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_U16 (const uint16_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_U16, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_U32 (uint32_t v, location_type l)
      {
        return symbol_type (token::TOK_U32, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_U32 (const uint32_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_U32, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_U64 (uint64_t v, location_type l)
      {
        return symbol_type (token::TOK_U64, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_U64 (const uint64_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_U64, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_F32 (float v, location_type l)
      {
        return symbol_type (token::TOK_F32, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_F32 (const float& v, const location_type& l)
      {
        return symbol_type (token::TOK_F32, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_F64 (double v, location_type l)
      {
        return symbol_type (token::TOK_F64, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_F64 (const double& v, const location_type& l)
      {
        return symbol_type (token::TOK_F64, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING (std::string v, location_type l)
      {
        return symbol_type (token::TOK_STRING, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_STRING (const std::string& v, const location_type& l)
      {
        return symbol_type (token::TOK_STRING, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NEG (location_type l)
      {
        return symbol_type (token::TOK_NEG, std::move (l));
      }
#else
      static
      symbol_type
      make_NEG (const location_type& l)
      {
        return symbol_type (token::TOK_NEG, l);
      }
#endif


    class context
    {
    public:
      context (const Parser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const Parser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    Parser (const Parser&);
    /// Non copyable.
    Parser& operator= (const Parser&);
#endif

    /// Check the lookahead yytoken.
    /// \returns  true iff the token will be eventually shifted.
    bool yy_lac_check_ (symbol_kind_type yytoken) const;
    /// Establish the initial context if no initial context currently exists.
    /// \returns  true iff the token will be eventually shifted.
    bool yy_lac_establish_ (symbol_kind_type yytoken);
    /// Discard any previous initial lookahead context because of event.
    /// \param event  the event which caused the lookahead to be discarded.
    ///               Only used for debbuging output.
    void yy_lac_discard_ (const char* event);

    /// Stored state numbers (used for stacks).
    typedef unsigned char state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const signed char yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;



    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const signed char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const signed char yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const unsigned char yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const signed char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type () YY_NOEXCEPT;
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;
    /// The stack for LAC.
    /// Logically, the yy_lac_stack's lifetime is confined to the function
    /// yy_lac_check_. We just store it as a member of this class to hold
    /// on to the memory and to avoid frequent reallocations.
    /// Since yy_lac_check_ is const, this member must be mutable.
    mutable std::vector<state_type> yylac_stack_;
    /// Whether an initial LAC context was established.
    bool yy_lac_established_;


    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 307,     ///< Last index in yytable_.
      yynnts_ = 24,  ///< Number of nonterminal symbols.
      yyfinal_ = 5 ///< Termination state number.
    };


    // User arguments.
    yyscan_t scanner;
    ProgramAstBuilder &builder;

  };

  inline
  Parser::symbol_kind_type
  Parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    return static_cast<symbol_kind_type> (t);
  }

  // basic_symbol.
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_statement: // statement
        value.copy< Statement > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_F64: // "double"
        value.copy< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_F32: // "float"
        value.copy< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I16: // "int16"
        value.copy< int16_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I32: // "int32"
        value.copy< int32_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I64: // "int64"
        value.copy< int64_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I8: // "int8"
        value.copy< int8_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_STRING: // "string"
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_list: // list
      case symbol_kind::S_function: // function
      case symbol_kind::S_infix_function: // infix_function
      case symbol_kind::S_named_function: // named_function
      case symbol_kind::S_arg: // arg
      case symbol_kind::S_boolean: // boolean
        value.copy< std::unique_ptr<jitfusion::ExecNode> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_f64_list: // f64_list
        value.copy< std::vector<double> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_f32_list: // f32_list
        value.copy< std::vector<float> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i16_list: // i16_list
        value.copy< std::vector<int16_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i32_list: // i32_list
        value.copy< std::vector<int32_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i64_list: // i64_list
        value.copy< std::vector<int64_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i8_list: // i8_list
        value.copy< std::vector<int8_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string_list: // string_list
        value.copy< std::vector<std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_args: // args
        value.copy< std::vector<std::unique_ptr<jitfusion::ExecNode>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u16_list: // u16_list
        value.copy< std::vector<uint16_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u32_list: // u32_list
        value.copy< std::vector<uint32_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u64_list: // u64_list
        value.copy< std::vector<uint64_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u8_list: // u8_list
        value.copy< std::vector<uint8_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U16: // "uint16"
        value.copy< uint16_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U32: // "uint32"
        value.copy< uint32_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U64: // "uint64"
        value.copy< uint64_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U8: // "uint8"
        value.copy< uint8_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.copy< void* > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  Parser::symbol_kind_type
  Parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  Parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_statement: // statement
        value.move< Statement > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_F64: // "double"
        value.move< double > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_F32: // "float"
        value.move< float > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_I16: // "int16"
        value.move< int16_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_I32: // "int32"
        value.move< int32_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_I64: // "int64"
        value.move< int64_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_I8: // "int8"
        value.move< int8_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_STRING: // "string"
        value.move< std::string > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_list: // list
      case symbol_kind::S_function: // function
      case symbol_kind::S_infix_function: // infix_function
      case symbol_kind::S_named_function: // named_function
      case symbol_kind::S_arg: // arg
      case symbol_kind::S_boolean: // boolean
        value.move< std::unique_ptr<jitfusion::ExecNode> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_f64_list: // f64_list
        value.move< std::vector<double> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_f32_list: // f32_list
        value.move< std::vector<float> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_i16_list: // i16_list
        value.move< std::vector<int16_t> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_i32_list: // i32_list
        value.move< std::vector<int32_t> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_i64_list: // i64_list
        value.move< std::vector<int64_t> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_i8_list: // i8_list
        value.move< std::vector<int8_t> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_string_list: // string_list
        value.move< std::vector<std::string> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_args: // args
        value.move< std::vector<std::unique_ptr<jitfusion::ExecNode>> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_u16_list: // u16_list
        value.move< std::vector<uint16_t> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_u32_list: // u32_list
        value.move< std::vector<uint32_t> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_u64_list: // u64_list
        value.move< std::vector<uint64_t> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_u8_list: // u8_list
        value.move< std::vector<uint8_t> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_U16: // "uint16"
        value.move< uint16_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_U32: // "uint32"
        value.move< uint32_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_U64: // "uint64"
        value.move< uint64_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_U8: // "uint8"
        value.move< uint8_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_program: // program
        value.move< void* > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  inline
  Parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
  Parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  inline
  Parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  inline
  Parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
  Parser::by_kind&
  Parser::by_kind::by_kind::operator= (const by_kind& that)
  {
    kind_ = that.kind_;
    return *this;
  }

  inline
  Parser::by_kind&
  Parser::by_kind::by_kind::operator= (by_kind&& that)
  {
    kind_ = that.kind_;
    that.clear ();
    return *this;
  }
#endif

  inline
  void
  Parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  inline
  void
  Parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  inline
  Parser::symbol_kind_type
  Parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  inline
  Parser::symbol_kind_type
  Parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


#line 10 "parser.yy"
} // athena
#line 3394 "parser.hh"


// "%code provides" blocks.
#line 28 "parser.yy"

// Give Flex the prototype of yylex we want ...
#define YY_DECL athena::Parser::symbol_type athena_yylex(yyscan_t yyscanner, athena::ProgramAstBuilder &builder)
// ... and declare it for the parser's sake.
YY_DECL;

#line 3405 "parser.hh"


#endif // !YY_YY_PARSER_HH_INCLUDED
