#include "catch.hpp"

#include <string>
#include <vector>
#include "util/common.hh"

TEST_CASE("StringTrim", "Removing trailing spaces from strings should work") {
  std::string s;

  SECTION("Removes spaces from the left end") {
    s = "      text";
    REQUIRE(util::string::Trim(&s) == "text");
  }

  SECTION("Removes spaces from the right end") {
    s = "text      ";
    REQUIRE(util::string::Trim(&s) == "text");
  }

  SECTION("Removes spaces from both ends") {
    s = "   text   ";
    REQUIRE(util::string::Trim(&s) == "text");
  }
}

TEST_CASE("SplitString",
          "Splitting a string using a character separator should work") {
  // Empty string: no matter the separator, results in a vector with an
  // empty string in it.
  REQUIRE(util::string::Split("", '*') == std::vector<std::string>{""});
  // Non-empty string, separator not found: results in a vector with the string
  // itself in it.
  REQUIRE(util::string::Split("test", '*') == std::vector<std::string>{"test"});
  // Non-empty string, separator found: separated as intuitively expected.
  REQUIRE(util::string::Split("This is a string", ' ') ==
          std::vector<std::string>({"This", "is", "a", "string"}));
  // String composed of the separator only: results in a vector with two
  // empty strings in it (the empty left and right halves).
  REQUIRE(util::string::Split("/", '/') == std::vector<std::string>({"", ""}));
  // String with the separator at the end: results in a vector with the left
  // substring, and an empty string.
  REQUIRE(util::string::Split("something//something", '/') ==
          std::vector<std::string>({"something", "", "something"}));
  // String with two continguous separators: results in a vector with the left
  // substring, an empty string, and the right substring.
  REQUIRE(util::string::Split("something//something", '/') ==
          std::vector<std::string>({"something", "", "something"}));
}

TEST_CASE("StartsWith",
          "Checking if a string has a prefix works even in corner cases") {
  // normal case: left string longer
  REQUIRE(util::string::StartsWith("tautology", ""));
  REQUIRE(util::string::StartsWith("left", "le"));

  // corner case: same length
  REQUIRE(util::string::StartsWith("", ""));
  REQUIRE(util::string::StartsWith("same", "same"));

  // corner case: right string longer
  REQUIRE_FALSE(util::string::StartsWith("", "tautology"));
  REQUIRE_FALSE(util::string::StartsWith("man", "mannequin"));
}

TEST_CASE("RegexMatch",
          "Matching a string by a regular expression should work") {
  SECTION("Character classes") {
    REQUIRE(util::string::RegexMatch("[0-9]", "0"));
    REQUIRE_FALSE(util::string::RegexMatch("[0-9]", "Z"));
  }

  SECTION("Any numer of occurrences (Kleene star)") {
    REQUIRE(util::string::RegexMatch("test[0-9]*", "test"));
    REQUIRE(util::string::RegexMatch("test[0-9]*", "test0"));
    REQUIRE(util::string::RegexMatch("test[0-9]*", "test01"));
  }

  SECTION("One or more occurrences (Kleene plus)") {
    REQUIRE_FALSE(util::string::RegexMatch("test[0-9]+", "test"));
    REQUIRE(util::string::RegexMatch("test[0-9]+", "test0"));
    REQUIRE(util::string::RegexMatch("test[0-9]+", "test01"));
  }

  SECTION("At most one occurrence") {
    REQUIRE(util::string::RegexMatch("test[0-9]?", "test"));
    REQUIRE(util::string::RegexMatch("test[0-9]?", "test0"));
    REQUIRE_FALSE(util::string::RegexMatch("test[0-9]?", "test01"));
  }
}

TEST_CASE("StringRepresentation") {
  SECTION("Pointers") {
    REQUIRE(util::string::Representation(nullptr) == "");
    REQUIRE(util::string::Representation(reinterpret_cast<void*>(0x1234)) ==
            "0x1234");
  }

  SECTION("Numbers") {
    REQUIRE(util::string::Representation(1234) == "1234");
    REQUIRE(util::string::Representation(-1234) == "-1234");
    REQUIRE(util::string::Representation(.1234) == "0.1234");
  }
}

