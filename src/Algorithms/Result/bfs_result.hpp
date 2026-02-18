#include "StorageEngine/ErrorCodes.hpp"
#include <vector>
#pragma once

namespace CinderPeak {
namespace Algorithms {
template <typename VertexType> class BFSResult {
public:
  explicit BFSResult(PeakStatus status = PeakStatus::OK())
      : _status(std::move(status)) {}

  bool isOK() { return _status.isOK(); }
  std::vector<VertexType> order_;
  PeakStatus _status;
};
} // namespace Algorithms

} // namespace CinderPeak
