#pragma once
#include "Concepts.hpp"
#include "Utils.hpp"
#include <cstdint>
#include <string>
#include <type_traits>

namespace CinderPeak {

/**
 * @brief Safe, generic debug string converter for any type used in CinderPeak.
 *
 * Uses compile-time if constexpr (C++17) to handle:
 *   - Primitive types, std::string, enums   → std::to_string / direct return
 *   - Unweighted sentinel                   → "Unweighted"
 *   - std::optional<T>                      → recursive dbg(*v) or "nullopt"
 *   - Pointer types                         → recursive dbg(*v) or "null"
 *   - Types with __id_ (CinderVertex etc.)  → "id:<n>"
 *   - All other user-defined types          → "obj@<address>"
 *
 * Does not use operator<<, streams, or toString().
 * Never causes a compile-time error for unsupported types.
 *
 * @tparam T Any type used as VertexType or EdgeType.
 * @param  v Value to convert.
 * @return   A non-empty std::string suitable for log messages.
 */
template <typename T> inline std::string dbg(const T &v) {

  // Primitive / string / enum
  if constexpr (Traits::is_primitive_enum_or_string_v<T>) {
    if constexpr (std::is_same_v<T, std::string>) {
      return v;
    } else if constexpr (std::is_same_v<T, bool>) {
      // bool is arithmetic — emit "true"/"false" not "1"/"0".
      return v ? "true" : "false";
    } else if constexpr (std::is_enum_v<T>) {
      return std::to_string(static_cast<std::underlying_type_t<T>>(v));
    } else {
      return std::to_string(v);
    }
  }

  // Unweighted
  else if constexpr (Traits::is_unweighted_v<T>) {
    return "Unweighted";
  }

  // Optional
  else if constexpr (Traits::is_optional_edge_v<T>) {
    if (v.has_value()) {
      return dbg(*v);
    }
    return "nullopt";
  }

  // Pointer
  else if constexpr (Traits::is_pointer_edge_v<T>) {
    if (v != nullptr) {
      return dbg(*v);
    }
    return "null";
  }

  // Types with __id_ (CinderVertex, CinderEdge, user-defined structs
  // that satisfy VertexHasher's requirement) — prefer id over address.
  else if constexpr (has___id<T>::value) {
    return "id:" + std::to_string(v.__id_);
  }

  // Fallback for fully opaque user-defined types.
  else {
    return "obj@" + std::to_string(reinterpret_cast<std::uintptr_t>(&v));
  }
}

/**
 * @brief Helper to format a vertex for logging.
 */
template <typename V> inline std::string vertexStr(const V &vertex) {
  return "Vertex(" + dbg(vertex) + ")";
}

/**
 * @brief Helper to format an unweighted edge for logging.
 */
template <typename V> inline std::string edgeStr(const V &src, const V &dest) {
  return "Edge(" + dbg(src) + " -> " + dbg(dest) + ")";
}

/**
 * @brief Helper to format a weighted edge for logging.
 */
template <typename V, typename E>
inline std::string weightedEdgeStr(const V &src, const V &dest, const E &weight) {
  return "Edge(" + dbg(src) + " -[" + dbg(weight) + "]-> " + dbg(dest) + ")";
}

} // namespace CinderPeak
