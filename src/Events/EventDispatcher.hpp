#pragma once

#include <functional>
#include <vector>

namespace CinderPeak {

template <typename Event> class EventDispatcher {
public:
  using Listener = std::function<void(const Event &)>;

  void subscribe(Listener listener) {
    listeners.push_back(std::move(listener));
  }

  void emit(const Event &event) {

    for (auto &listener : listeners) {
      listener(event);
    }
  }

private:
  std::vector<Listener> listeners;
};

} // namespace CinderPeak