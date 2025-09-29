#pragma once
#include "StorageEngine/GraphStatistics.hpp"
#include "StorageEngine/Utils.hpp" // for Unweighted
#include <functional>
#include <optional>
#include <string>
#include <type_traits>

namespace CinderPeak::Traits {

// True if EdgeType is Unweighted
template <typename T> struct is_unweighted : std::is_same<T, Unweighted> {};

template <typename T> constexpr bool is_unweighted_v = is_unweighted<T>::value;

// True if EdgeType is not Unweighted (i.e., weighted)
template <typename T>
struct is_weighted : std::integral_constant<bool, !is_unweighted_v<T>> {};

template <typename T> constexpr bool is_weighted_v = is_weighted<T>::value;

// True if EdgeType is numeric
template <typename T> struct is_numeric_edge : std::is_arithmetic<T> {};

template <typename T>
constexpr bool is_numeric_edge_v = is_numeric_edge<T>::value;

// True if EdgeType is a pointer
template <typename T> struct is_pointer_edge : std::is_pointer<T> {};

template <typename T>
constexpr bool is_pointer_edge_v = is_pointer_edge<T>::value;

// True if EdgeType is optional
template <typename T> struct is_optional_edge : std::false_type {};

template <typename T>
struct is_optional_edge<std::optional<T>> : std::true_type {};

template <typename T>
constexpr bool is_optional_edge_v = is_optional_edge<T>::value;

// True if VertexType is primitive or string
template <typename T>
struct is_primitive_or_string
    : std::disjunction<std::is_arithmetic<T>, std::is_same<T, std::string>> {};

template <typename T>
constexpr bool is_primitive_or_string_v = is_primitive_or_string<T>::value;

// True if VertexType is enum
template <typename T> struct is_enum_vertex : std::is_enum<T> {};

template <typename T>
constexpr bool is_enum_vertex_v = is_enum_vertex<T>::value;

// True if VertexType is comparable (supports <)
template <typename T, typename = void>
struct is_comparable_vertex : std::false_type {};

template <typename T>
struct is_comparable_vertex<
    T, std::void_t<decltype(std::declval<T>() < std::declval<T>())>>
    : std::true_type {};

template <typename T>
constexpr bool is_comparable_vertex_v = is_comparable_vertex<T>::value;

// True if VertexType is hashable (usable in unordered_map/set)
template <typename T, typename = void> struct is_hashable : std::false_type {};

template <typename T>
struct is_hashable<T, std::void_t<decltype(std::hash<T>{}(std::declval<T>()))>>
    : std::true_type {};

template <typename T> constexpr bool is_hashable_v = is_hashable<T>::value;

// Primitive, enum, or string
template <typename T>
struct is_primitive_enum_or_string
    : std::disjunction<is_primitive_or_string<T>, is_enum_vertex<T>> {};

template <typename T>
constexpr bool is_primitive_enum_or_string_v =
    is_primitive_enum_or_string<T>::value;

// Weighted numeric edge
template <typename T>
struct is_weighted_numeric_edge
    : std::integral_constant<bool, is_weighted_v<T> && is_numeric_edge_v<T>> {};

template <typename T>
constexpr bool is_weighted_numeric_edge_v = is_weighted_numeric_edge<T>::value;

// Unweighted + integral vertex
template <typename V, typename E>
struct is_unweighted_integral_vertex
    : std::integral_constant<bool, is_unweighted_v<E> &&
                                       std::is_integral<V>::value> {};

template <typename V, typename E>
constexpr bool is_unweighted_integral_vertex_v =
    is_unweighted_integral_vertex<V, E>::value;

// Runtime helper
template <typename T> constexpr bool isTypePrimitive() {
  if constexpr (is_primitive_or_string_v<T>) {
    return true;
  }
  return false;
}
template <typename T> constexpr bool isGraphWeighted() {
  if constexpr (is_weighted_v<T>) {
    return true;
  }
  return false;
}

#define STATIC_ASSERT_WEIGHTED(E)                                              \
  static_assert(CinderPeak::Traits::is_weighted_v<E>,                          \
                "EdgeType must be weighted (not Unweighted).")

#define STATIC_ASSERT_UNWEIGHTED(E)                                            \
  static_assert(CinderPeak::Traits::is_unweighted_v<E>,                        \
                "EdgeType must be Unweighted.")

#define STATIC_ASSERT_NUMERIC_EDGE(E)                                          \
  static_assert(CinderPeak::Traits::is_numeric_edge_v<E>,                      \
                "EdgeType must be numeric for this algorithm.")

#define STATIC_ASSERT_COMPARABLE_VERTEX(V)                                     \
  static_assert(CinderPeak::Traits::is_comparable_vertex_v<V>,                 \
                "VertexType must support operator< for this algorithm.")

} // namespace CinderPeak::Traits
