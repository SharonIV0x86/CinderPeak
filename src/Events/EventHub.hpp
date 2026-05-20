#pragma once

#include "EventDispatcher.hpp"
#include "GraphEvents.hpp"

namespace CinderPeak {

template <typename V, typename E> struct EventHub {

  EventDispatcher<EdgeAddedEvent<V, E>> edgeAdded;

  EventDispatcher<EdgeRemovedEvent<V, E>> edgeRemoved;

  EventDispatcher<VertexAddedEvent<V>> vertexAdded;

  EventDispatcher<VertexRemovedEvent<V>> vertexRemoved;
};

} // namespace CinderPeak