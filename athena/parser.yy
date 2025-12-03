%skeleton "lalr1.cc"
%require "3.8"
%header

%define api.token.raw

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace {athena}
%define api.parser.class {Parser}

%code requires {
  #pragma once
  #include <string>
  #include "exec_engine.h"
  #include "ast_builder.h"

  typedef void* yyscan_t;
  #ifndef yylex
  #define yylex athena_yylex
  #endif
}

%parse-param { yyscan_t scanner }
%lex-param { yyscan_t scanner }

%code provides {
// Give Flex the prototype of yylex we want ...
#define YY_DECL athena::Parser::symbol_type athena_yylex(yyscan_t yyscanner, athena::ProgramAstBuilder &builder)
// ... and declare it for the parser's sake.
YY_DECL;
}

// The parsing context.
%param { ProgramAstBuilder &builder }

%locations

%define parse.trace
%define parse.error detailed
%define parse.lac full

%define api.token.prefix {TOK_}
%token
  IF                          "if"
  SWITCH                      "switch"
  IN                          "in"
  NOT                         "not"
  AND                         "and"
  OR                          "or"
  TRUE                        "true"
  FALSE                       "false"
  MINUS                       "-"
  PLUS                        "+"
  STAR                        "*"
  SLASH                       "/"
  LPAREN                      "("
  RPAREN                      ")"
  MODOLO                      "%"
  BITWISE_OR                  "|"
  BITWISE_XOR                 "^"
  BITWISE_AND                 "&"
  BITWISE_NOT                 "~"
  BITWISE_LEFT                "<<"
  BITWISE_RIGHT               ">>"
  EQUAL                       "==" 
  ASSIGNMENT                  "="
  NOT_EQUAL                   "!="
  LESS_THAN                   "<" 
  LESS_THAN_OR_EQUAL_TO       "<=" 
  GREATER_THAN                ">" 
  GREATER_THAN_OR_EQUAL_TO    ">="
  COMMA                       ","
  LBRACKET                    "["
  RBRACKET                    "]"
  SEMI                        ";"
  ENTRY_ARG                   "entry_arg"
  EXEC_CTX                    "exec_ctx"
  OUTPUT                      "output"
;

%token <std::string> IDENTIFIER "identifier";

%token <int8_t> I8 "int8";
%token <int16_t> I16 "int16";
%token <int32_t> I32 "int32";
%token <int64_t> I64 "int64";

%token <uint8_t> U8 "uint8";
%token <uint16_t> U16 "uint16";
%token <uint32_t> U32 "uint32";
%token <uint64_t> U64 "uint64";

%token <float> F32 "float";
%token <double> F64 "double";

%token <std::string> STRING "string";

%nterm <void*> program;
%nterm <Statement> statement;
%nterm <std::unique_ptr<jitfusion::ExecNode>> expr;
%nterm <std::unique_ptr<jitfusion::ExecNode>> term;
%nterm <std::unique_ptr<jitfusion::ExecNode>> literal;
%nterm <std::unique_ptr<jitfusion::ExecNode>> function;
%nterm <std::unique_ptr<jitfusion::ExecNode>> infix_function;
%nterm <std::unique_ptr<jitfusion::ExecNode>> named_function;
%nterm <std::unique_ptr<jitfusion::ExecNode>> boolean;


%nterm <std::vector<std::unique_ptr<jitfusion::ExecNode>>> args;
%nterm <std::unique_ptr<jitfusion::ExecNode>> arg;


%nterm <std::unique_ptr<jitfusion::ExecNode>> list;

%nterm <std::vector<int8_t>> i8_list;
%nterm <std::vector<int16_t>> i16_list;
%nterm <std::vector<int32_t>> i32_list;
%nterm <std::vector<int64_t>> i64_list;

%nterm <std::vector<uint8_t>> u8_list;
%nterm <std::vector<uint16_t>> u16_list;
%nterm <std::vector<uint32_t>> u32_list;
%nterm <std::vector<uint64_t>> u64_list;

%nterm <std::vector<float>> f32_list;
%nterm <std::vector<double>> f64_list;

%nterm <std::vector<std::string>> string_list;

%%
%start program;

%left "or";
%left "and";
%left "in";
%left "|";
%left "^";
%left "&";

%left "==" "!=";
%left "<" "<=" ">" ">=";
%left "<<" ">>";
%left "-" "+";
%left "*" "/" "%";
%precedence NEG;
%left "(" ")";

program: 
  statement { 
    builder.AddStatement(std::move($1));
    $$ = nullptr;
  }
| program statement {
  builder.AddStatement(std::move($2));
  $$ = nullptr;
}

statement: 
  IDENTIFIER "=" expr ";" { $$ = Statement(std::move($1), std::move($3)); }
| expr ";" { $$ = Statement(std::move($1)); }

expr: term { $$ = std::move($1);}
;

term:
  literal { $$ = std::move($1); }
| function { $$ = std::move($1); }
| boolean { $$ = std::move($1); }
| "(" term ")" { $$ = std::move($2); }
| IDENTIFIER { $$ = builder.MakeRefNode(std::move($1)); }
| ENTRY_ARG { $$ = std::unique_ptr<jitfusion::ExecNode>(new jitfusion::EntryArgumentNode); }
| EXEC_CTX { $$ = std::unique_ptr<jitfusion::ExecNode>(new jitfusion::ExecContextNode); }
| OUTPUT { $$ = std::unique_ptr<jitfusion::ExecNode>(new jitfusion::OutputNode); }
;

