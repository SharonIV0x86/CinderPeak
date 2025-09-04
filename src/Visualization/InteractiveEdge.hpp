#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "InteractiveNode.hpp"

namespace CinderPeak::Visualization {

/**
 * @brief Dynamic edge connecting two InteractiveNode instances
 * 
 * Represents a visual connection between two nodes that automatically
 * updates its position when the connected nodes are moved.
 * 
 * Features:
 * - Automatic position updates when nodes move
 * - Self-loop prevention with validation
 * - Efficient line rendering using SFML vertices
 * - Handles edge cases like overlapping nodes
 * 
 * @author CinderPeak Contributors
 * @version 1.0
 */
class InteractiveEdge : public sf::Drawable {
public:
    InteractiveEdge(const InteractiveNode& source, const InteractiveNode& target)
        : source_(source), target_(target) {
        if (&source_ == &target_) {
            throw std::invalid_argument("Cannot create edge from a node to itself");
        }
        line_[0].color = Defaults::EDGE_COLOR;
        line_[1].color = Defaults::EDGE_COLOR;
        update();
    }

    void update() {
        const auto& src = source_.getPosition();
        const auto& dst = target_.getPosition();
        
        
        if (std::abs(src.x - dst.x) < 1e-5f && std::abs(src.y - dst.y) < 1e-5f) {
          
            line_[0].position = src;
            line_[1].position = sf::Vector2f(dst.x + 0.1f, dst.y + 0.1f);
        } else {
            line_[0].position = src;
            line_[1].position = dst;
        }
    }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(line_, 2, sf::Lines, states);
    }

    const InteractiveNode& source_;
    const InteractiveNode& target_;
    sf::Vertex line_[2];
};

} 
