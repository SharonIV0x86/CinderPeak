#pragma once

#include "StorageEngine/ErrorCodes.hpp"
#include <vector>

namespace CinderPeak {
namespace Algorithms {

template <typename VertexType> class TopoSortResult {
public:
  explicit TopoSortResult(PeakStatus status = PeakStatus::OK())
      : _status(std::move(status)) {}

  bool isOK() { return _status.isOK(); }

  std::vector<VertexType> order_;
  PeakStatus _status;
  bool has_cycle_ = false;
};

} // namespace Algorithms

} // namespace CinderPeak