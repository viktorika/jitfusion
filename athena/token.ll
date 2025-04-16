%{
#include <iostream>
#include <string>
#include "parser.hh"
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
%}

%option prefix="athena_"
%option reentrant

%{
#if defined __clang__
# define CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#endif

// Clang and ICC like to pretend they are GCC.
#if defined __GNUC__ && !defined __clang__ && !defined __ICC
# define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#endif

// Pacify warnings in yy_init_buffer (observed with Flex 2.6.4)
// and GCC 6.4.0, 7.3.0 with -O3.
#if defined GCC_VERSION && 600 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wnull-dereference"
#endif

// This example uses Flex's C back end, yet compiles it as C++.
// So expect warnings about C style casts and NULL.
#if defined CLANG_VERSION && 500 <= CLANG_VERSION
# pragma clang diagnostic ignored "-Wold-style-cast"
# pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#elif defined GCC_VERSION && 407 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#define FLEX_VERSION (YY_FLEX_MAJOR_VERSION * 100 + YY_FLEX_MINOR_VERSION)

// Old versions of Flex (2.5.35) generate an incomplete documentation comment.
//
//  In file included from src/scan-code-c.c:3:
//  src/scan-code.c:2198:21: error: empty paragraph passed to '@param' command
//        [-Werror,-Wdocumentation]
//   * @param line_number
//     ~~~~~~~~~~~~~~~~~^
//  1 error generated.
#if FLEX_VERSION < 206 && defined CLANG_VERSION
# pragma clang diagnostic ignored "-Wdocumentation"
#endif

// Old versions of Flex (2.5.35) use 'register'.  Warnings introduced in
// GCC 7 and Clang 6.
#if FLEX_VERSION < 206
# if defined CLANG_VERSION && 600 <= CLANG_VERSION
#  pragma clang diagnostic ignored "-Wdeprecated-register"
# elif defined GCC_VERSION && 700 <= GCC_VERSION
#  pragma GCC diagnostic ignored "-Wregister"
# endif
#endif

#if FLEX_VERSION <= 206
# if defined CLANG_VERSION
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wdocumentation"
#  pragma clang diagnostic ignored "-Wshorten-64-to-32"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wsign-compare"
# elif defined GCC_VERSION
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
# endif
#endif

// Flex 2.6.4, GCC 9
// warning: useless cast to type 'int' [-Wuseless-cast]
// 1361 |   YY_CURRENT_BUFFER_LVALUE->yy_buf_size = (int) (new_size - 2);
//      |                                                 ^
#if defined GCC_VERSION && 900 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
%}

%option noyywrap nounput noinput batch

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+
float ([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+)
blank [ \t\r]

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}
%%
%{
  // A handy shortcut to the location held by the Parser.
  athena::location& loc = builder.GetLocation();
  // Code run each time yylex is called.
  loc.step ();
%}
{blank}+   loc.step ();
\n+        loc.lines (yyleng); loc.step ();

"if"          return  athena::Parser::make_IF                          (loc);
"switch"      return  athena::Parser::make_SWITCH                      (loc);
";"           return  athena::Parser::make_SEMI                        (loc);

"in"          return  athena::Parser::make_IN                          (loc);      
(not|!)       return  athena::Parser::make_NOT                         (loc);     
(and|&&)      return  athena::Parser::make_AND                         (loc);     
(or|\|\|)     return  athena::Parser::make_OR                          (loc);      
"true"        return  athena::Parser::make_TRUE                        (loc);      
"false"       return  athena::Parser::make_FALSE                       (loc);      
"-"           return  athena::Parser::make_MINUS                       (loc);   
"+"           return  athena::Parser::make_PLUS                        (loc);    
"*"           return  athena::Parser::make_STAR                        (loc);    
"/"           return  athena::Parser::make_SLASH                       (loc);   
"("           return  athena::Parser::make_LPAREN                      (loc);  
")"           return  athena::Parser::make_RPAREN                      (loc);  
"%"           return  athena::Parser::make_MODOLO                      (loc);  
"|"           return  athena::Parser::make_BITWISE_OR                  (loc);
"^"           return  athena::Parser::make_BITWISE_XOR                 (loc);
"&"           return  athena::Parser::make_BITWISE_AND                 (loc);
"~"           return  athena::Parser::make_BITWISE_NOT                 (loc);
"=="          return  athena::Parser::make_EQUAL                       (loc);
"="           return  athena::Parser::make_ASSIGNMENT                  (loc);
(!=|<>)       return  athena::Parser::make_NOT_EQUAL                   (loc);
"<"           return  athena::Parser::make_LESS_THAN                   (loc);
"<="          return  athena::Parser::make_LESS_THAN_OR_EQUAL_TO       (loc);
">"           return  athena::Parser::make_GREATER_THAN                (loc);
">="          return  athena::Parser::make_GREATER_THAN_OR_EQUAL_TO    (loc);
","           return  athena::Parser::make_COMMA                       (loc);
"["           return  athena::Parser::make_LBRACKET                    (loc);
"]"           return  athena::Parser::make_RBRACKET                    (loc);
"<<"          return  athena::Parser::make_BITWISE_LEFT                (loc);
">>"          return  athena::Parser::make_BITWISE_RIGHT               (loc);

"entry_arg"   return  athena::Parser::make_ENTRY_ARG   (loc);
"exec_ctx"    return  athena::Parser::make_EXEC_CTX    (loc);
"output"      return  athena::Parser::make_OUTPUT        (loc);

{id}       return  athena::Parser::make_IDENTIFIER (std::string(yytext), loc);

{int}      return  athena::Parser::make_I32 (std::stoull(yytext), loc);
{int}"u8"  return  athena::Parser::make_U8 (std::stoull(yytext), loc);
{int}"u16" return  athena::Parser::make_U16 (std::stoull(yytext), loc);
{int}"u32" return  athena::Parser::make_U32 (std::stoull(yytext), loc);
{int}"u64" return  athena::Parser::make_U64 (std::stoull(yytext), loc);
{int}"i8"  return  athena::Parser::make_I8 (std::stoull(yytext), loc);
{int}"i16" return  athena::Parser::make_I16 (std::stoull(yytext), loc);
{int}"i32" return  athena::Parser::make_I32 (std::stoull(yytext), loc);
{int}"i64" return  athena::Parser::make_I64 (std::stoull(yytext), loc);

{float}       return  athena::Parser::make_F64 (std::stod(yytext), loc);
{float}"f32"  return  athena::Parser::make_F32 (std::stof(yytext), loc);
{float}"f64"  return  athena::Parser::make_F64 (std::stod(yytext), loc);
{int}"f32"    return  athena::Parser::make_F32 (std::stof(yytext), loc);
{int}"f64"    return  athena::Parser::make_F64 (std::stod(yytext), loc);


\"([^\\\"]|\\.)*\"     return  athena::Parser::make_STRING (std::string(yytext + 1, yyleng - 2), loc);
\'([^\\\']|\\.)*\'     return  athena::Parser::make_STRING (std::string(yytext + 1, yyleng - 2), loc);

<<EOF>>    return  athena::Parser::make_YYEOF (loc);
%%

jitfusion::Status athena::ProgramAstBuilder::Scan(const std::string &code) {
  yyscan_t scanner;
  yylex_init(&scanner);
  auto *buffer = yy_scan_string(code.c_str(), scanner);
  athena::Parser parser(scanner, *this);
  int res = parser();
  yy_delete_buffer(buffer, scanner);
  if (res != 0) {
    return jitfusion::Status::ParseError(parser_error_message_);
  }
  return jitfusion::Status::OK();
}
