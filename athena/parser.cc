// A Bison parser, made by GNU Bison 3.8.2.45-25b3d-dirty.

// Skeleton implementation for Bison LALR(1) parsers in C++

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

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "parser.hh"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 10 "parser.yy"
namespace athena {
#line 137 "parser.cc"

  /// Build a parser object.
  Parser::Parser (yyscan_t scanner_yyarg, ProgramAstBuilder &builder_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      yy_lac_established_ (false),
      scanner (scanner_yyarg),
      builder (builder_yyarg)
  {}

  Parser::~Parser ()
  {}

  Parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  Parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  Parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  Parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  Parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  Parser::symbol_kind_type
  Parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  Parser::stack_symbol_type::stack_symbol_type () YY_NOEXCEPT
  {}

  Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_statement: // statement
        value.YY_MOVE_OR_COPY< Statement > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_F64: // "double"
        value.YY_MOVE_OR_COPY< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_F32: // "float"
        value.YY_MOVE_OR_COPY< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I16: // "int16"
        value.YY_MOVE_OR_COPY< int16_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I32: // "int32"
        value.YY_MOVE_OR_COPY< int32_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I64: // "int64"
        value.YY_MOVE_OR_COPY< int64_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I8: // "int8"
        value.YY_MOVE_OR_COPY< int8_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_STRING: // "string"
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
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
        value.YY_MOVE_OR_COPY< std::unique_ptr<jitfusion::ExecNode> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_f64_list: // f64_list
        value.YY_MOVE_OR_COPY< std::vector<double> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_f32_list: // f32_list
        value.YY_MOVE_OR_COPY< std::vector<float> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i16_list: // i16_list
        value.YY_MOVE_OR_COPY< std::vector<int16_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i32_list: // i32_list
        value.YY_MOVE_OR_COPY< std::vector<int32_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i64_list: // i64_list
        value.YY_MOVE_OR_COPY< std::vector<int64_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i8_list: // i8_list
        value.YY_MOVE_OR_COPY< std::vector<int8_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string_list: // string_list
        value.YY_MOVE_OR_COPY< std::vector<std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_args: // args
        value.YY_MOVE_OR_COPY< std::vector<std::unique_ptr<jitfusion::ExecNode>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u16_list: // u16_list
        value.YY_MOVE_OR_COPY< std::vector<uint16_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u32_list: // u32_list
        value.YY_MOVE_OR_COPY< std::vector<uint32_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u64_list: // u64_list
        value.YY_MOVE_OR_COPY< std::vector<uint64_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u8_list: // u8_list
        value.YY_MOVE_OR_COPY< std::vector<uint8_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U16: // "uint16"
        value.YY_MOVE_OR_COPY< uint16_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U32: // "uint32"
        value.YY_MOVE_OR_COPY< uint32_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U64: // "uint64"
        value.YY_MOVE_OR_COPY< uint64_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U8: // "uint8"
        value.YY_MOVE_OR_COPY< uint8_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.YY_MOVE_OR_COPY< void* > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_statement: // statement
        value.move< Statement > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_F64: // "double"
        value.move< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_F32: // "float"
        value.move< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I16: // "int16"
        value.move< int16_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I32: // "int32"
        value.move< int32_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I64: // "int64"
        value.move< int64_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_I8: // "int8"
        value.move< int8_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_STRING: // "string"
        value.move< std::string > (YY_MOVE (that.value));
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
        value.move< std::unique_ptr<jitfusion::ExecNode> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_f64_list: // f64_list
        value.move< std::vector<double> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_f32_list: // f32_list
        value.move< std::vector<float> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i16_list: // i16_list
        value.move< std::vector<int16_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i32_list: // i32_list
        value.move< std::vector<int32_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i64_list: // i64_list
        value.move< std::vector<int64_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_i8_list: // i8_list
        value.move< std::vector<int8_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string_list: // string_list
        value.move< std::vector<std::string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_args: // args
        value.move< std::vector<std::unique_ptr<jitfusion::ExecNode>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u16_list: // u16_list
        value.move< std::vector<uint16_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u32_list: // u32_list
        value.move< std::vector<uint32_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u64_list: // u64_list
        value.move< std::vector<uint64_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_u8_list: // u8_list
        value.move< std::vector<uint8_t> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U16: // "uint16"
        value.move< uint16_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U32: // "uint32"
        value.move< uint32_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U64: // "uint64"
        value.move< uint64_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_U8: // "uint8"
        value.move< uint8_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.move< void* > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
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

    location = that.location;
    return *this;
  }

  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_statement: // statement
        value.move< Statement > (that.value);
        break;

      case symbol_kind::S_F64: // "double"
        value.move< double > (that.value);
        break;

      case symbol_kind::S_F32: // "float"
        value.move< float > (that.value);
        break;

      case symbol_kind::S_I16: // "int16"
        value.move< int16_t > (that.value);
        break;

      case symbol_kind::S_I32: // "int32"
        value.move< int32_t > (that.value);
        break;

      case symbol_kind::S_I64: // "int64"
        value.move< int64_t > (that.value);
        break;

      case symbol_kind::S_I8: // "int8"
        value.move< int8_t > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_STRING: // "string"
        value.move< std::string > (that.value);
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
        value.move< std::unique_ptr<jitfusion::ExecNode> > (that.value);
        break;

      case symbol_kind::S_f64_list: // f64_list
        value.move< std::vector<double> > (that.value);
        break;

      case symbol_kind::S_f32_list: // f32_list
        value.move< std::vector<float> > (that.value);
        break;

      case symbol_kind::S_i16_list: // i16_list
        value.move< std::vector<int16_t> > (that.value);
        break;

      case symbol_kind::S_i32_list: // i32_list
        value.move< std::vector<int32_t> > (that.value);
        break;

