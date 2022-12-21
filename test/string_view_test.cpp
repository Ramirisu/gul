//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/string_view.hpp>

using namespace gul;

TEST_SUITE_BEGIN("string_view");

namespace test {
const char* s = "hello world!";
}

TEST_CASE("basic")
{
  {
    string_view sv;
    CHECK(sv.empty());
    CHECK_EQ(sv.data(), nullptr);
    CHECK_EQ(sv.size(), 0);
  }
  {
    string_view sv(test::s);
    CHECK(!sv.empty());
    CHECK_EQ(sv.data(), test::s);
    CHECK_EQ(sv.size(), strlen(test::s));
    CHECK_EQ(sv[0], 'h');
    CHECK_EQ(sv.at(0), 'h');
    CHECK_EQ(sv[1], 'e');
    CHECK_EQ(sv.at(1), 'e');
    CHECK_EQ(sv[2], 'l');
    CHECK_EQ(sv.at(2), 'l');
#if !GUL_NO_EXCEPTIONS
    CHECK_THROWS_AS(sv.at(sv.size()), std::out_of_range);
#endif
    CHECK_EQ(sv.front(), 'h');
    CHECK_EQ(sv.back(), '!');
    CHECK_EQ(sv.length(), strlen(test::s));
  }
  {
    string_view sv(test::s, 6);
    CHECK(!sv.empty());
    CHECK_EQ(sv.data(), test::s);
    CHECK_EQ(sv.size(), 6);
  }
  {
    std::string str(test::s);
    string_view sv(str);
    CHECK_EQ(sv.data(), str.data());
    CHECK_EQ(sv.size(), str.size());
  }
}

TEST_CASE("remove_prefix")
{
  string_view sv(test::s);
  sv.remove_prefix(3);
  CHECK_EQ(sv, string_view(test::s + 3));
}

TEST_CASE("remove_suffix")
{
  string_view sv(test::s);
  sv.remove_suffix(3);
  CHECK_EQ(sv, string_view(test::s, strlen(test::s) - 3));
}

TEST_CASE("swap")
{
  string_view sv(test::s);
  string_view empty;
  swap(sv, empty);
  CHECK_EQ(sv, string_view());
  CHECK_EQ(empty, string_view(test::s));
}

TEST_CASE("copy")
{
  string_view sv(test::s);
  char buf[16] = {};
#if !GUL_NO_EXCEPTIONS
  CHECK_THROWS_AS(sv.copy(buf, sv.size(), sv.size() + 1), std::out_of_range);
#endif
  CHECK_EQ(sv.copy(buf, 3, 6), 3);
  CHECK_EQ(string_view(test::s + 6, 3).compare(0, 3, buf), 0);
  CHECK_EQ(sv.copy(buf, 12, 6), 6);
  CHECK_EQ(string_view(test::s + 6).compare(0, 6, buf), 0);
}

TEST_CASE("substr")
{
  string_view sv(test::s);
  CHECK_EQ(sv.substr(), sv);
  CHECK_EQ(sv.substr(6), string_view(test::s + 6));
  CHECK_EQ(sv.substr(12), string_view(test::s + 12));
#if !GUL_NO_EXCEPTIONS
  CHECK_THROWS_AS(sv.substr(13, 0), std::out_of_range);
#endif
  CHECK_EQ(sv.substr(0, 6), string_view(test::s, 6));
  CHECK_EQ(sv.substr(0, 16), string_view(test::s));
  CHECK_EQ(sv.substr(6, 16), string_view(test::s + 6));
}

TEST_CASE("first")
{
  string_view sv(test::s);
  CHECK_EQ(sv.first(0), string_view());
  CHECK_EQ(sv.first(6), string_view(test::s, 6));
  CHECK_EQ(sv.first(12), string_view(test::s, 12));
}

TEST_CASE("last")
{
  string_view sv(test::s);
  CHECK_EQ(sv.last(0), string_view());
  CHECK_EQ(sv.last(6), string_view(test::s + 6, 6));
  CHECK_EQ(sv.last(12), string_view(test::s, 12));
}

TEST_CASE("starts_with")
{
  string_view sv(test::s);
  CHECK(sv.starts_with('h'));
  CHECK(sv.starts_with("hello"));
  CHECK(!sv.starts_with('e'));
  CHECK(!sv.starts_with("ello"));
  CHECK(sv.ends_with('!'));
  CHECK(sv.ends_with("world!"));
  CHECK(!sv.ends_with('d'));
  CHECK(!sv.ends_with("world"));
}

