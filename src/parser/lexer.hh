#ifndef TINT3_PARSER_LEXER_HH
#define TINT3_PARSER_LEXER_HH

#include <functional>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace parser {
namespace matcher {

// NewLine matcher accepts DOS-style (<CR><LF>), Macintosh-style (<CR>) or
// Unix-style (<LF>) newline sequences.
bool NewLine(std::string const& buffer, unsigned int* position,
             std::string* output);

// Whitespace matcher accepts all the characters accepted by isspace(), except
// for the newline character (which is usually to be accepted as a separate
// token, and not swallowed by this matcher).
bool Whitespace(std::string const& buffer, unsigned int* position,
                std::string* output);

// Any matcher accepts any single character.
bool Any(std::string const& buffer, unsigned int* position,
         std::string* output);

}  // namespace matcher

using Symbol = unsigned int;

static constexpr Symbol kEOF = 0;

class TokenMatcher {
 public:
  using MatcherCallback = bool(std::string const&, unsigned int*, std::string*);
  using MatcherFunction = std::function<MatcherCallback>;

  TokenMatcher(TokenMatcher const& other);
  TokenMatcher(TokenMatcher&& other);

  TokenMatcher(MatcherCallback matcher);
  TokenMatcher(char c);
  TokenMatcher(const char* regexp);
  TokenMatcher(std::string const& regexp);

  TokenMatcher& operator=(TokenMatcher other);
  bool operator()(std::string const& buffer, unsigned int* position,
                  std::string* output) const;

 private:
  MatcherFunction matcher_;

  static MatcherFunction MakeCharacterMatcher(char c);
  static MatcherFunction MakeRegexpMatcher(std::string const& pattern);
};

class Token {
 public:
  Symbol const symbol;
  unsigned int const begin;
  unsigned int const end;
  std::string const match;

  Token(Symbol symbol, unsigned int begin, unsigned int end,
        std::string const& match);
};

class Lexer {
 public:
  using Result = std::vector<Token>;
  using MatchPair = std::pair<TokenMatcher, Symbol>;

  Lexer(Lexer const& other);
  Lexer(Lexer&& other);
  explicit Lexer(std::initializer_list<MatchPair> const& match_map);

  Lexer& operator=(Lexer other);

  bool ProcessContents(std::string const& buffer, Result* result) const;

 private:
  // This could be an std::map for brevity, but we want to preserve the
  // given matcher ordering.
  std::vector<MatchPair> matcher_to_symbol_;
};

}  // namespace parser

#endif  // TINT3_PARSER_LEXER_HH