      case symbol_kind::S_i64_list: // i64_list
        value.move< std::vector<int64_t> > (that.value);
        break;

      case symbol_kind::S_i8_list: // i8_list
        value.move< std::vector<int8_t> > (that.value);
        break;

      case symbol_kind::S_string_list: // string_list
        value.move< std::vector<std::string> > (that.value);
        break;

      case symbol_kind::S_args: // args
        value.move< std::vector<std::unique_ptr<jitfusion::ExecNode>> > (that.value);
        break;

      case symbol_kind::S_u16_list: // u16_list
        value.move< std::vector<uint16_t> > (that.value);
        break;

      case symbol_kind::S_u32_list: // u32_list
        value.move< std::vector<uint32_t> > (that.value);
        break;

      case symbol_kind::S_u64_list: // u64_list
        value.move< std::vector<uint64_t> > (that.value);
        break;

      case symbol_kind::S_u8_list: // u8_list
        value.move< std::vector<uint8_t> > (that.value);
        break;

      case symbol_kind::S_U16: // "uint16"
        value.move< uint16_t > (that.value);
        break;

      case symbol_kind::S_U32: // "uint32"
        value.move< uint32_t > (that.value);
        break;

      case symbol_kind::S_U64: // "uint64"
        value.move< uint64_t > (that.value);
        break;

      case symbol_kind::S_U8: // "uint8"
        value.move< uint8_t > (that.value);
        break;

      case symbol_kind::S_program: // program
        value.move< void* > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  Parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  Parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  Parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::operator() ()
  {
    return parse ();
  }

  int
  Parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ YY_ATTRIBUTE_UNUSED = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // Discard the LAC context in case there still is one left from a
    // previous invocation.
    yy_lac_discard_ ("init");

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (scanner, builder));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;

        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    yy_lac_discard_ ("shift");
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_statement: // statement
        yylhs.value.emplace< Statement > ();
        break;

      case symbol_kind::S_F64: // "double"
        yylhs.value.emplace< double > ();
        break;

      case symbol_kind::S_F32: // "float"
        yylhs.value.emplace< float > ();
        break;

      case symbol_kind::S_I16: // "int16"
        yylhs.value.emplace< int16_t > ();
        break;

      case symbol_kind::S_I32: // "int32"
        yylhs.value.emplace< int32_t > ();
        break;

      case symbol_kind::S_I64: // "int64"
        yylhs.value.emplace< int64_t > ();
        break;

      case symbol_kind::S_I8: // "int8"
        yylhs.value.emplace< int8_t > ();
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_STRING: // "string"
        yylhs.value.emplace< std::string > ();
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
        yylhs.value.emplace< std::unique_ptr<jitfusion::ExecNode> > ();
        break;

      case symbol_kind::S_f64_list: // f64_list
        yylhs.value.emplace< std::vector<double> > ();
        break;

      case symbol_kind::S_f32_list: // f32_list
        yylhs.value.emplace< std::vector<float> > ();
        break;

      case symbol_kind::S_i16_list: // i16_list
        yylhs.value.emplace< std::vector<int16_t> > ();
        break;

      case symbol_kind::S_i32_list: // i32_list
        yylhs.value.emplace< std::vector<int32_t> > ();
        break;

      case symbol_kind::S_i64_list: // i64_list
        yylhs.value.emplace< std::vector<int64_t> > ();
        break;

      case symbol_kind::S_i8_list: // i8_list
        yylhs.value.emplace< std::vector<int8_t> > ();
        break;

      case symbol_kind::S_string_list: // string_list
        yylhs.value.emplace< std::vector<std::string> > ();
        break;

      case symbol_kind::S_args: // args
        yylhs.value.emplace< std::vector<std::unique_ptr<jitfusion::ExecNode>> > ();
        break;

      case symbol_kind::S_u16_list: // u16_list
        yylhs.value.emplace< std::vector<uint16_t> > ();
        break;

      case symbol_kind::S_u32_list: // u32_list
        yylhs.value.emplace< std::vector<uint32_t> > ();
        break;

      case symbol_kind::S_u64_list: // u64_list
        yylhs.value.emplace< std::vector<uint64_t> > ();
        break;

      case symbol_kind::S_u8_list: // u8_list
        yylhs.value.emplace< std::vector<uint8_t> > ();
        break;

      case symbol_kind::S_U16: // "uint16"
        yylhs.value.emplace< uint16_t > ();
        break;

      case symbol_kind::S_U32: // "uint32"
        yylhs.value.emplace< uint32_t > ();
        break;

      case symbol_kind::S_U64: // "uint64"
        yylhs.value.emplace< uint64_t > ();
        break;

      case symbol_kind::S_U8: // "uint8"
        yylhs.value.emplace< uint8_t > ();
        break;

      case symbol_kind::S_program: // program
        yylhs.value.emplace< void* > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // program: statement
#line 150 "parser.yy"
            { 
    builder.AddStatement(std::move(yystack_[0].value.as < Statement > ()));
    yylhs.value.as < void* > () = nullptr;
  }
#line 1111 "parser.cc"
    break;

  case 3: // program: program statement
#line 154 "parser.yy"
                    {
  builder.AddStatement(std::move(yystack_[0].value.as < Statement > ()));
  yylhs.value.as < void* > () = nullptr;
}
#line 1120 "parser.cc"
    break;

  case 4: // statement: "identifier" "=" expr ";"
#line 159 "parser.yy"
                                   { yylhs.value.as < Statement > () = Statement(std::move(yystack_[3].value.as < std::string > ()), std::move(yystack_[1].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1126 "parser.cc"
    break;

  case 5: // expr: term
#line 161 "parser.yy"
           { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ());}
#line 1132 "parser.cc"
    break;

  case 6: // term: literal
#line 165 "parser.yy"
          { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ()); }
#line 1138 "parser.cc"
    break;

  case 7: // term: function
#line 166 "parser.yy"
           { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ()); }
