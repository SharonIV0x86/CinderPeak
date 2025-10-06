#pragma once
#include <iostream>
#include "CinderPeak.hpp" // or appropriate header with class/struct definitions

namespace CinderPeak {

inline std::ostream &operator<<(std::ostream &os, const CinderVertex &vertex) {
    os << vertex.__id_ << ' ' << vertex.__v___name;
    return os;
}

inline std::istream &operator>>(std::istream &is, CinderVertex &vertex) {
    is >> vertex.__id_ >> vertex.__v___name;
    return is;
}

inline std::ostream &operator<<(std::ostream &os, const CinderEdge &edge) {
    os << edge.__id_ << ' ' << edge.__e___name;
    return os;
}

inline std::istream &operator>>(std::istream &is, CinderEdge &edge) {
    is >> edge.__id_ >> edge.__e___name;
    return is;
}

} // namespace CinderPeak