TEST_CASE("contains")
{
  string_view sv(test::s);
  CHECK(sv.contains('o'));
  CHECK(sv.contains("hello"));
  CHECK(sv.contains("world!"));
  CHECK(sv.contains("llo wor"));
  CHECK(!sv.contains('a'));
  CHECK(!sv.contains("cde"));
}

TEST_CASE("compare")
{
  CHECK_EQ(string_view().compare(""), 0);
  CHECK_EQ(string_view("").compare(""), 0);
  CHECK_EQ(string_view("1").compare(""), 1);
  CHECK_EQ(string_view("").compare("1"), -1);
  CHECK_EQ(string_view("11").compare("11"), 0);
  CHECK_EQ(string_view("10").compare("01"), 1);
  CHECK_EQ(string_view("01").compare("10"), -1);
  CHECK_EQ(string_view("111").compare(1, 1, "11"), -1);
  CHECK_EQ(string_view("111").compare(1, 2, "11"), 0);
  CHECK_EQ(string_view("111").compare(1, 3, "11"), 0);
  CHECK_EQ(string_view("111").compare(0, 1, "11", 0), 1);
  CHECK_EQ(string_view("111").compare(0, 1, "11", 1), 0);
  CHECK_EQ(string_view("111").compare(0, 1, "11", 2), -1);
  CHECK_EQ(string_view("111").compare(1, 1, "11", 0), 1);
  CHECK_EQ(string_view("111").compare(1, 1, "11", 1), 0);
  CHECK_EQ(string_view("111").compare(1, 1, "11", 2), -1);
  CHECK_EQ(string_view("111").compare(2, 1, "11", 0), 1);
  CHECK_EQ(string_view("111").compare(2, 1, "11", 1), 0);
  CHECK_EQ(string_view("111").compare(2, 1, "11", 2), -1);
}

TEST_CASE("find")
{
  string_view sv(test::s);
  CHECK_EQ(sv.find(""), 0);
  CHECK_EQ(sv.find('h'), 0);
  CHECK_EQ(sv.find("h"), 0);
  CHECK_EQ(sv.find(' '), 5);
  CHECK_EQ(sv.find(" wo"), 5);
  CHECK_EQ(sv.find('!'), 11);
  CHECK_EQ(sv.find("!"), 11);
  CHECK_EQ(sv.find('#'), string_view::npos);
  CHECK_EQ(sv.find("#"), string_view::npos);
  CHECK_EQ(sv.find("", 1), 1);
  CHECK_EQ(sv.find("", 16), string_view::npos);
  CHECK_EQ(sv.find('h', 1), string_view::npos);
  CHECK_EQ(sv.find('l', 4), 9);
  CHECK_EQ(sv.find('h', 16), string_view::npos);
}

TEST_CASE("rfind")
{
  string_view sv(test::s);
  CHECK_EQ(sv.rfind(""), 12);
  CHECK_EQ(sv.rfind('h'), 0);
  CHECK_EQ(sv.rfind("h"), 0);
  CHECK_EQ(sv.rfind(' '), 5);
  CHECK_EQ(sv.rfind(" wo"), 5);
  CHECK_EQ(sv.rfind('!'), 11);
  CHECK_EQ(sv.rfind("!"), 11);
  CHECK_EQ(sv.rfind('#'), string_view::npos);
  CHECK_EQ(sv.rfind("#"), string_view::npos);
  CHECK_EQ(sv.rfind("", 10), 10);
  CHECK_EQ(sv.rfind("", 16), 12);
  CHECK_EQ(sv.rfind('h', 1), 0);
  CHECK_EQ(sv.rfind('l', 8), 3);
  CHECK_EQ(sv.rfind('!', 10), string_view::npos);
}