#line 1144 "parser.cc"
    break;

  case 8: // term: boolean
#line 167 "parser.yy"
          { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ()); }
#line 1150 "parser.cc"
    break;

  case 9: // term: "(" term ")"
#line 168 "parser.yy"
               { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::move(yystack_[1].value.as < std::unique_ptr<jitfusion::ExecNode> > ()); }
#line 1156 "parser.cc"
    break;

  case 10: // term: "identifier"
#line 169 "parser.yy"
             { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = builder.MakeRefNode(std::move(yystack_[0].value.as < std::string > ())); }
#line 1162 "parser.cc"
    break;

  case 11: // term: "entry_arg"
#line 170 "parser.yy"
            { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::unique_ptr<jitfusion::ExecNode>(new jitfusion::EntryArgumentNode); }
#line 1168 "parser.cc"
    break;

  case 12: // term: "exec_ctx"
#line 171 "parser.yy"
           { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::unique_ptr<jitfusion::ExecNode>(new jitfusion::ExecContextNode); }
#line 1174 "parser.cc"
    break;

  case 13: // literal: "int8"
#line 175 "parser.yy"
     { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < int8_t > ()); }
#line 1180 "parser.cc"
    break;

  case 14: // literal: "int16"
#line 176 "parser.yy"
      { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < int16_t > ()); }
#line 1186 "parser.cc"
    break;

  case 15: // literal: "int32"
#line 177 "parser.yy"
      { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < int32_t > ()); }
#line 1192 "parser.cc"
    break;

  case 16: // literal: "int64"
#line 178 "parser.yy"
      { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < int64_t > ()); }
#line 1198 "parser.cc"
    break;

  case 17: // literal: "uint8"
#line 179 "parser.yy"
     { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < uint8_t > ()); }
#line 1204 "parser.cc"
    break;

  case 18: // literal: "uint16"
#line 180 "parser.yy"
      { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < uint16_t > ()); }
#line 1210 "parser.cc"
    break;

  case 19: // literal: "uint32"
#line 181 "parser.yy"
      { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < uint32_t > ()); }
#line 1216 "parser.cc"
    break;

  case 20: // literal: "uint64"
#line 182 "parser.yy"
      { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < uint64_t > ()); }
#line 1222 "parser.cc"
    break;

  case 21: // literal: "float"
#line 183 "parser.yy"
      { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < float > ()); }
#line 1228 "parser.cc"
    break;

  case 22: // literal: "double"
#line 184 "parser.yy"
      { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < double > ()); }
#line 1234 "parser.cc"
    break;

  case 23: // literal: "string"
#line 185 "parser.yy"
         { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(yystack_[0].value.as < std::string > ()); }
#line 1240 "parser.cc"
    break;

  case 24: // literal: list
#line 186 "parser.yy"
       { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ()); }
#line 1246 "parser.cc"
    break;

  case 25: // literal: "true"
#line 187 "parser.yy"
         { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(uint8_t(1)); }
#line 1252 "parser.cc"
    break;

  case 26: // literal: "false"
#line 188 "parser.yy"
          { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantValueNode>(uint8_t(0)); }
#line 1258 "parser.cc"
    break;

  case 27: // list: "[" i8_list "]"
#line 192 "parser.yy"
                  { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<int8_t> > ()); }
#line 1264 "parser.cc"
    break;

  case 28: // list: "[" i16_list "]"
#line 193 "parser.yy"
                   { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<int16_t> > ()); }
#line 1270 "parser.cc"
    break;

  case 29: // list: "[" i32_list "]"
#line 194 "parser.yy"
                   { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<int32_t> > ()); }
#line 1276 "parser.cc"
    break;

  case 30: // list: "[" i64_list "]"
#line 195 "parser.yy"
                   { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<int64_t> > ()); }
#line 1282 "parser.cc"
    break;

  case 31: // list: "[" u8_list "]"
#line 196 "parser.yy"
                  { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<uint8_t> > ()); }
#line 1288 "parser.cc"
    break;

  case 32: // list: "[" u16_list "]"
#line 197 "parser.yy"
                   { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<uint16_t> > ()); }
#line 1294 "parser.cc"
    break;

  case 33: // list: "[" u32_list "]"
#line 198 "parser.yy"
                   { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<uint32_t> > ()); }
#line 1300 "parser.cc"
    break;

  case 34: // list: "[" u64_list "]"
#line 199 "parser.yy"
                   { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<uint64_t> > ()); }
#line 1306 "parser.cc"
    break;

  case 35: // list: "[" f32_list "]"
#line 200 "parser.yy"
                   { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<float> > ()); }
#line 1312 "parser.cc"
    break;

  case 36: // list: "[" f64_list "]"
#line 201 "parser.yy"
                   { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<double> > ()); }
#line 1318 "parser.cc"
    break;

  case 37: // list: "[" string_list "]"
#line 202 "parser.yy"
                      { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::ConstantListValueNode>(yystack_[1].value.as < std::vector<std::string> > ()); }
#line 1324 "parser.cc"
    break;

  case 38: // i8_list: "int8"
#line 206 "parser.yy"
     { yylhs.value.as < std::vector<int8_t> > () = std::vector<int8_t>{yystack_[0].value.as < int8_t > ()}; }
#line 1330 "parser.cc"
    break;

  case 39: // i8_list: i8_list "," "int8"
#line 207 "parser.yy"
                 {
  yystack_[2].value.as < std::vector<int8_t> > ().emplace_back(std::move(yystack_[0].value.as < int8_t > ()));
  yylhs.value.as < std::vector<int8_t> > () = std::move(yystack_[2].value.as < std::vector<int8_t> > ());
}
#line 1339 "parser.cc"
    break;

  case 40: // i16_list: "int16"
