#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace cxl {

using index_t = long long signed;

inline namespace detail {
template<index_t, typename...>
struct select_impl;

template<typename T, typename... Ts>
struct select_impl<0, T, Ts...>
{
  using type = T;
};

template<index_t Index, typename T, typename... Ts>
struct select_impl<Index, T, Ts...>
{
  static_assert(Index <= sizeof...(Ts), "parameter pack index out of bounds");
  using type = typename select_impl<Index - 1, Ts...>::type;
};
}

template<index_t AtIndex, typename... Types>
using select_t = typename select_impl<AtIndex, Types...>::type;

template<index_t... Indices>
struct index_range
{
  constexpr auto size() const { return std::integral_constant<index_t, sizeof...(Indices)>{}; }
  template<index_t Index>
  constexpr auto operator[](const std::integral_constant<index_t, Index>) const
  {
    return std::integral_constant<index_t, m_range[Index]>{};
  }

private:
  static constexpr index_t m_range[] = { Indices... };
};

template<index_t Begin, index_t End, index_t Position = Begin, index_t... Result>
constexpr auto
make_index_range()
{
  if constexpr (Begin < End) {
    if constexpr (Position != End)
      return make_index_range<Begin, End, Position + 1, Result..., Position>();
    else
      return index_range<Result..., Position>{};
  } else if constexpr (Begin > End) {
    if constexpr (Position != End)
      return make_index_range<Begin, End, Position - 1, Result..., Position>();
    else
      return index_range<Result..., Position>{};
  } else
    return index_range<Position>{};
}

inline namespace detail {
template<typename Find, typename T0, typename... Ts, index_t Index>
constexpr auto
index_of_impl(const std::integral_constant<index_t, Index> = std::integral_constant<index_t, 0>{})
{
  if constexpr (std::is_same_v<Find, T0>)
    return Index;
  else
    return index_of<Find, Ts...>(std::integral_constant<index_t, Index + 1>{});
}
}

template<typename TypeToFind, typename... TypesToSearch, typename Deduced>
constexpr auto
index_of(const Deduced)
{
  return index_of_impl<TypeToFind, TypesToSearch...>(Deduced{});
}

inline namespace detail {
template<auto N, auto E, index_t... Indices>
constexpr auto
pow_impl(const index_range<Indices...>) -> double
{
  constexpr double abs_pow = (1.0 * ... * (Indices, N));
  if constexpr (E > 0)
    return abs_pow;
  if constexpr (E < 0)
    return 1.0 / abs_pow;
  else
    return 1.0;
}
}

template<auto N, auto E>
constexpr auto
pow() -> double
{
  if constexpr (N == 0)
    return 0.0;
  if constexpr (E > 0)
    return pow_impl<N, E>(make_index_range<0, E - 1>());
  else
    return pow_impl<N, E>(make_index_range<0, E + 1>());
}

inline namespace detail {
template<class... Digits>
constexpr index_t
combine_digits_base10(index_t result, index_t digit0, Digits... digits)
{
  if constexpr (sizeof...(Digits) == 0)
    return result + digit0;
  else
    return combine_digits_base10(result + (pow<10, sizeof...(Digits)>() * digit0), digits...);
}

constexpr index_t
parse_digit(char C)
{
  return (C >= '0' && C <= '9') ? C - '0' : throw std::out_of_range("only decimal digits are allowed");
}
}
}
