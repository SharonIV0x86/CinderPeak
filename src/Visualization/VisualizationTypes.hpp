#pragma once
#include <SFML/Graphics.hpp>

namespace CinderPeak {
namespace Visualization {

/**
 * @file VisualizationTypes.hpp
 * @brief Common type definitions and default constants for the visualization engine
 * 
 * This header provides shared types and visual constants used throughout
 * the CinderPeak visualization system to ensure consistency and maintainability.
 * 
 * @author CinderPeak Contributors
 * @version 1.0
 */

/// @brief Type alias for node identifiers
using NodeId = uint32_t;

/**
 * @namespace Defaults
 * @brief Default visual constants for the visualization engine
 * 
 * Contains predefined values for node and edge appearance to maintain
 * visual consistency across the visualization system.
 */
namespace Defaults {
    constexpr float NODE_RADIUS = 20.0f;
    const sf::Color NODE_COLOR{100, 149, 237};    
    const sf::Color NODE_SELECTED_COLOR{255, 99, 71}; 
    const sf::Color EDGE_COLOR{50, 50, 50};       
    const float EDGE_THICKNESS = 2.0f;
} 

} 
}