#line 214 "parser.yy"
      { yylhs.value.as < std::vector<int16_t> > () = std::vector<int16_t>{yystack_[0].value.as < int16_t > ()}; }
#line 1345 "parser.cc"
    break;

  case 41: // i16_list: i16_list "," "int16"
#line 215 "parser.yy"
                   {
  yystack_[2].value.as < std::vector<int16_t> > ().emplace_back(std::move(yystack_[0].value.as < int16_t > ()));
  yylhs.value.as < std::vector<int16_t> > () = std::move(yystack_[2].value.as < std::vector<int16_t> > ());
}
#line 1354 "parser.cc"
    break;

  case 42: // i32_list: "int32"
#line 222 "parser.yy"
      { yylhs.value.as < std::vector<int32_t> > () = std::vector<int32_t>{yystack_[0].value.as < int32_t > ()}; }
#line 1360 "parser.cc"
    break;

  case 43: // i32_list: i32_list "," "int32"
#line 223 "parser.yy"
                   {
  yystack_[2].value.as < std::vector<int32_t> > ().emplace_back(std::move(yystack_[0].value.as < int32_t > ()));
  yylhs.value.as < std::vector<int32_t> > () = std::move(yystack_[2].value.as < std::vector<int32_t> > ());
}
#line 1369 "parser.cc"
    break;

  case 44: // i64_list: "int64"
#line 230 "parser.yy"
      { yylhs.value.as < std::vector<int64_t> > () = std::vector<int64_t>{yystack_[0].value.as < int64_t > ()}; }
#line 1375 "parser.cc"
    break;

  case 45: // i64_list: i64_list "," "int64"
#line 231 "parser.yy"
                   {
  yystack_[2].value.as < std::vector<int64_t> > ().emplace_back(std::move(yystack_[0].value.as < int64_t > ()));
  yylhs.value.as < std::vector<int64_t> > () = std::move(yystack_[2].value.as < std::vector<int64_t> > ());
}
#line 1384 "parser.cc"
    break;

  case 46: // u8_list: "uint8"
#line 238 "parser.yy"
     { yylhs.value.as < std::vector<uint8_t> > () = std::vector<uint8_t>{yystack_[0].value.as < uint8_t > ()}; }
#line 1390 "parser.cc"
    break;

  case 47: // u8_list: u8_list "," "uint8"
#line 239 "parser.yy"
                 {
  yystack_[2].value.as < std::vector<uint8_t> > ().emplace_back(std::move(yystack_[0].value.as < uint8_t > ()));
  yylhs.value.as < std::vector<uint8_t> > () = std::move(yystack_[2].value.as < std::vector<uint8_t> > ());
}
#line 1399 "parser.cc"
    break;

  case 48: // u16_list: "uint16"
#line 246 "parser.yy"
      { yylhs.value.as < std::vector<uint16_t> > () = std::vector<uint16_t>{yystack_[0].value.as < uint16_t > ()}; }
#line 1405 "parser.cc"
    break;

  case 49: // u16_list: u16_list "," "uint16"
#line 247 "parser.yy"
                   {
  yystack_[2].value.as < std::vector<uint16_t> > ().emplace_back(std::move(yystack_[0].value.as < uint16_t > ()));
  yylhs.value.as < std::vector<uint16_t> > () = std::move(yystack_[2].value.as < std::vector<uint16_t> > ());
}
#line 1414 "parser.cc"
    break;

  case 50: // u32_list: "uint32"
#line 254 "parser.yy"
      { yylhs.value.as < std::vector<uint32_t> > () = std::vector<uint32_t>{yystack_[0].value.as < uint32_t > ()}; }
#line 1420 "parser.cc"
    break;

  case 51: // u32_list: u32_list "," "uint32"
#line 255 "parser.yy"
                   {
  yystack_[2].value.as < std::vector<uint32_t> > ().emplace_back(std::move(yystack_[0].value.as < uint32_t > ()));
  yylhs.value.as < std::vector<uint32_t> > () = std::move(yystack_[2].value.as < std::vector<uint32_t> > ());
}
#line 1429 "parser.cc"
    break;

  case 52: // u64_list: "uint64"
#line 262 "parser.yy"
      { yylhs.value.as < std::vector<uint64_t> > () = std::vector<uint64_t>{yystack_[0].value.as < uint64_t > ()}; }
#line 1435 "parser.cc"
    break;

  case 53: // u64_list: u64_list "," "uint64"
#line 263 "parser.yy"
                   {
  yystack_[2].value.as < std::vector<uint64_t> > ().emplace_back(std::move(yystack_[0].value.as < uint64_t > ()));
  yylhs.value.as < std::vector<uint64_t> > () = std::move(yystack_[2].value.as < std::vector<uint64_t> > ());
}
#line 1444 "parser.cc"
    break;

  case 54: // f32_list: "float"
#line 270 "parser.yy"
      { yylhs.value.as < std::vector<float> > () = std::vector<float>{yystack_[0].value.as < float > ()}; }
#line 1450 "parser.cc"
    break;

  case 55: // f32_list: f32_list "," "float"
#line 271 "parser.yy"
                   {
  yystack_[2].value.as < std::vector<float> > ().emplace_back(std::move(yystack_[0].value.as < float > ()));
  yylhs.value.as < std::vector<float> > () = std::move(yystack_[2].value.as < std::vector<float> > ());
}
#line 1459 "parser.cc"
    break;

  case 56: // f64_list: "double"
#line 278 "parser.yy"
      { yylhs.value.as < std::vector<double> > () = std::vector<double>{yystack_[0].value.as < double > ()}; }
#line 1465 "parser.cc"
    break;

  case 57: // f64_list: f64_list "," "double"
