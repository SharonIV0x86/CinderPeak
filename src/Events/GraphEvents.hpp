#pragma once

namespace CinderPeak {

template <typename V, typename E> struct EdgeAddedEvent {

  const V &src;
  const V &dest;
  const E &weight;
};

template <typename V, typename E> struct EdgeRemovedEvent {

  const V &src;
  const V &dest;
};

template <typename V> struct VertexAddedEvent { const V &vertex; };

template <typename V> struct VertexRemovedEvent { const V &vertex; };

} // namespace CinderPeak