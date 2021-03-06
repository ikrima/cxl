#pragma once

#include "integral.hpp"
#include "iterator.hpp"
#include "typelist.hpp"
#include "utility.hpp"
#include <type_traits>

namespace cxl
{

template <char... Chars>
struct string
{
  using value_type = char;

  constexpr operator const char *() const { return &m_data[0]; }

  template <index_t Index>
  constexpr auto operator[](::std::integral_constant<index_t, Index>) const
  {
    return ::std::integral_constant<char, m_data[Index]>{};
  }

  constexpr auto size() const { return m_size; }

  constexpr char front() const { return *begin(); }
  constexpr char back() const { return *(--end()); }

  constexpr auto begin() const { return iterator<string<Chars...>, 0>{}; }
  constexpr auto end() const { return iterator<string<Chars...>, sizeof...(Chars)>{}; }

  constexpr bool operator==(string<Chars...>) const { return true; }

  template <char... OtherChars>
  constexpr bool operator==(string<OtherChars...>) const
  {
    return false;
  }

private:
  static constexpr ::std::integral_constant<index_t, sizeof...(Chars)> m_size = {};
  static constexpr char m_data[sizeof...(Chars) + 1] = {Chars..., '\0'};
};

inline namespace detail
{
template <typename String, size_t... Indices>
constexpr auto
build_string_impl(String, std::index_sequence<Indices...>)
{
  return string<String{}.chars[Indices]...>{};
}
} // namespace detail

template <size_t Size, typename LiteralWrapper>
constexpr auto
build_string()
{
  if constexpr (Size == 0)
    return string<>{};
  else
    return detail::build_string_impl(LiteralWrapper{}, std::make_index_sequence<Size>());
}

#define STR(string_literal)                                                  \
  []() constexpr                                                             \
  {                                                                          \
    struct literal_wrapper                                                   \
    {                                                                        \
      const char *chars = string_literal;                                    \
    };                                                                       \
    return cxl::build_string<sizeof(string_literal) - 1, literal_wrapper>(); \
  }                                                                          \
  ()

#define CHR(char_literal) \
  ::std::integral_constant<char, char_literal> {}

template <char... L, char... R>
constexpr string<L..., R...>
operator+(string<L...>, string<R...>)
{
  return string<L..., R...>{};
}

inline namespace detail
{
template <typename String, index_t... Indices>
constexpr auto
substr_impl(String, index_range<Indices...>)
{
  return string<String{}[Indices]...>{};
}
} // namespace detail

// creates a sub-string from a pair of iterators
template <typename String, index_t Begin, index_t End>
constexpr auto substr(iterator<String, Begin>, iterator<String, End>)
{
  if constexpr (Begin == End)
    return string<>{};
  else
    return detail::substr_impl(String{}, make_index_range<Begin, End - 1>());
}

// creates a sub-string from a string, position and length
template <typename String, index_t Pos, index_t Len>
constexpr auto substr(String, ::std::integral_constant<index_t, Pos>, ::std::integral_constant<index_t, Len>)
{
  return detail::substr_impl(String{}, make_index_range<Pos, (Pos + Len) - 1>());
}

inline namespace detail
{
template <typename Target, typename String, index_t... Indices>
constexpr auto
strmatch_impl(Target, String, index_range<Indices...>)
{
  constexpr auto match_char = [](index_t index) -> index_t { return Target{}[index] == String{}[index] ? 1 : 0; };
  return ::std::integral_constant<index_t, (0 + ... + match_char(Indices))>{};
}
} // namespace detail

// returns the amount of characters that each string have in common, from beginning until first non-matching character
template <typename Target, typename String>
constexpr auto strmatch(Target, String)
{
  return strmatch_impl(Target{}, String{}, make_index_range<0, Target{}.size() - 1>());
}

template <typename Target, typename String, typename Begin>
constexpr auto find(Target, String, Begin)
{
  constexpr Target target;
  constexpr String string;
  constexpr Begin begin;
  if constexpr (*begin == target)
    return begin;
  else
    return find(target, string, ++begin);
}

template <char Begin, char... Chars>
constexpr auto
stoi(string<Begin, Chars...>)
{
  if constexpr (Begin == '-')
  {
    return -(::std::integral_constant<int, combine_digits_base10(0, parse_digit(Chars)...)>{});
  }
  else if constexpr (Begin == '+')
  {
    return ::std::integral_constant<int, combine_digits_base10(0, parse_digit(Chars)...)>{};
  }
  else
  {
    return ::std::integral_constant<int, combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template <char Begin, char... Chars>
constexpr auto
stol(string<Begin, Chars...>)
{
  if constexpr (Begin == '-')
  {
    return -(::std::integral_constant<long, combine_digits_base10(0, parse_digit(Chars)...)>{});
  }
  else if constexpr (Begin == '+')
  {
    return ::std::integral_constant<long, combine_digits_base10(0, parse_digit(Chars)...)>{};
  }
  else
  {
    return ::std::integral_constant<long, combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template <char Begin, char... Chars>
constexpr auto
stoll(string<Begin, Chars...>)
{
  if constexpr (Begin == '-')
  {
    return -(::std::integral_constant<long long, combine_digits_base10(0, parse_digit(Chars)...)>{});
  }
  else if constexpr (Begin == '+')
  {
    return ::std::integral_constant<long long, combine_digits_base10(0, parse_digit(Chars)...)>{};
  }
  else
  {
    return ::std::integral_constant<long long, combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template <char Begin, char... Chars>
constexpr auto
stoui(string<Begin, Chars...>)
{
  if constexpr (Begin == '-')
  {
    return -(::std::integral_constant<unsigned int, combine_digits_base10(0, parse_digit(Chars)...)>{});
  }
  else if constexpr (Begin == '+')
  {
    return ::std::integral_constant<unsigned int, combine_digits_base10(0, parse_digit(Chars)...)>{};
  }
  else
  {
    return ::std::integral_constant<unsigned int, combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template <char Begin, char... Chars>
constexpr auto
stoul(string<Begin, Chars...>)
{
  if constexpr (Begin == '-')
  {
    return -(::std::integral_constant<unsigned long, combine_digits_base10(0, parse_digit(Chars)...)>{});
  }
  else if constexpr (Begin == '+')
  {
    return ::std::integral_constant<unsigned long, combine_digits_base10(0, parse_digit(Chars)...)>{};
  }
  else
  {
    return ::std::integral_constant<unsigned long, combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template <char Begin, char... Chars>
constexpr auto
stoull(string<Begin, Chars...>)
{
  if constexpr (Begin == '-')
  {
    return -(::std::integral_constant<unsigned long long, combine_digits_base10(0, parse_digit(Chars)...)>{});
  }
  else if constexpr (Begin == '+')
  {
    return ::std::integral_constant<unsigned long long, combine_digits_base10(0, parse_digit(Chars)...)>{};
  }
  else
  {
    return ::std::integral_constant<unsigned long long,
                                    combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template <char Begin, char... Chars>
constexpr float
stof(string<Begin, Chars...>)
{
  if constexpr (Begin == '-')
  {
    constexpr auto work_string = string<Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr float integer_part = stoi(substr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr float fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size())>();
    return -(integer_part + fractional_part);
  }
  else if constexpr (Begin == '+')
  {
    constexpr auto work_string = string<Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr float integer_part = stoi(substr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr float fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size())>();
    return integer_part + fractional_part;
  }
  else
  {
    constexpr auto work_string = string<Begin, Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr float integer_part = stoi(substr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr float fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size())>();
    return integer_part + fractional_part;
  }
}

template <char Begin, char... Chars>
constexpr double
stod(string<Begin, Chars...>)
{
  if constexpr (Begin == '-')
  {
    constexpr auto work_string = string<Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr double integer_part = stoi(substr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr double fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size)>();
    return -(integer_part + fractional_part);
  }
  else if constexpr (Begin == '+')
  {
    constexpr auto work_string = string<Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr double integer_part = stoi(substr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr double fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size)>();
    return integer_part + fractional_part;
  }
  else
  {
    constexpr auto work_string = string<Begin, Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr double integer_part = stoi(substr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr double fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size)>();
    return integer_part + fractional_part;
  }
}
} // namespace cxl
