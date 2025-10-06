#pragma once
#include "Concepts.hpp"
#include "PeakStore.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <optional>
#include <tuple>
#include <utility>
#include <fstream>
#include <sstream>
#include "Serialization.hpp" // Make sure this exists in your src/

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}

template <typename VertexType, typename EdgeType> class CinderGraph;

template <typename VertexType, typename EdgeType> class CinderGraphRowProxy {
    CinderGraph<VertexType, EdgeType> &graph;
    VertexType src;

public:
    CinderGraphRowProxy(CinderGraph<VertexType, EdgeType> &g, const VertexType &s)
        : graph(g), src(s) {}

    EdgeType operator[](const VertexType &dest) const {
        auto [optWeight, found] = graph.getEdge(src, dest);
        if (!found || !optWeight.has_value()) {
            throw std::runtime_error("Edge not found");
        }
        return *optWeight;
    }
    CinderGraphRowProxy &operator=(const EdgeType &newWeight) = delete;

    struct EdgeAssignProxy {
        CinderGraph<VertexType, EdgeType> &graph;
        VertexType src, dest;

        EdgeAssignProxy(CinderGraph<VertexType, EdgeType> &g, const VertexType &s,
                        const VertexType &d)
            : graph(g), src(s), dest(d) {}

        EdgeAssignProxy &operator=(const EdgeType &weight) {
            graph.addEdge(src, dest, weight);
            return *this;
        }

        operator EdgeType() const {
            auto [optWeight, found] = graph.getEdge(src, dest);
            return (found && optWeight) ? *optWeight : EdgeType{};
        }
    };

    EdgeAssignProxy operator[](const VertexType &dest) {
        return EdgeAssignProxy(graph, src, dest);
    }
};

template <typename VertexType, typename EdgeType>
class CinderGraph {
    std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>> peak_store;

    using EdgeKey = std::pair<VertexType, VertexType>;
    using WeightedEdgeKey = std::tuple<VertexType, VertexType, EdgeType>;
    using VertexAddResult = std::pair<VertexType, bool>;
    using UnweightedEdgeAddResult = std::pair<EdgeKey, bool>;
    using WeightedEdgeAddResult = std::pair<WeightedEdgeKey, bool>;
    using UpdateEdgeResult = std::pair<EdgeType, bool>;
    using GetEdgeResult = std::pair<std::optional<EdgeType>, bool>;
    using RemoveEdgeResult = std::pair<std::optional<EdgeType>, bool>;

public:
    CinderGraph(const GraphCreationOptions &options =
                    GraphCreationOptions::getDefaultCreateOptions(),
                const PolicyConfiguration &cfg = PolicyConfiguration()) {
        PeakStore::GraphInternalMetadata metadata(
            "cinder_graph", Traits::isTypePrimitive<VertexType>(),
            Traits::isTypePrimitive<EdgeType>(),
            Traits::isGraphWeighted<EdgeType>(),
            !Traits::isGraphWeighted<EdgeType>());
        peak_store = std::make_unique<PeakStore::PeakStore<VertexType, EdgeType>>(
            metadata, options, cfg);
    }
    VertexAddResult addVertex(const VertexType &v) {
        auto resp = peak_store->addVertex(v);
        if (!resp.isOK()) {
            Exceptions::handle_exception_map(resp);
            return {v, false};
        }
        return {v, true};
    }
    bool removeVertex(const VertexType &v) {
        auto resp = peak_store->removeVertex(v);
        if (!resp.isOK()) {
            Exceptions::handle_exception_map(resp);
            return false;
        }
        return true;
    }
    RemoveEdgeResult removeEdge(const VertexType &src, const VertexType &dest) {
        auto [data, status] = peak_store->removeEdge(src, dest);
        if (!status.isOK()) {
            Exceptions::handle_exception_map(status);
            return {std::nullopt, false};
        }
        return {std::make_optional(data), true};
    }
    void clearEdges() {
        auto resp = peak_store->clearEdges();
        if (!resp.isOK()) {
            Exceptions::handle_exception_map(resp);
            return;
        }
    }
    bool hasVertex(const VertexType &v) { return peak_store->hasVertex(v); }