TEST_CASE("find_first_of")
{
  string_view sv(test::s);
  CHECK_EQ(sv.find_first_of('h'), 0);
  CHECK_EQ(sv.find_first_of("h"), 0);
  CHECK_EQ(sv.find_first_of("lo"), 2);
  CHECK_EQ(sv.find_first_of(' '), 5);
  CHECK_EQ(sv.find_first_of(" "), 5);
  CHECK_EQ(sv.find_first_of('!'), 11);
  CHECK_EQ(sv.find_first_of("!"), 11);
  CHECK_EQ(sv.find_first_of('#'), string_view::npos);
  CHECK_EQ(sv.find_first_of("#"), string_view::npos);
  CHECK_EQ(sv.find_first_of('h', 1), string_view::npos);
  CHECK_EQ(sv.find_first_of("h", 1), string_view::npos);
}

TEST_CASE("find_last_of")
{
  string_view sv(test::s);
  CHECK_EQ(sv.find_last_of('h'), 0);
  CHECK_EQ(sv.find_last_of("h"), 0);
  CHECK_EQ(sv.find_last_of("lo"), 9);
  CHECK_EQ(sv.find_last_of(' '), 5);
  CHECK_EQ(sv.find_last_of(" "), 5);
  CHECK_EQ(sv.find_last_of('!'), 11);
  CHECK_EQ(sv.find_last_of("!"), 11);
  CHECK_EQ(sv.find_last_of('#'), string_view::npos);
  CHECK_EQ(sv.find_last_of("#"), string_view::npos);
  CHECK_EQ(sv.find_last_of('!', 10), string_view::npos);
  CHECK_EQ(sv.find_last_of("!#", 10), string_view::npos);
}

TEST_CASE("find_first_not_of")
{
  string_view sv(test::s);
  CHECK_EQ(sv.find_first_not_of('h'), 1);
  CHECK_EQ(sv.find_first_not_of("h"), 1);
  CHECK_EQ(sv.find_first_not_of("lo"), 0);
  CHECK_EQ(sv.find_first_not_of('#'), 0);
  CHECK_EQ(sv.find_first_not_of("#"), 0);
  CHECK_EQ(sv.find_first_not_of('!'), 0);
  CHECK_EQ(sv.find_first_not_of("!"), 0);
  CHECK_EQ(sv.find_first_not_of(test::s), string_view::npos);
  CHECK_EQ(sv.find_first_not_of('h', 1), 1);
  CHECK_EQ(sv.find_first_not_of("h", 1), 1);
}

TEST_CASE("find_last_not_of")
{
  string_view sv(test::s);
  CHECK_EQ(sv.find_last_not_of('h'), 11);
  CHECK_EQ(sv.find_last_not_of("h"), 11);
  CHECK_EQ(sv.find_last_not_of("lo"), 11);
  CHECK_EQ(sv.find_last_not_of(' '), 11);
  CHECK_EQ(sv.find_last_not_of(" "), 11);
  CHECK_EQ(sv.find_last_not_of('!'), 10);
  CHECK_EQ(sv.find_last_not_of("!"), 10);
  CHECK_EQ(sv.find_last_not_of('#'), 11);
  CHECK_EQ(sv.find_last_not_of("#"), 11);
  CHECK_EQ(sv.find_last_not_of('!', 10), 10);
  CHECK_EQ(sv.find_last_not_of("!#", 10), 10);
  CHECK_EQ(sv.find_last_not_of(""), 11);
  CHECK_EQ(string_view().find_last_not_of(""), string_view::npos);
}

TEST_CASE("iterator")
{
  string_view sv(test::s);
  auto it = sv.begin();
  CHECK_EQ(it, sv.cbegin());
  CHECK_EQ(*it++, 'h');
  CHECK_EQ(*it++, 'e');
  CHECK_EQ(*it++, 'l');
  CHECK_EQ(*it++, 'l');
  CHECK_EQ(*it--, 'o');
  CHECK_EQ(*it, 'l');
  CHECK_EQ(it[2], ' ');
  CHECK_EQ(*(it += 5), 'r');
  CHECK_EQ(*(it -= 3), ' ');
  CHECK_EQ(*(it + 1), 'w');
  CHECK_EQ(*(2 + it), 'o');
  CHECK_EQ(*(it - 4), 'e');
  CHECK_EQ(sv.end() - it, 7);
  CHECK_EQ(*(it += 6), '!');
  ++it;
  CHECK_EQ(it, sv.end());
  CHECK_EQ(it, sv.cend());
  CHECK_NE(it, sv.begin());
  CHECK_FALSE(sv.begin() < sv.begin());
  CHECK(sv.begin() < sv.end());
  CHECK(sv.begin() <= sv.begin());
  CHECK(sv.begin() <= sv.end());
  CHECK_FALSE(sv.end() > sv.end());
  CHECK(sv.end() > sv.begin());
  CHECK(sv.end() >= sv.end());
  CHECK(sv.end() >= sv.begin());
}

