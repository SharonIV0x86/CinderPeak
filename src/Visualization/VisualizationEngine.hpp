#pragma once
#include "InteractiveNode.hpp"
#include "InteractiveEdge.hpp"
#include "VisualizationTypes.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

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
class VisualizationEngine {
public:
    /**
     * @brief Constructs a new VisualizationEngine with default window settings
     * 
     * Initializes an 800x600 SFML window with 60 FPS limit for smooth rendering.
     * Sets up the rendering context and prepares for graph visualization.
     */
    VisualizationEngine() 
        : window_(sf::VideoMode(800, 600), "Graph Visualization"),
          selectedNode_(nullptr) {
        window_.setFramerateLimit(60);
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
    void run() {
      
        nodes_.emplace_back(std::make_unique<InteractiveNode>(1, sf::Vector2f(200, 200)));
        nodes_.emplace_back(std::make_unique<InteractiveNode>(2, sf::Vector2f(400, 300)));
        nodes_.emplace_back(std::make_unique<InteractiveNode>(3, sf::Vector2f(600, 200)));

    
        edges_.emplace_back(std::make_unique<InteractiveEdge>(*nodes_[0], *nodes_[1]));
        edges_.emplace_back(std::make_unique<InteractiveEdge>(*nodes_[1], *nodes_[2]));

      
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

    sf::RenderWindow window_;
    std::vector<std::unique_ptr<InteractiveNode>> nodes_;
    std::vector<std::unique_ptr<InteractiveEdge>> edges_;
    InteractiveNode* selectedNode_;
    sf::Vector2f dragOffset_;
};

}