    template <typename E = EdgeType>
    auto addEdge(const VertexType &src, const VertexType &dest)
        -> std::enable_if_t<CinderPeak::Traits::is_unweighted_v<E>,
                            UnweightedEdgeAddResult> {
        auto resp = peak_store->addEdge(src, dest);
        if (!resp.isOK()) {
            Exceptions::handle_exception_map(resp);
            return {{src, dest}, false};
        }
        return {{src, dest}, true};
    }
    template <typename E = EdgeType>
    auto addEdge(const VertexType &src, const VertexType &dest, const EdgeType &weight)
        -> std::enable_if_t<!CinderPeak::Traits::is_unweighted_v<E>,
                            WeightedEdgeAddResult> {
        auto resp = peak_store->addEdge(src, dest, weight);
        if (!resp.isOK()) {
            Exceptions::handle_exception_map(resp);
            return {{src, dest, weight}, false};
        }
        return {{src, dest, weight}, true};
    }
    template <typename E = EdgeType>
    auto updateEdge(const VertexType &src, const VertexType &dest,
                    const EdgeType &newWeight)
        -> std::enable_if_t<CinderPeak::Traits::is_weighted_v<E>,
                            UpdateEdgeResult> {
        auto [status, updatedEdge] = peak_store->updateEdge(src, dest, newWeight);
        if (!status.isOK()) {
            Exceptions::handle_exception_map(status);
            return {newWeight, false};
        }
        return {newWeight, true};
    }
    GetEdgeResult getEdge(const VertexType &src, const VertexType &dest) {
        LOG_INFO("Called getEdge");
        auto [data, status] = peak_store->getEdge(src, dest);
        if (!status.isOK()) {
            Exceptions::handle_exception_map(status);
            return {std::nullopt, false};
        }
        return {std::make_optional(data), true};
    }

    std::string getGraphStatistics() { return peak_store->getGraphStatistics(); }
    size_t numEdges() const { return peak_store->numEdges(); }
    size_t numVertices() const { return peak_store->numVertices(); }

    static void setConsoleLogging(const bool toggle) {
        CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>::setConsoleLogging(toggle);
    }

    // New: Serialization support
    bool saveToFile(const std::string &filename) const {
        std::ofstream ofs(filename);
        if (!ofs.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return false;
        }
        try {
            // Example assumes adjacency storage is exposed by PeakStore
            const auto &adj_list = peak_store->getContext()->adjacency_storage->getAdjList();
            for (const auto &vertex_pair : adj_list) {
                ofs << "V " << vertex_pair.first << "\n";
            }
            for (const auto &vertex_pair : adj_list) {
                const VertexType &src = vertex_pair.first;
                const auto &neighbors = vertex_pair.second;
                for (const auto &neighbor : neighbors) {
                    ofs << "E " << src << " " << neighbor.first;
                    if constexpr (!Traits::is_unweighted_v<EdgeType>) {
                        ofs << " " << neighbor.second;
                    }
                    ofs << "\n";
                }
            }
            ofs.close();
            return true;
        } catch (const std::exception &e) {
            std::cerr << "Exception during saveToFile: " << e.what() << std::endl;
            return false;
        }
    }

    bool loadFromFile(const std::string &filename) {
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            std::cerr << "Failed to open file for reading: " << filename << std::endl;
            return false;
        }
        try {
            clearEdges();
            const auto &adj_list = peak_store->getContext()->adjacency_storage->getAdjList();
            std::vector<VertexType> vertices_to_remove;
            for (const auto &pair : adj_list) {
                vertices_to_remove.push_back(pair.first);
            }
            for (const auto &v : vertices_to_remove) {
                peak_store->removeVertex(v);
            }

            std::string line;
            while (std::getline(ifs, line)) {
                if (line.empty()) continue;
                std::istringstream iss(line);
                char type;
                iss >> type;
                if (type == 'V') {
                    VertexType v;
                    iss >> v;
                    addVertex(v);
                } else if (type == 'E') {
                    VertexType src, dest;
                    iss >> src >> dest;
                    if constexpr (Traits::is_unweighted_v<EdgeType>) {
                        addEdge(src, dest);
                    } else {
                        EdgeType weight;
                        iss >> weight;
                        addEdge(src, dest, weight);
                    }
                }
            }
            ifs.close();
            return true;
        } catch (const std::exception &e) {
            std::cerr << "Exception during loadFromFile: " << e.what() << std::endl;
            return false;
        }
    }

    // Proxy support
    CinderGraphRowProxy<VertexType, EdgeType> operator[](const VertexType &v) {
        return CinderGraphRowProxy<VertexType, EdgeType>(*this, v);
    }
    const CinderGraphRowProxy<VertexType, EdgeType>
    operator[](const VertexType &v) const {
        return CinderGraphRowProxy<VertexType, EdgeType>(
            const_cast<CinderGraph<VertexType, EdgeType> &>(*this), v);
    }
};

} // namespace CinderPeak
