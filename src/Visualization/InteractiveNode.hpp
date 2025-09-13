#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include "VisualizationTypes.hpp"

namespace CinderPeak::Visualization {

/**
 * @brief Interactive graph node with drag-and-drop functionality
 * 
 * Represents a single node in the graph visualization that can be selected,
 * dragged, and provides visual feedback for user interactions.
 * 
 * Features:
 * - Click detection and selection highlighting
 * - Smooth drag-and-drop interaction
 * - Visual state changes (normal, selected, dragging)
 * - Collision detection for mouse interaction
 * 
 * @author CinderPeak Contributors
 * @version 1.0
 */
class InteractiveNode : public sf::Drawable {
public:
    /**
     * @brief Constructs a new InteractiveNode at the specified position
     * 
     * @param id Unique identifier for this node
     * @param position Initial position in world coordinates
     * @throws std::invalid_argument if NODE_RADIUS is not positive
     * 
     * Initializes the node with default visual properties and validates
     * that the node radius is positive to prevent rendering issues.
     */
    InteractiveNode(NodeId id, const sf::Vector2f& position)
        : id_(id), isSelected_(false), isBeingDragged_(false) {
        if (Defaults::NODE_RADIUS <= 0.0f) {
            throw std::invalid_argument("Node radius must be positive");
        }
        
        shape_.setRadius(Defaults::NODE_RADIUS);
        shape_.setOrigin(Defaults::NODE_RADIUS, Defaults::NODE_RADIUS);
        shape_.setPosition(position);
        shape_.setFillColor(Defaults::NODE_COLOR);
        shape_.setOutlineThickness(2.0f);
        shape_.setOutlineColor(sf::Color::Black);
    }
    
    
    NodeId getId() const { return id_; }
    sf::Vector2f getPosition() const { return shape_.getPosition(); }
    bool isSelected() const { return isSelected_; }
    bool isBeingDragged() const { return isBeingDragged_; }
    
  
    void setPosition(const sf::Vector2f& position) { 
        shape_.setPosition(position); 
    }
    
    void setSelected(bool selected) {
        isSelected_ = selected;
        shape_.setFillColor(selected ? Defaults::NODE_SELECTED_COLOR : Defaults::NODE_COLOR);
    }

    void setBeingDragged(bool dragging) {
        isBeingDragged_ = dragging;
        shape_.setOutlineColor(dragging ? sf::Color::Red : sf::Color::Black);
    }
    
    
    bool contains(const sf::Vector2f& point) const {
        const float dx = point.x - shape_.getPosition().x;
        const float dy = point.y - shape_.getPosition().y;
        return (dx * dx + dy * dy) <= (Defaults::NODE_RADIUS * Defaults::NODE_RADIUS);
    }
    
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(shape_, states);
    }
    
    NodeId id_;
    sf::CircleShape shape_;
    bool isSelected_;
    bool isBeingDragged_;
};

}
