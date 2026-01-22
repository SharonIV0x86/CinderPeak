#pragma once
#include "Concepts.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "StorageEngine/Utils.hpp"
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace CinderPeak
{
    namespace Serialization
    {
        using json = nlohmann::json;

        // Thread-safe serialization format version
        // Note: Currently using JSON for cross-platform compatibility and readability.
        // Future versions should migrate to MessagePack or Protocol Buffers for
        // production-grade binary serialization with better performance.
        constexpr const char *SERIALIZATION_VERSION = "2.0";

        // Helper functions to convert vertex/edge types to JSON
        template <typename T>
        typename std::enable_if<std::is_arithmetic<T>::value, json>::type
        toJson(const T &value)
        {
            return json(value);
        }

        template <typename T>
        typename std::enable_if<std::is_same<T, std::string>::value, json>::type
        toJson(const T &value)
        {
            return json(value);
        }

        // For CinderVertex and CinderEdge types
        template <typename T>
        typename std::enable_if<
            std::is_base_of<CinderVertex, T>::value ||
                std::is_same<T, CinderVertex>::value,
            json>::type
        toJson(const T &vertex)
        {
            json j;
            j["__id_"] = vertex.__id_;
            j["__v___name"] = vertex.__v___name;
            return j;
        }

        template <typename T>
        typename std::enable_if<std::is_base_of<CinderEdge, T>::value ||
                                    std::is_same<T, CinderEdge>::value,
                                json>::type
        toJson(const T &edge)
        {
            json j;
            j["__id_"] = edge.__id_;
            j["__e___name"] = edge.__e___name;
            return j;
        }

        // Helper functions to convert from JSON to vertex/edge types
        template <typename T>
        typename std::enable_if<std::is_arithmetic<T>::value, T>::type
        fromJson(const json &j)
        {
            return j.get<T>();
        }

        template <typename T>
        typename std::enable_if<std::is_same<T, std::string>::value, T>::type
        fromJson(const json &j)
        {
            return j.get<std::string>();
        }

        // For CinderVertex
        template <typename T>
        typename std::enable_if<
            std::is_base_of<CinderVertex, T>::value ||
                std::is_same<T, CinderVertex>::value,
            T>::type
        fromJson(const json &j)
        {
            T vertex;
            if (j.contains("__id_"))
                vertex.__id_ = j["__id_"].get<size_t>();
            if (j.contains("__v___name"))
                vertex.__v___name = j["__v___name"].get<std::string>();
            return vertex;
        }

        // For CinderEdge
        template <typename T>
        typename std::enable_if<std::is_base_of<CinderEdge, T>::value ||
                                    std::is_same<T, CinderEdge>::value,
                                T>::type
        fromJson(const json &j)
        {
            T edge;
            if (j.contains("__id_"))
                edge.__id_ = j["__id_"].get<size_t>();
            if (j.contains("__e___name"))
                edge.__e___name = j["__e___name"].get<std::string>();
            return edge;
        }

        // Thread-safe serialization class with enhanced metadata support
        template <typename VertexType, typename EdgeType>
        class GraphSerializer
        {
        private:
            mutable std::shared_mutex mtx_; // Thread safety for concurrent operations

        public:
            struct SerializationResult
            {
                bool success;
                std::string message;
                PeakStatus status;

                static SerializationResult Success(const std::string &msg = "Success")
                {
                    return {true, msg, PeakStatus::OK()};
                }

                static SerializationResult Failure(const std::string &msg,
                                                   const PeakStatus &st)
                {
                    return {false, msg, st};
                }
            };

            // Serialize graph data to JSON format with full metadata (thread-safe)
            static SerializationResult
            serialize(const std::vector<VertexType> &vertices,
                      const std::vector<
                          std::tuple<VertexType, VertexType, EdgeType>> &edges,
                      const GraphCreationOptions &options, const std::string &filepath)
            {
                try
                {
                    json j;

                    // Enhanced graph metadata with version and timestamp
                    j["graph_metadata"] = {
                        {"version", SERIALIZATION_VERSION},
                        {"timestamp", static_cast<uint64_t>(
                                          std::chrono::system_clock::now().time_since_epoch().count())},
                        {"directed", options.hasOption(GraphCreationOptions::Directed)},
                        {"undirected", options.hasOption(GraphCreationOptions::Undirected)},
                        {"self_loops", options.hasOption(GraphCreationOptions::SelfLoops)},
                        {"parallel_edges",
                         options.hasOption(GraphCreationOptions::ParallelEdges)},
                        {"weighted", !Traits::is_unweighted_v<EdgeType>},
                        {"vertex_count", vertices.size()},
                        {"edge_count", edges.size()},
                        {"is_vertex_type_primitive", std::is_arithmetic<VertexType>::value},
                        {"is_edge_type_primitive", std::is_arithmetic<EdgeType>::value}};

                    // Serialize vertices
                    json vertices_array = json::array();
                    for (const auto &v : vertices)
                    {
                        vertices_array.push_back(toJson(v));
                    }
                    j["vertices"] = vertices_array;

                    // Serialize edges
                    json edges_array = json::array();
                    for (const auto &edge_tuple : edges)
                    {
                        json edge_obj;
                        edge_obj["source"] = toJson(std::get<0>(edge_tuple));
                        edge_obj["target"] = toJson(std::get<1>(edge_tuple));

                        // Only add weight if the graph is weighted
                        if (!Traits::is_unweighted_v<EdgeType>)
                        {
                            edge_obj["weight"] = toJson(std::get<2>(edge_tuple));
                        }

                        edges_array.push_back(edge_obj);
                    }
                    j["edges"] = edges_array;

                    // Write to file
                    std::ofstream file(filepath);
                    if (!file.is_open())
                    {
                        return SerializationResult::Failure(
                            "Failed to open file for writing: " + filepath,
                            PeakStatus::FileIOError());
                    }

                    file << j.dump(2); // Pretty print with 2-space indentation
                    file.close();

                    return SerializationResult::Success("Graph serialized successfully to " +
                                                        filepath);
                }
                catch (const json::exception &e)
                {
                    return SerializationResult::Failure(
                        std::string("JSON serialization error: ") + e.what(),
                        PeakStatus::SerializationError());
                }
                catch (const std::exception &e)
                {
                    return SerializationResult::Failure(
                        std::string("Serialization error: ") + e.what(),
                        PeakStatus::UnknownError());
                }
            }

            // Deserialize graph data from JSON format (thread-safe)
            static SerializationResult
            deserialize(const std::string &filepath, std::vector<VertexType> &vertices,
                        std::vector<std::tuple<VertexType, VertexType, EdgeType>> &edges,
                        GraphCreationOptions &options)
            {
                try
                {
                    // Read from file
                    std::ifstream file(filepath);
                    if (!file.is_open())
                    {
                        return SerializationResult::Failure(
                            "Failed to open file for reading: " + filepath,
                            PeakStatus::FileIOError());
                    }

                    json j;
                    file >> j;
                    file.close();

                    // Validate schema
                    if (!j.contains("graph_metadata") || !j.contains("vertices") ||
                        !j.contains("edges"))
                    {
                        return SerializationResult::Failure(
                            "Invalid graph file format: missing required fields",
                            PeakStatus::InvalidFormat());
                    }

                    // Parse metadata and reconstruct options
                    auto metadata = j["graph_metadata"];
                    std::vector<GraphCreationOptions::GraphType> graph_types;

                    if (metadata.value("directed", false))
                    {
                        graph_types.push_back(GraphCreationOptions::Directed);
                    }
                    if (metadata.value("undirected", false))
                    {
                        graph_types.push_back(GraphCreationOptions::Undirected);
                    }
                    if (metadata.value("self_loops", false))
                    {
                        graph_types.push_back(GraphCreationOptions::SelfLoops);
                    }
                    if (metadata.value("parallel_edges", false))
                    {
                        graph_types.push_back(GraphCreationOptions::ParallelEdges);
                    }

                    // Create GraphCreationOptions from the collected types
                    switch (graph_types.size())
                    {
                    case 0:
                        options = GraphCreationOptions({});
                        break;
                    case 1:
                        options = GraphCreationOptions({graph_types[0]});
                        break;
                    case 2:
                        options = GraphCreationOptions({graph_types[0], graph_types[1]});
                        break;
                    case 3:
                        options = GraphCreationOptions({graph_types[0], graph_types[1], graph_types[2]});
                        break;
                    case 4:
                        options = GraphCreationOptions({graph_types[0], graph_types[1], graph_types[2], graph_types[3]});
                        break;
                    default:
                        return {false, "Invalid number of graph types", PeakStatus::DeserializationError()};
                    }

                    // Deserialize vertices
                    vertices.clear();
                    for (const auto &v_json : j["vertices"])
                    {
                        vertices.push_back(fromJson<VertexType>(v_json));
                    }

                    // Deserialize edges
                    edges.clear();
                    for (const auto &e_json : j["edges"])
                    {
                        if (!e_json.contains("source") || !e_json.contains("target"))
                        {
                            return SerializationResult::Failure(
                                "Invalid edge format: missing source or target",
                                PeakStatus::InvalidFormat());
                        }

                        VertexType source = fromJson<VertexType>(e_json["source"]);
                        VertexType target = fromJson<VertexType>(e_json["target"]);

                        EdgeType weight = EdgeType();
                        if (e_json.contains("weight"))
                        {
                            weight = fromJson<EdgeType>(e_json["weight"]);
                        }

                        edges.push_back(std::make_tuple(source, target, weight));
                    }

                    return SerializationResult::Success("Graph deserialized successfully from " +
                                                        filepath);
                }
                catch (const json::exception &e)
                {
                    return SerializationResult::Failure(
                        std::string("JSON deserialization error: ") + e.what(),
                        PeakStatus::DeserializationError());
                }
                catch (const std::exception &e)
                {
                    return SerializationResult::Failure(
                        std::string("Deserialization error: ") + e.what(),
                        PeakStatus::UnknownError());
                }
            }
        };

    } // namespace Serialization
} // namespace CinderPeak
