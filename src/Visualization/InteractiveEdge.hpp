#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <iostream>
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
    InteractiveEdge(const InteractiveNode& source, const InteractiveNode& target, float weight = 1.0f)
        : source_(source), target_(target), weight_(weight) {
        if (&source_ == &target_) {
            throw std::invalid_argument("Cannot create edge from a node to itself");
        }
        line_[0].color = Defaults::EDGE_COLOR;
        line_[1].color = Defaults::EDGE_COLOR;
        
        // Set up the text for weight display
        // Try to load a system font
        const std::vector<std::string> fontPaths = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
            "/usr/share/fonts/truetype/ubuntu/Ubuntu-Bold.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf"
        };
        
        bool fontLoaded = false;
        for (const auto& path : fontPaths) {
            if (font_.loadFromFile(path)) {
                fontLoaded = true;
                break;
            }
        }
        
        // If no system font was found, try to load the default font
        if (!fontLoaded) {
            if (!font_.loadFromFile("Arial.ttf")) {
                // If no font can be loaded, we'll just have to proceed without text
                std::cerr << "Warning: Could not load any font for edge weight display" << std::endl;
            }
        }
        weightText_.setFont(font_);
        weightText_.setCharacterSize(12);
        weightText_.setFillColor(sf::Color::Black);
        weightText_.setString(std::to_string(weight).substr(0, 4));
        
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
        
        // Draw weight text in the middle of the edge
        sf::Vector2f midPoint = {
            (line_[0].position.x + line_[1].position.x) / 2.0f,
            (line_[0].position.y + line_[1].position.y) / 2.0f
        };
        
        // Offset the text slightly for better visibility
        midPoint.x += 5.0f;
        midPoint.y += 5.0f;
        
        weightText_.setPosition(midPoint);
        target.draw(weightText_, states);
    }
    
    const InteractiveNode& source_;
    const InteractiveNode& target_;
    float weight_;
    sf::Vertex line_[2];
    sf::Font font_;
    mutable sf::Text weightText_;
};

} 