literal:
  I8 { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| I16 { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| I32 { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| I64 { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| U8 { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| U16 { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| U32 { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| U64 { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| F32 { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| F64 { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| STRING { $$ = std::make_unique<jitfusion::ConstantValueNode>($1); }
| list { $$ = std::move($1); }
| "true" { $$ = std::make_unique<jitfusion::ConstantValueNode>(uint8_t(1)); }
| "false" { $$ = std::make_unique<jitfusion::ConstantValueNode>(uint8_t(0)); }
;

list:
  "[" i8_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
| "[" i16_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
| "[" i32_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
| "[" i64_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
| "[" u8_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
| "[" u16_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
| "[" u32_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
| "[" u64_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
| "[" f32_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
| "[" f64_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
| "[" string_list "]" { $$ = std::make_unique<jitfusion::ConstantListValueNode>($2); }
;

i8_list:
  I8 { $$ = std::vector<int8_t>{$1}; }
| i8_list "," I8 {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

i16_list:
  I16 { $$ = std::vector<int16_t>{$1}; }
| i16_list "," I16 {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

i32_list:
  I32 { $$ = std::vector<int32_t>{$1}; }
| i32_list "," I32 {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

i64_list:
  I64 { $$ = std::vector<int64_t>{$1}; }
| i64_list "," I64 {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

u8_list:
  U8 { $$ = std::vector<uint8_t>{$1}; }
| u8_list "," U8 {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

u16_list:
  U16 { $$ = std::vector<uint16_t>{$1}; }
| u16_list "," U16 {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

u32_list:
  U32 { $$ = std::vector<uint32_t>{$1}; }
| u32_list "," U32 {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

u64_list:
  U64 { $$ = std::vector<uint64_t>{$1}; }
| u64_list "," U64 {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

f32_list:
  F32 { $$ = std::vector<float>{$1}; }
| f32_list "," F32 {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

f64_list:
  F64 { $$ = std::vector<double>{$1}; }
| f64_list "," F64 {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

string_list:
  STRING { $$ = std::vector<std::string>{$1}; }
| string_list "," STRING {
  $1.emplace_back(std::move($3));
  $$ = std::move($1);
}
;

function:
  infix_function { $$ = std::move($1); }
| named_function { $$ = std::move($1); }
;


infix_function:
  "-" term %prec NEG { $$ = std::make_unique<jitfusion::UnaryOPNode>(jitfusion::UnaryOPType::kMinus, std::move($2)); }
| "not" term %prec NEG { $$ = std::make_unique<jitfusion::UnaryOPNode>(jitfusion::UnaryOPType::kNot, std::move($2)); }
| "+" term %prec NEG { $$ = std::make_unique<jitfusion::UnaryOPNode>(jitfusion::UnaryOPType::kPlus, std::move($2)); }
| "~" term %prec NEG { $$ = std::make_unique<jitfusion::UnaryOPNode>(jitfusion::UnaryOPType::kBitwiseNot, std::move($2)); }
| term "+" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kAdd, std::move($1), std::move($3)); }
| term "-" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kSub, std::move($1), std::move($3)); }
| term "*" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kMul, std::move($1), std::move($3)); }
| term "/" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kDiv, std::move($1), std::move($3)); }
| term "%" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kMod, std::move($1), std::move($3)); }
| term "&" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kBitwiseAnd, std::move($1), std::move($3)); }
| term "|" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kBitwiseOr, std::move($1), std::move($3)); }
| term "^" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kBitwiseXor, std::move($1), std::move($3)); }
| term "==" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kEqual, std::move($1), std::move($3)); }
| term "!=" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kNotEqual, std::move($1), std::move($3)); }
| term ">" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kLarge, std::move($1), std::move($3)); }
| term ">=" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kLargeEqual, std::move($1), std::move($3)); }
| term "<" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kLess, std::move($1), std::move($3)); }
| term "<=" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kLessEqual, std::move($1), std::move($3)); }
| term "<<" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kBitwiseShiftLeft, std::move($1), std::move($3)); }
| term ">>" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kBitwiseShiftRight, std::move($1), std::move($3)); }
;

named_function:
  "if" "(" args ")" { $$ = std::make_unique<jitfusion::IfNode>(std::move($3)); }
|  IDENTIFIER "(" args ")" { $$ = std::make_unique<jitfusion::FunctionNode>(std::move($1), std::move($3)); }
| term "in" term {
    $$ = std::make_unique<jitfusion::FunctionNode>("in", std::vector<std::unique_ptr<jitfusion::ExecNode>>());
    static_cast<jitfusion::FunctionNode *>($$.get())->AppendArgs(std::move($1));
    static_cast<jitfusion::FunctionNode *>($$.get())->AppendArgs(std::move($3));
}
| "switch" "(" args ")" { $$ = std::make_unique<jitfusion::SwitchNode>(std::move($3)); }
;

args:
  arg {
    $$ = std::vector<std::unique_ptr<jitfusion::ExecNode>>{};
    $$.emplace_back(std::move($1));
  }
| args "," arg {
    $1.emplace_back(std::move($3));
    $$ = std::move($1);
  }
;

arg:
  term { $$ = std::move($1); }
;

boolean:
  term "and" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kAnd, std::move($1), std::move($3)); }
| term "or" term { $$ = std::make_unique<jitfusion::BinaryOPNode>(jitfusion::BinaryOPType::kOr, std::move($1), std::move($3)); }

%%

void athena::Parser::error(const location_type &l, const std::string &m) {
  std::stringstream sstream;
  sstream << l << ": " << m;
  builder.parser_error_message_ = sstream.str();
}