#line 279 "parser.yy"
                   {
  yystack_[2].value.as < std::vector<double> > ().emplace_back(std::move(yystack_[0].value.as < double > ()));
  yylhs.value.as < std::vector<double> > () = std::move(yystack_[2].value.as < std::vector<double> > ());
}
#line 1474 "parser.cc"
    break;

  case 58: // string_list: "string"
#line 286 "parser.yy"
         { yylhs.value.as < std::vector<std::string> > () = std::vector<std::string>{yystack_[0].value.as < std::string > ()}; }
#line 1480 "parser.cc"
    break;

  case 59: // string_list: string_list "," "string"
#line 287 "parser.yy"
                         {
  yystack_[2].value.as < std::vector<std::string> > ().emplace_back(std::move(yystack_[0].value.as < std::string > ()));
  yylhs.value.as < std::vector<std::string> > () = std::move(yystack_[2].value.as < std::vector<std::string> > ());
}
#line 1489 "parser.cc"
    break;

  case 60: // function: infix_function
#line 294 "parser.yy"
                 { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ()); }
#line 1495 "parser.cc"
    break;

  case 61: // function: named_function
#line 295 "parser.yy"
                 { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ()); }
#line 1501 "parser.cc"
    break;

  case 62: // infix_function: "-" term
#line 300 "parser.yy"
                     { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::UnaryOPNode>(jitfusion::UnaryOPType::kMinus, std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1507 "parser.cc"
    break;

  case 63: // infix_function: "not" term
#line 301 "parser.yy"
                       { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::UnaryOPNode>(jitfusion::UnaryOPType::kNot, std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1513 "parser.cc"
    break;

  case 64: // infix_function: "+" term
#line 302 "parser.yy"
                     { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::UnaryOPNode>(jitfusion::UnaryOPType::kPlus, std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1519 "parser.cc"
    break;

  case 65: // infix_function: "~" term
#line 303 "parser.yy"
                     { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::UnaryOPNode>(jitfusion::UnaryOPType::kBitwiseNot, std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1525 "parser.cc"
    break;

  case 66: // infix_function: term "+" term
#line 304 "parser.yy"
                { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kAdd, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1531 "parser.cc"
    break;

  case 67: // infix_function: term "-" term
#line 305 "parser.yy"
                { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kSub, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1537 "parser.cc"
    break;

  case 68: // infix_function: term "*" term
#line 306 "parser.yy"
                { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kMul, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1543 "parser.cc"
    break;

  case 69: // infix_function: term "/" term
#line 307 "parser.yy"
                { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kDiv, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1549 "parser.cc"
    break;

  case 70: // infix_function: term "%" term
#line 308 "parser.yy"
                { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kMod, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1555 "parser.cc"
    break;

  case 71: // infix_function: term "&" term
#line 309 "parser.yy"
                { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kBitwiseAnd, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1561 "parser.cc"
    break;

  case 72: // infix_function: term "|" term
#line 310 "parser.yy"
                { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kBitwiseOr, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1567 "parser.cc"
    break;

  case 73: // infix_function: term "^" term
#line 311 "parser.yy"
                { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kBitwiseXor, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1573 "parser.cc"
    break;

  case 74: // infix_function: term "==" term
#line 312 "parser.yy"
                 { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kEqual, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1579 "parser.cc"
    break;

  case 75: // infix_function: term "!=" term
#line 313 "parser.yy"
                 { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kNotEqual, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1585 "parser.cc"
    break;

  case 76: // infix_function: term ">" term
#line 314 "parser.yy"
                { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kLarge, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1591 "parser.cc"
    break;

  case 77: // infix_function: term ">=" term
#line 315 "parser.yy"
                 { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kLargeEqual, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1597 "parser.cc"
    break;

  case 78: // infix_function: term "<" term
#line 316 "parser.yy"
                { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kLess, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1603 "parser.cc"
    break;

  case 79: // infix_function: term "<=" term
#line 317 "parser.yy"
                 { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kLessEqual, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1609 "parser.cc"
    break;

  case 80: // infix_function: term "<<" term
#line 318 "parser.yy"
                 { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kBitwiseShiftLeft, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1615 "parser.cc"
    break;

  case 81: // infix_function: term ">>" term
#line 319 "parser.yy"
                 { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kBitwiseShiftRight, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1621 "parser.cc"
    break;

  case 82: // named_function: "if" "(" args ")"
#line 323 "parser.yy"
                    { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::IfNode>(std::move(yystack_[1].value.as < std::vector<std::unique_ptr<jitfusion::ExecNode>> > ())); }
#line 1627 "parser.cc"
    break;

  case 83: // named_function: "identifier" "(" args ")"
#line 324 "parser.yy"
                           { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::FunctionNode>(std::move(yystack_[3].value.as < std::string > ()), std::move(yystack_[1].value.as < std::vector<std::unique_ptr<jitfusion::ExecNode>> > ())); }
#line 1633 "parser.cc"
    break;

  case 84: // named_function: term "in" term
#line 325 "parser.yy"
                 {
    yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::FunctionNode>("in", std::vector<std::unique_ptr<jitfusion::ExecNode>>());
    static_cast<jitfusion::FunctionNode *>(yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > ().get())->AppendArgs(std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()));
    static_cast<jitfusion::FunctionNode *>(yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > ().get())->AppendArgs(std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ()));
}
#line 1643 "parser.cc"
    break;

  case 85: // named_function: "switch" "(" args ")"
#line 330 "parser.yy"
                        { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::SwitchNode>(std::move(yystack_[1].value.as < std::vector<std::unique_ptr<jitfusion::ExecNode>> > ())); }
#line 1649 "parser.cc"
    break;

  case 86: // args: arg
#line 334 "parser.yy"
      {
    yylhs.value.as < std::vector<std::unique_ptr<jitfusion::ExecNode>> > () = std::vector<std::unique_ptr<jitfusion::ExecNode>>{};
    yylhs.value.as < std::vector<std::unique_ptr<jitfusion::ExecNode>> > ().emplace_back(std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ()));
  }
#line 1658 "parser.cc"
    break;

  case 87: // args: args "," arg
#line 338 "parser.yy"
               {
    yystack_[2].value.as < std::vector<std::unique_ptr<jitfusion::ExecNode>> > ().emplace_back(std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ()));
    yylhs.value.as < std::vector<std::unique_ptr<jitfusion::ExecNode>> > () = std::move(yystack_[2].value.as < std::vector<std::unique_ptr<jitfusion::ExecNode>> > ());
  }
#line 1667 "parser.cc"
    break;

  case 88: // arg: term
#line 345 "parser.yy"
       { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ()); }
#line 1673 "parser.cc"
    break;

  case 89: // boolean: term "and" term
#line 349 "parser.yy"
                  { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kAnd, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1679 "parser.cc"
    break;

  case 90: // boolean: term "or" term
#line 350 "parser.yy"
                 { yylhs.value.as < std::unique_ptr<jitfusion::ExecNode> > () = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kOr, std::move(yystack_[2].value.as < std::unique_ptr<jitfusion::ExecNode> > ()), std::move(yystack_[0].value.as < std::unique_ptr<jitfusion::ExecNode> > ())); }
#line 1685 "parser.cc"
    break;


#line 1689 "parser.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      yy_lac_discard_ ("error recovery");
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  const char *
  Parser::symbol_name (symbol_kind_type yysymbol)
  {
    static const char *const yy_sname[] =
    {
    "end of file", "error", "invalid token", "if", "switch", "in", "not",
  "and", "or", "true", "false", "-", "+", "*", "/", "(", ")", "%", "|",
  "^", "&", "~", "<<", ">>", "==", "=", "!=", "<", "<=", ">", ">=", ",",
  "[", "]", ";", "entry_arg", "exec_ctx", "identifier", "int8", "int16",
  "int32", "int64", "uint8", "uint16", "uint32", "uint64", "float",
  "double", "string", "NEG", "$accept", "program", "statement", "expr",
  "term", "literal", "list", "i8_list", "i16_list", "i32_list", "i64_list",
  "u8_list", "u16_list", "u32_list", "u64_list", "f32_list", "f64_list",
  "string_list", "function", "infix_function", "named_function", "args",
  "arg", "boolean", YY_NULLPTR
    };
    return yy_sname[yysymbol];
  }



  // Parser::context.
  Parser::context::context (const Parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  Parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

#if YYDEBUG
    // Execute LAC once. We don't care if it is successful, we
    // only do it for the sake of debugging output.
    if (!yyparser_.yy_lac_established_)
      yyparser_.yy_lac_check_ (yyla_.kind ());
#endif

    for (int yyx = 0; yyx < YYNTOKENS; ++yyx)
      {
        symbol_kind_type yysym = YY_CAST (symbol_kind_type, yyx);
        if (yysym != symbol_kind::S_YYerror
            && yysym != symbol_kind::S_YYUNDEF
            && yyparser_.yy_lac_check_ (yysym))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = yysym;
          }
      }
    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }




  bool
  Parser::yy_lac_check_ (symbol_kind_type yytoken) const
  {
    // Logically, the yylac_stack's lifetime is confined to this function.
    // Clear it, to get rid of potential left-overs from previous call.
    yylac_stack_.clear ();
    // Reduce until we encounter a shift and thereby accept the token.
#if YYDEBUG
    YYCDEBUG << "LAC: checking lookahead " << symbol_name (yytoken) << ':';
#endif
    std::ptrdiff_t lac_top = 0;
    while (true)
      {
        state_type top_state = (yylac_stack_.empty ()
                                ? yystack_[lac_top].state
                                : yylac_stack_.back ());
        int yyrule = yypact_[+top_state];
        if (yy_pact_value_is_default_ (yyrule)
            || (yyrule += yytoken) < 0 || yylast_ < yyrule
            || yycheck_[yyrule] != yytoken)
          {
            // Use the default action.
            yyrule = yydefact_[+top_state];
            if (yyrule == 0)
              {
                YYCDEBUG << " Err\n";
                return false;
              }
          }
        else
          {
            // Use the action from yytable.
            yyrule = yytable_[yyrule];
            if (yy_table_value_is_error_ (yyrule))
              {
                YYCDEBUG << " Err\n";
                return false;
              }
            if (0 < yyrule)
              {
                YYCDEBUG << " S" << yyrule << '\n';
                return true;
              }
            yyrule = -yyrule;
          }
        // By now we know we have to simulate a reduce.
        YYCDEBUG << " R" << yyrule - 1;
        // Pop the corresponding number of values from the stack.
        {
          std::ptrdiff_t yylen = yyr2_[yyrule];
          // First pop from the LAC stack as many tokens as possible.
          std::ptrdiff_t lac_size = std::ptrdiff_t (yylac_stack_.size ());
          if (yylen < lac_size)
            {
              yylac_stack_.resize (std::size_t (lac_size - yylen));
              yylen = 0;
            }
          else if (lac_size)
            {
              yylac_stack_.clear ();
              yylen -= lac_size;
            }
          // Only afterwards look at the main stack.
          // We simulate popping elements by incrementing lac_top.
          lac_top += yylen;
        }
        // Keep top_state in sync with the updated stack.
        top_state = (yylac_stack_.empty ()
                     ? yystack_[lac_top].state
                     : yylac_stack_.back ());
        // Push the resulting state of the reduction.
        state_type state = yy_lr_goto_state_ (top_state, yyr1_[yyrule]);
        YYCDEBUG << " G" << int (state);
        yylac_stack_.push_back (state);
      }
  }

  // Establish the initial context if no initial context currently exists.
  bool
  Parser::yy_lac_establish_ (symbol_kind_type yytoken)
  {
    /* Establish the initial context for the current lookahead if no initial
       context is currently established.

       We define a context as a snapshot of the parser stacks.  We define
       the initial context for a lookahead as the context in which the
       parser initially examines that lookahead in order to select a
       syntactic action.  Thus, if the lookahead eventually proves
       syntactically unacceptable (possibly in a later context reached via a
       series of reductions), the initial context can be used to determine
       the exact set of tokens that would be syntactically acceptable in the
       lookahead's place.  Moreover, it is the context after which any
       further semantic actions would be erroneous because they would be
       determined by a syntactically unacceptable token.

       yy_lac_establish_ should be invoked when a reduction is about to be
       performed in an inconsistent state (which, for the purposes of LAC,
       includes consistent states that don't know they're consistent because
       their default reductions have been disabled).

       For parse.lac=full, the implementation of yy_lac_establish_ is as
       follows.  If no initial context is currently established for the
       current lookahead, then check if that lookahead can eventually be
       shifted if syntactic actions continue from the current context.  */
    if (yy_lac_established_)
      return true;
    else
      {
#if YYDEBUG
        YYCDEBUG << "LAC: initial context established for "
                 << symbol_name (yytoken) << '\n';
#endif
        yy_lac_established_ = true;
        return yy_lac_check_ (yytoken);
      }
  }

  // Discard any previous initial lookahead context.
  void
  Parser::yy_lac_discard_ (const char* event)
  {
   /* Discard any previous initial lookahead context because of Event,
      which may be a lookahead change or an invalidation of the currently
      established initial context for the current lookahead.

      The most common example of a lookahead change is a shift.  An example
      of both cases is syntax error recovery.  That is, a syntax error
      occurs when the lookahead is syntactically erroneous for the
      currently established initial context, so error recovery manipulates
      the parser stacks to try to find a new initial context in which the
      current lookahead is syntactically acceptable.  If it fails to find
      such a context, it discards the lookahead.  */
    if (yy_lac_established_)
      {
        YYCDEBUG << "LAC: initial context discarded due to "
                 << event << '\n';
        yy_lac_established_ = false;
      }
  }


  int
  Parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
         In the first two cases, it might appear that the current syntax
         error should have been detected in the previous state when
         yy_lac_check was invoked.  However, at that time, there might
         have been a different syntax error that discarded a different
         initial context during error recovery, leaving behind the
         current lookahead.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char Parser::yypact_ninf_ = -38;

  const signed char Parser::yytable_ninf_ = -1;

  const short
  Parser::yypact_[] =
  {
     -21,     2,     1,   -38,     9,   -38,   -38,    17,    25,     9,
     -38,   -38,     9,     9,     9,     9,   258,   -38,   -38,    27,
     -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
     -38,     3,   106,   -38,   -38,   -38,   -38,   -38,   -38,     9,
       9,   -38,   -38,   -38,    80,   -38,   -38,   -38,   -38,   -38,
     -38,   -38,   -38,   -38,   -38,   -38,   -38,   -27,    -8,    -5,
      30,   107,   177,   197,   200,   217,   220,   237,     9,   -38,
       9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
       9,     9,     9,     9,     9,     9,     9,     9,     9,   106,
     -14,   -38,    -9,   -38,     5,   -38,    21,   -38,    24,   -38,
      48,   -38,    23,   -38,    43,   -38,    46,   -38,    50,   -38,
      55,   -38,    58,   -38,    64,   -38,    -2,   172,   152,   132,
      45,    45,   -38,   -38,   -38,   192,   212,   232,    22,    22,
     252,   252,   272,   272,   272,   272,   -38,     9,   -38,   -38,
     -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
     -38,   -38
  };

  const signed char
  Parser::yydefact_[] =
  {
       0,     0,     0,     2,     0,     1,     3,     0,     0,     0,
      25,    26,     0,     0,     0,     0,     0,    11,    12,    10,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,     0,     5,     6,    24,     7,    60,    61,     8,     0,
       0,    63,    62,    64,     0,    65,    38,    40,    42,    44,
      46,    48,    50,    52,    54,    56,    58,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    88,
       0,    86,     0,     9,     0,    27,     0,    28,     0,    29,
       0,    30,     0,    31,     0,    32,     0,    33,     0,    34,
       0,    35,     0,    36,     0,    37,     0,    84,    89,    90,
      67,    66,    68,    69,    70,    72,    73,    71,    80,    81,
      74,    75,    78,    79,    76,    77,    82,     0,    85,    39,
      41,    43,    45,    47,    49,    51,    53,    55,    57,    59,
      83,    87
  };

  const signed char
  Parser::yypgoto_[] =
  {
     -38,   -38,   113,   -38,    -4,   -38,   -38,   -38,   -38,   -38,
     -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
     -38,   -37,   -16,   -38
  };

  const signed char
  Parser::yydefgoto_[] =
  {
       0,     2,     3,    31,    89,    33,    34,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    35,    36,
      37,    90,    91,    38
  };

  const unsigned char
  Parser::yytable_[] =
  {
      32,     5,   136,    92,    94,    41,    95,   138,    42,    43,
      44,    45,     7,     8,   150,     9,     1,   137,    10,    11,
      12,    13,   137,    96,    14,    97,    98,     4,    99,   137,
      15,   116,    39,    73,    74,    75,    76,    69,     1,    77,
      40,    16,    68,   139,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    75,    76,
     140,   100,    77,   101,   141,   143,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,    70,   144,    71,    72,   142,
     145,    73,    74,    75,    76,   146,    93,    77,    78,    79,
      80,   147,    81,    82,    83,   148,    84,    85,    86,    87,
      88,    70,   149,    71,    72,     6,     0,    73,    74,    75,
      76,   151,     0,    77,    78,    79,    80,     0,    81,    82,
      83,     0,    84,    85,    86,    87,    88,    70,   102,    71,
     103,     0,     0,    73,    74,    75,    76,     0,     0,    77,
      78,    79,    80,     0,    81,    82,    83,    70,    84,    85,
      86,    87,    88,    73,    74,    75,    76,     0,     0,    77,
      78,    79,    80,     0,    81,    82,    83,     0,    84,    85,
      86,    87,    88,    73,    74,    75,    76,     0,     0,    77,
      78,    79,    80,     0,    81,    82,    83,     0,    84,    85,
      86,    87,    88,    73,    74,    75,    76,     0,   104,    77,
     105,    79,    80,     0,    81,    82,    83,     0,    84,    85,
      86,    87,    88,    73,    74,    75,    76,     0,   106,    77,
     107,   108,    80,   109,    81,    82,    83,     0,    84,    85,
      86,    87,    88,    73,    74,    75,    76,     0,   110,    77,
     111,   112,     0,   113,    81,    82,    83,     0,    84,    85,
      86,    87,    88,    73,    74,    75,    76,     0,   114,    77,
     115,     0,     0,     0,    81,    82,     0,     0,     0,    85,
      86,    87,    88,    73,    74,    75,    76,     0,     0,    77,
       0,     0,     0,     0,    81,    82,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56
  };

  const short
  Parser::yycheck_[] =
  {
       4,     0,    16,    40,    31,     9,    33,    16,    12,    13,
      14,    15,     3,     4,    16,     6,    37,    31,     9,    10,
      11,    12,    31,    31,    15,    33,    31,    25,    33,    31,
      21,    68,    15,    11,    12,    13,    14,    34,    37,    17,
      15,    32,    15,    38,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    13,    14,
      39,    31,    17,    33,    40,    42,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,     5,    43,     7,     8,    41,
      44,    11,    12,    13,    14,    45,    16,    17,    18,    19,
      20,    46,    22,    23,    24,    47,    26,    27,    28,    29,
      30,     5,    48,     7,     8,     2,    -1,    11,    12,    13,
      14,   137,    -1,    17,    18,    19,    20,    -1,    22,    23,
      24,    -1,    26,    27,    28,    29,    30,     5,    31,     7,
      33,    -1,    -1,    11,    12,    13,    14,    -1,    -1,    17,
      18,    19,    20,    -1,    22,    23,    24,     5,    26,    27,
      28,    29,    30,    11,    12,    13,    14,    -1,    -1,    17,
      18,    19,    20,    -1,    22,    23,    24,    -1,    26,    27,
      28,    29,    30,    11,    12,    13,    14,    -1,    -1,    17,
      18,    19,    20,    -1,    22,    23,    24,    -1,    26,    27,
      28,    29,    30,    11,    12,    13,    14,    -1,    31,    17,
      33,    19,    20,    -1,    22,    23,    24,    -1,    26,    27,
      28,    29,    30,    11,    12,    13,    14,    -1,    31,    17,
      33,    31,    20,    33,    22,    23,    24,    -1,    26,    27,
      28,    29,    30,    11,    12,    13,    14,    -1,    31,    17,
      33,    31,    -1,    33,    22,    23,    24,    -1,    26,    27,
      28,    29,    30,    11,    12,    13,    14,    -1,    31,    17,
      33,    -1,    -1,    -1,    22,    23,    -1,    -1,    -1,    27,
      28,    29,    30,    11,    12,    13,    14,    -1,    -1,    17,
      -1,    -1,    -1,    -1,    22,    23,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48
  };

  const signed char
  Parser::yystos_[] =
  {
       0,    37,    51,    52,    25,     0,    52,     3,     4,     6,
       9,    10,    11,    12,    15,    21,    32,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    53,    54,    55,    56,    68,    69,    70,    73,    15,
      15,    54,    54,    54,    54,    54,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    15,    34,
       5,     7,     8,    11,    12,    13,    14,    17,    18,    19,
      20,    22,    23,    24,    26,    27,    28,    29,    30,    54,
      71,    72,    71,    16,    31,    33,    31,    33,    31,    33,
      31,    33,    31,    33,    31,    33,    31,    33,    31,    33,
      31,    33,    31,    33,    31,    33,    71,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    16,    31,    16,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      16,    72
  };

  const signed char
  Parser::yyr1_[] =
  {
       0,    50,    51,    51,    52,    53,    54,    54,    54,    54,
      54,    54,    54,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    57,    57,
      58,    58,    59,    59,    60,    60,    61,    61,    62,    62,
      63,    63,    64,    64,    65,    65,    66,    66,    67,    67,
      68,    68,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    70,    70,    70,    70,    71,    71,    72,    73,
      73
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,     1,     2,     4,     1,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     1,     2,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     4,     4,     3,     4,     1,     3,     1,     3,
       3
  };




#if YYDEBUG
  const short
  Parser::yyrline_[] =
  {
       0,   150,   150,   154,   159,   161,   165,   166,   167,   168,
     169,   170,   171,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   206,   207,
     214,   215,   222,   223,   230,   231,   238,   239,   246,   247,
     254,   255,   262,   263,   270,   271,   278,   279,   286,   287,
     294,   295,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   323,   324,   325,   330,   334,   338,   345,   349,
     350
  };

  void
  Parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  Parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 10 "parser.yy"
} // athena
#line 2385 "parser.cc"

#line 352 "parser.yy"


void athena::Parser::error(const location_type &l, const std::string &m) {
  std::stringstream sstream;
  sstream << l << ": " << m;
  builder.parser_error_message_ = sstream.str();
}
