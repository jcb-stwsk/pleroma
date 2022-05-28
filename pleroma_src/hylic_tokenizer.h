#pragma once

#include "hylic_compex.h"
#include <list>
#include <string>
#include <vector>

enum class TokenType {
  Newline,
  Tab,
  EndOfFile,

  LeftParen,
  RightParen,
  LeftBrace,
  RightBrace,

  Actor,

  Plus,
  Minus,
  Star,
  Slash,

  Equals,
  GreaterThan,
  GreaterThanEqual,
  LessThan,
  LessThanEqual,
  NotEqual,
  Not,

  Pound,
  LeftBracket,
  RightBracket,
  Dollar,
  Bang,
  Dot,
  Comma,
  Colon,
  Breakthrough,

  Import,
  Match,
  True,
  False,
  Function,
  Pure,
  For,
  While,
  Return,
  Fallthrough,

  PromiseType,

  LocVar,
  FarVar,
  AlnVar,

  Message,

  // Literals
  Number,
  String,
  Character,
  Symbol,

};

struct Token {
  TokenType type;
  std::string lexeme;
};

struct TokenStream {
  std::list<Token *> tokens;
  std::list<Token *>::iterator current;

  int line_number = 0;
  int char_number = 0;

  Token *peek();
  Token *peek_forward();
  void go_back(int n);
  Token *get();
  void reset();

  void expect(TokenType t);
  Token *check(TokenType t);
  Token *accept(TokenType t);
  void add_token(TokenType t, std::string lexeme);
  std::vector<Token *> accept_all(std::vector<TokenType> toks);
};

struct TokenizerException : CompileException {};

TokenStream* tokenize_file(FILE *f);

const char *token_type_to_string(TokenType t);
