#pragma once
#include "ErrorCodes.hpp"
#include "CinderExceptions.hpp"
#include "PeakLogger.hpp"
#include <bitset>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <random>
#include <chrono>
#include <sstream>
#include <map>
#include <functional>
namespace CinderPeak
{
  class GraphCreationOptions
  {
  public:
    enum GraphType
    {
      Directed = 0,
      Weighted,
      SelfLoops,
      ParallelEdges,
      Undirected,
      Unweighted,
    };
    GraphCreationOptions(std::initializer_list<GraphType> graph_types)
    {
      for (auto type : graph_types)
      {
        options.set(type);
      }
    }

    bool hasOption(GraphType type) const { return options.test(type); }

  private:
    std::bitset<8> options;
  };
  template <typename T, typename Enable = void>
  struct VertexHasher;
  template <typename T, typename Enable = void>
  struct EdgeHasher;

  template <typename T>
  struct VertexHasher<
      T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T> ||
                          std::is_same_v<T, std::string>>>
  {
    std::size_t operator()(const T &v) const { return std::hash<T>{}(v); }
  };

  template <typename T>
  struct VertexHasher<T, std::enable_if_t<std::is_class_v<T> &&
                                          !std::is_same_v<T, std::string>>>
  {
    std::size_t operator()(const T &v) const
    {
      return std::hash<int>{}(v.__id_) ^
             (std::hash<std::string>{}(v.__v___name) << 1);
    }
  };
  template <typename T>
  struct EdgeHasher<
      T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T> ||
                          std::is_same_v<T, std::string>>>
  {
    std::size_t operator()(const T &v) const { return std::hash<T>{}(v); }
  };
  // pair hasher for future use.
  template <typename VertexType, typename EdgeType>
  struct PairHasher
  {
    std::size_t operator()(const std::pair<VertexType, EdgeType> &p) const
    {
      return VertexHasher<VertexType>{}(p.first) ^
             (EdgeHasher<EdgeType>{}(p.second) << 1);
    }
  };
  template <typename T>
  struct is_primitive_or_string : std::disjunction<
                                      std::is_arithmetic<T>,
                                      std::is_same<T, std::string>>
  {
  };
  template <typename T>
  inline constexpr bool is_primitive_or_string_v = is_primitive_or_string<T>::value;

  std::string __generate_vertex_name()
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const size_t name_length = 10;
    std::uniform_int_distribution<> dist(0, charset.size() - 1);
    std::stringstream ss;
    for (size_t i = 0; i < name_length; ++i)
    {
      ss << charset[dist(gen)];
    }
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    ss << "_" << duration;
    return ss.str();
  }
  template <typename T>
  bool isTypePrimitive()
  {
    if constexpr (is_primitive_or_string_v<T>)
    {
      return true;
    }
    return false;
  }
  class CinderVertex
  {
  public:
    size_t __id_;
    static size_t nextId;
    std::string __v___name;

    CinderVertex()
    {
      __id_ = nextId++;
      __v___name = __generate_vertex_name();
    }
    CinderVertex(std::string vertexName) : __v___name{vertexName} {};

    bool operator<(const CinderVertex &other) const
    {
      return __id_ < other.__id_;
    }
    bool operator==(const CinderVertex &other) const
    {
      return __id_ == other.__id_;
    }
    const std::string __to_vertex_string(){
      return __v___name;
    }
  };
  class CinderEdge
  {
  public:
    size_t __id_;
    static size_t nextId;
    std::string __e___name;
    CinderEdge()
    {
      __id_ = nextId++;
      __e___name = __generate_vertex_name();
    }
    CinderEdge(std::string edge_name) : __e___name{edge_name} {};

    bool operator<(const CinderEdge &other) const
    {
      return __id_ < other.__id_;
    }
    bool operator>(const CinderEdge &other) const
    {
      return __id_ > other.__id_;
    }
    bool operator==(const CinderEdge &other) const
    {
      return __id_ == other.__id_;
    }
    const std::string __to_edge_string(){
      return __e___name;
    } 
  };
  size_t CinderPeak::CinderVertex::nextId = 1;
  size_t CinderPeak::CinderEdge::nextId = 1;

  namespace PeakStore
  {
    class GraphInternalMetadata
    {
    public:
      size_t density;
      size_t num_vertices;
      size_t num_edges;
      size_t num_self_loops;
      size_t num_parallel_edges;
      const std::string graph_type;
      bool is_vertex_type_primitive;
      bool is_edge_type_primitive;
      GraphInternalMetadata(const std::string &graph_type, bool vertex_tp_p, bool edge_tp_p)
          : graph_type(graph_type), is_vertex_type_primitive(vertex_tp_p), is_edge_type_primitive(edge_tp_p)
      {
        num_vertices = 0;
        num_edges = 0;
        density = 0;
        num_self_loops = 0;
        num_parallel_edges = 0;
      }
      // default ctor for basic testing, this has to be removed later on.
      GraphInternalMetadata() {}
    };
  } // namespace PeakStore
  namespace Exceptions
  {
    inline void handle_exception_map(const PeakStatus &status)
    {
      switch (static_cast<int>(status.code()))
      {
      // not needed, not severe, breaks the program.
      //  case static_cast<int>(StatusCode::VERTEX_ALREADY_EXISTS):
      //     throw DuplicateVertexException("Vertex Already Exists");
      //     break;
      case static_cast<int>(StatusCode::NOT_FOUND):
        LOG_INFO("Resource Not Found");
        break;
      case static_cast<int>(StatusCode::UNIMPLEMENTED):
        LOG_WARNING("Called an Unimplemented method");
        break;
      case static_cast<int>(StatusCode::ALREADY_EXISTS):
        LOG_INFO("Resource Already Exists");
        break;
      case static_cast<int>(StatusCode::VERTEX_ALREADY_EXISTS):
        LOG_INFO("Vertex Already Exists");
        break;
      case static_cast<int>(StatusCode::VERTEX_NOT_FOUND):
        LOG_ERROR("Vertex does not exist");
        break;
      case static_cast<int>(StatusCode::EDGE_ALREADY_EXISTS):
        LOG_INFO("Edge Already Exists");
        break;
      default:
        LOG_CRITICAL("Unhandled Exception Occurred");
        break;
      }
    }
  }

} // namespace CinderPeak
