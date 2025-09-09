#pragma once
#include "InteractiveNode.hpp"
#include "InteractiveEdge.hpp"
#include "VisualizationTypes.hpp"
#include "../StorageEngine/AdjacencyList.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

namespace CinderPeak::Visualization {

/**
 * @brief Main visualization engine for interactive graph rendering using SFML
 * 
 * The VisualizationEngine provides a complete interactive graph visualization system
 * with support for draggable nodes, dynamic edges, and real-time rendering.
 * 
 * Features:
 * - Interactive node selection and dragging
 * - Dynamic edge updates when nodes move
 * - Event-driven architecture with mouse interaction
 * - Modular design for easy extension
 * 
 * @author CinderPeak Contributors
 * @version 1.0
 */
template <typename VertexType = int, typename EdgeType = float>
class VisualizationEngine {
public:
    using GraphType = CinderPeak::PeakStore::AdjacencyList<VertexType, EdgeType>;
    
    /**
     * @brief Constructs a new VisualizationEngine with a graph
     * 
     * Initializes an 800x600 SFML window with 60 FPS limit for smooth rendering.
     * Creates a visualization of the provided graph.
     * 
     * @param graph Reference to the graph to visualize
     */
    explicit VisualizationEngine(GraphType& graph) 
        : window_(sf::VideoMode(800, 600), "Graph Visualization"),
          graph_(&graph),
          selectedNode_(nullptr) {
        window_.setFramerateLimit(60);
        createVisualizationFromGraph();
    }
    
    /**
     * @brief Constructs a new VisualizationEngine with default window settings
     * 
     * Initializes an 800x600 SFML window with 60 FPS limit for smooth rendering.
     * Creates an empty visualization that can be populated later.
     */
    VisualizationEngine() 
        : window_(sf::VideoMode(800, 600), "Graph Visualization"),
          selectedNode_(nullptr) {
        window_.setFramerateLimit(60);
    }
    
    /**
     * @brief Sets the graph to visualize
     * 
     * @param graph Reference to the graph to visualize
     */
    void setGraph(GraphType& graph) {
        graph_ = &graph;
        createVisualizationFromGraph();
    }

    /**
     * @brief Main execution loop for the visualization engine
     * 
     * Creates a sample graph with 3 nodes and 2 edges for demonstration.
     * Runs the main event loop handling user input, updates, and rendering
     * until the window is closed.
     * 
     * The demo graph structure:
     * - Node 1 at (200, 200)
     * - Node 2 at (400, 300) 
     * - Node 3 at (600, 200)
     * - Edge from Node 1 to Node 2
     * - Edge from Node 2 to Node 3
     */
    /**
     * @brief Main execution loop for the visualization engine
     * 
     * Runs the main event loop handling user input, updates, and rendering
     * until the window is closed.
     */
    void run() {
        while (window_.isOpen()) {
            processEvents();
            update();
            render();
        }
    }
    
private:
    /**
     * @brief Processes all pending SFML window events
     * 
     * Handles mouse clicks, drags, releases, and window close events.
     * Implements the interaction logic for node selection and dragging.
     */
    void processEvents() {
        sf::Event event;
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handleMouseClick(window_.mapPixelToCoords(
                        sf::Mouse::getPosition(window_)));
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (selectedNode_) {
                        selectedNode_->setBeingDragged(false);
                        selectedNode_ = nullptr;
                    }
                }
            } else if (event.type == sf::Event::MouseMoved) {
                handleMouseMove(window_.mapPixelToCoords(
                    sf::Mouse::getPosition(window_)));
            }
        }
    }

    void handleMouseClick(const sf::Vector2f& mousePos) {
       
        for (auto& node : nodes_) {
            node->setSelected(false);
        }

   
        for (auto& node : nodes_) {
            if (node->contains(mousePos)) {
                node->setSelected(true);
                selectedNode_ = node.get();
                selectedNode_->setBeingDragged(true);
                dragOffset_ = selectedNode_->getPosition() - mousePos;
                break;
            }
        }
    }

    void handleMouseMove(const sf::Vector2f& mousePos) {
        if (selectedNode_ && selectedNode_->isBeingDragged()) {
            selectedNode_->setPosition(mousePos + dragOffset_);
        }
    }

    /**
     * @brief Updates the visualization state
     * 
     * Updates all edges to ensure they follow their connected nodes
     */
    void update() {
        for (auto& edge : edges_) {
            edge->update();
        }
    }

    void render() {
        window_.clear(sf::Color::White);
        
       
        for (const auto& edge : edges_) {
            window_.draw(*edge);
        }
        
  
        for (const auto& node : nodes_) {
            window_.draw(*node);
        }
        
        window_.display();
    }

    /**
     * @brief Creates visual elements from the current graph data
     */
    void createVisualizationFromGraph() {
        if (!graph_) return;
        
        // Clear existing visualization
        nodes_.clear();
        edges_.clear();
        
        // Get the adjacency list
        auto adjList = graph_->getAdjList();
        
        // Create a map to store vertex to Node mapping
        std::unordered_map<VertexType, InteractiveNode*, std::hash<VertexType>> vertexToNode;
        
        // First pass: Create nodes
        float angle = 0;
        const float angleStep = 2 * 3.14159f / adjList.size();
        const sf::Vector2f center(400, 300);
        const float radius = 200.0f;
        
        for (const auto& [vertex, _] : adjList) {
            // Position nodes in a circle
            sf::Vector2f position(
                center.x + radius * std::cos(angle),
                center.y + radius * std::sin(angle)
            );
            angle += angleStep;
            
            // Create a node and store the mapping
            nodes_.emplace_back(std::make_unique<InteractiveNode>(vertex, position));
            vertexToNode[vertex] = nodes_.back().get();
        }
        
        // Second pass: Create edges
        for (const auto& [srcVertex, neighbors] : adjList) {
            auto srcIt = vertexToNode.find(srcVertex);
            if (srcIt == vertexToNode.end()) continue;
            
            for (const auto& [destVertex, weight] : neighbors) {
                auto destIt = vertexToNode.find(destVertex);
                if (destIt != vertexToNode.end()) {
                    edges_.emplace_back(std::make_unique<InteractiveEdge>(
                        *(srcIt->second), *(destIt->second), weight
                    ));
                }
            }
        }
    }
    
    /**
     * @brief Updates the visualization to reflect any changes in the graph
     */
    void updateVisualization() {
        createVisualizationFromGraph();
    }
    
    sf::RenderWindow window_;
    GraphType* graph_ = nullptr;
    std::vector<std::unique_ptr<InteractiveNode>> nodes_;
    std::vector<std::unique_ptr<InteractiveEdge>> edges_;
    InteractiveNode* selectedNode_;
    sf::Vector2f dragOffset_;
    
    // Hash function for custom vertex types
    template<typename T>
    struct VertexHasher {
        size_t operator()(const T& vertex) const {
            // Use std::hash for primitive types
            if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, std::string>) {
                return std::hash<T>{}(vertex);
            } else {
                // For custom types, use the __id_ field if it exists
                return std::hash<decltype(vertex.__id_)>{}(vertex.__id_);
            }
        }
    };
};

}