TEST_CASE("AdjustASB",
          "Adjustments are in agreement with http://colorizer.org/") {
  // A 2x2 bitmap.
  DATA32 image_data[] = {0x00000000, 0xffffffff, 0xa0b0c0d0, 0x0a0b0c0d};

  // Increase brightness by 10%.
  AdjustASB(image_data, 2, 2, 100, 0.0, +0.1);

  // Transparent pixel, shouldn't change.
  REQUIRE(image_data[0] == 0x00000000);
  // White pixel, shouldn't change.
  REQUIRE(image_data[1] == 0xffffffff);
  // Bright pixel, should be made brighter.
  REQUIRE(image_data[2] == 0xa0c6d8ea);
  // Dark pixel, should be made brighter.
  REQUIRE(image_data[3] == 0x0a212427);
}

TEST_CASE("ScopedDeleter") {
  bool was_invoked = false;
  {
    auto _ = util::MakeScopedDeleter([&] { was_invoked = true; });
    REQUIRE_FALSE(was_invoked);
  }
  REQUIRE(was_invoked);
}

TEST_CASE("ToNumber") {
  SECTION("int") {
    int i;
    REQUIRE(util::string::ToNumber("0", &i));
    REQUIRE(i == 0);  // parsed and read correctly
    REQUIRE(util::string::ToNumber("900\n", &i));
    REQUIRE(i == 900);  // parsed and read correctly
    REQUIRE(util::string::ToNumber("1234", &i));
    REQUIRE(i == 1234);  // parsed and read correctly
    REQUIRE_FALSE(util::string::ToNumber("5678abcd", &i));
    REQUIRE(i == 1234);  // failed parsing, unchanged
  }

  SECTION("long") {
    long l;
    REQUIRE(util::string::ToNumber("0", &l));
    REQUIRE(l == 0);  // parsed and read correctly
    REQUIRE(util::string::ToNumber("900\n", &l));
    REQUIRE(l == 900);  // parsed and read correctly
    REQUIRE(util::string::ToNumber("1234", &l));
    REQUIRE(l == 1234);  // parsed and read correctly
    REQUIRE_FALSE(util::string::ToNumber("5678abcd", &l));
    REQUIRE(l == 1234);  // failed parsing, unchanged
  }

  SECTION("float") {
    float f;
    REQUIRE(util::string::ToNumber("0", &f));
    REQUIRE(f == 0);  // parsed and read correctly
    REQUIRE(util::string::ToNumber("900\n", &f));
    REQUIRE(f == 900);  // parsed and read correctly
    REQUIRE(util::string::ToNumber("1.234", &f));
    REQUIRE(f == 1.234f);  // parsed and read correctly
    REQUIRE_FALSE(util::string::ToNumber("5.678abcd", &f));
    REQUIRE(f == 1.234f);  // failed parsing, unchanged

    // Out of range values are rejected and writing to the provided pointer
    // is not attempted in such a case.
    float* nptr = nullptr;
    REQUIRE_FALSE(util::string::ToNumber("inf", nptr));
    REQUIRE_FALSE(util::string::ToNumber("infinity", nptr));
    REQUIRE_FALSE(util::string::ToNumber("nan", nptr));
    REQUIRE_FALSE(util::string::ToNumber("nan(16)", nptr));
  }
}

TEST_CASE("iterator_range") {
  std::vector<int> container = {1, 2, 3, 4, 5};

  SECTION("make_iterator_range") {
    std::vector<int> result;
    for (auto& x : util::make_iterator_range(container.begin() + 1,
                                             container.end() - 1)) {
      result.push_back(x);
    }

    for (size_t i = 1; i < container.size() - 1; ++i) {
      REQUIRE(result[i - 1] == container[i]);
    }
  }

  SECTION("range_skip_n(0)") {
    std::vector<int> result;
    for (auto& x : util::range_skip_n(container, 0)) {
      result.push_back(x);
    }

    for (size_t i = 0; i < container.size(); ++i) {
      REQUIRE(result[i] == container[i]);
    }
  }

  SECTION("range_skip_n(2)") {
    std::vector<int> result;
    for (auto& x : util::range_skip_n(container, 2)) {
      result.push_back(x);
    }

    for (size_t i = 2; i < container.size(); ++i) {
      REQUIRE(result[i - 2] == container[i]);
    }
  }
}