TEST_CASE("reverse iterator")
{
  string_view sv(test::s);
  auto it = sv.rbegin();
  CHECK_EQ(it, sv.crbegin());
  CHECK_EQ(*it++, '!');
  CHECK_EQ(*it++, 'd');
  CHECK_EQ(*it++, 'l');
  CHECK_EQ(*it++, 'r');
  it += 8;
  CHECK_EQ(it, sv.rend());
  CHECK_EQ(it, sv.crend());
}

TEST_CASE("converting operator")
{
  CHECK_EQ(static_cast<std::string>(string_view(test::s)),
           std::string(test::s));
#if defined(GUL_HAS_CXX17)
  CHECK_EQ(static_cast<std::string_view>(string_view(test::s)),
           std::string_view(test::s));
#endif
}

TEST_CASE("comparator")
{
  CHECK(string_view() == string_view());
  CHECK(!(string_view() == string_view("1")));
  CHECK(!(string_view() != string_view()));
  CHECK(string_view() != string_view("1"));
  CHECK(string_view() < string_view("1"));
  CHECK(!(string_view("1") < string_view("1")));
  CHECK(!(string_view("1") < string_view()));
  CHECK(string_view() <= string_view("1"));
  CHECK(string_view("1") <= string_view("1"));
  CHECK(!(string_view("1") <= string_view()));
  CHECK(!(string_view() > string_view("1")));
  CHECK(!(string_view("1") > string_view("1")));
  CHECK(string_view("1") > string_view());
  CHECK(!(string_view() >= string_view("1")));
  CHECK(string_view("1") >= string_view("1"));
  CHECK(string_view("1") >= string_view());
}

TEST_CASE("user-defined literals")
{
  auto sv = "hello world!"_sv;
  STATIC_ASSERT_SAME(decltype(sv), string_view);
  CHECK_EQ(sv, string_view("hello world!"));
  auto wsv = L"hello world!"_sv;
  STATIC_ASSERT_SAME(decltype(wsv), wstring_view);
  CHECK_EQ(wsv, wstring_view(L"hello world!"));
  auto u16sv = u"hello world!"_sv;
  STATIC_ASSERT_SAME(decltype(u16sv), u16string_view);
  CHECK_EQ(u16sv, u16string_view(u"hello world!"));
  auto u32sv = U"hello world!"_sv;
  STATIC_ASSERT_SAME(decltype(u32sv), u32string_view);
  CHECK_EQ(u32sv, u32string_view(U"hello world!"));
#if defined(GUL_HAS_CXX20) || defined(__cpp_char8_t)
  auto u8sv = u8"hello world!"_sv;
  STATIC_ASSERT_SAME(decltype(u8sv), u8string_view);
  CHECK_EQ(u8sv, u8string_view(u8"hello world!"));
#endif
}

TEST_CASE("hash")
{
  std::string str(test::s);
  string_view sv(test::s);
  CHECK_EQ(std::hash<std::string>()(str), std::hash<string_view>()(sv));

  const wchar_t* ws = L"hello world!";
  std::wstring wstr(ws);
  wstring_view wsv(ws);
  CHECK_EQ(std::hash<std::wstring>()(wstr), std::hash<wstring_view>()(wsv));

  const char16_t* u16s = u"hello world!";
  std::u16string u16str(u16s);
  u16string_view u16sv(u16s);
  CHECK_EQ(std::hash<std::u16string>()(u16str),
           std::hash<u16string_view>()(u16sv));

  const char32_t* u32s = U"hello world!";
  std::u32string u32str(u32s);
  u32string_view u32sv(u32s);
  CHECK_EQ(std::hash<std::u32string>()(u32str),
           std::hash<u32string_view>()(u32sv));

#if defined(GUL_HAS_CXX20) || defined(__cpp_char8_t)
  const char8_t* u8s = u8"hello world!";
  std::u8string u8str(u8s);
  u8string_view u8sv(u8s);
  CHECK_EQ(std::hash<std::u8string>()(u8str), std::hash<u8string_view>()(u8sv));
#endif
}

TEST_SUITE_END();
