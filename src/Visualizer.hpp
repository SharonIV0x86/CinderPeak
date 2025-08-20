#pragma once
#include "ArialFontDataEmbed.hpp"
#include <type_traits>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>

namespace CinderPeak {

template <typename VertexType, typename EdgeType>
class InteractiveVisualizer {
public:
    using AdjListType =
        std::unordered_map<VertexType,
                        std::vector<std::pair<VertexType, EdgeType>>>;

    explicit InteractiveVisualizer(AdjListType adj_list)
        : _adj_list(adj_list) {}

    void visualize() {
        sf::RenderWindow window(sf::VideoMode({800, 600}), "Interactive Graph");
        window.setFramerateLimit(60);

        if (_adj_list.empty()) {
            std::cerr << "Empty adjacency list!" << std::endl;
            return;
        }

        std::unordered_map<VertexType, sf::Vector2f> vertex_positions;
        const float center_x = 400.f;
        const float center_y = 300.f;
        const float radius = 200.f;
        float angle_step = 2 * 3.14159265f / _adj_list.size();
        float current_angle = 0.f;

        for (const auto& [vertex, _] : _adj_list) {
            float x = center_x + radius * std::cos(current_angle);
            float y = center_y + radius * std::sin(current_angle);
            vertex_positions[vertex] = sf::Vector2f(x, y);
            current_angle += angle_step;
        }

        std::unordered_map<VertexType, sf::Color> vertex_colors;
        const std::vector<sf::Color> palette = {
            sf::Color(102, 197, 204), sf::Color(246, 207, 113),
            sf::Color(248, 156, 116), sf::Color(220, 176, 242),
            sf::Color(135, 197, 125), sf::Color(158, 185, 243),
            sf::Color(254, 136, 177)};

        int colorIndex = 0;
        for (const auto& [vertex, _] : _adj_list) {
            vertex_colors[vertex] = palette[colorIndex % palette.size()];
            colorIndex++;
        }

        sf::Font font;
        if (!font.loadFromMemory(Arial_ttf, Arial_ttf_len)) {
            std::cerr << "Failed to load font from memory" << std::endl;
            return;
        }

        std::optional<VertexType> selected_vertex = std::nullopt;
        sf::Vector2f mouse_offset;

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mouse_pos(event.mouseButton.x, event.mouseButton.y);
                    for (auto& [vertex, pos] : vertex_positions) {
                        float dist = std::hypot(pos.x - mouse_pos.x, pos.y - mouse_pos.y);
                        if (dist <= 22.f) {
                            selected_vertex = vertex;
                            mouse_offset = pos - mouse_pos;
                            break;
                        }
                    }
                }

                if (event.type == sf::Event::MouseButtonReleased &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    selected_vertex = std::nullopt;
                }
            }

            if (selected_vertex.has_value()) {
                sf::Vector2i mpos = sf::Mouse::getPosition(window);
                vertex_positions[*selected_vertex] = sf::Vector2f(mpos.x, mpos.y) + mouse_offset;
            }

            window.clear(sf::Color(240, 240, 240));

            for (const auto& [src, edges] : _adj_list) {
                sf::Vector2f src_pos = vertex_positions[src];
                for (const auto& [dest, edge_weight] : edges) {
                    if (vertex_positions.find(dest) == vertex_positions.end()) continue;
                    sf::Vector2f dest_pos = vertex_positions[dest];
                    sf::Vertex line[] = {
                        sf::Vertex(src_pos, sf::Color(100, 100, 100, 180)),
                        sf::Vertex(dest_pos, sf::Color(100, 100, 100, 180))
                    };
                    window.draw(line, 2, sf::Lines);
                }
            }

            for (const auto& [vertex, pos] : vertex_positions) {
                sf::CircleShape circle(22.f);
                circle.setFillColor(vertex_colors[vertex]);
                circle.setOutlineThickness(2.f);
                circle.setOutlineColor(sf::Color(80, 80, 80));
                circle.setOrigin(22.f, 22.f);
                circle.setPosition(pos);
                window.draw(circle);

                sf::Text label;
                label.setFont(font);
                label.setCharacterSize(20);
                label.setFillColor(sf::Color(30, 30, 30));
                if constexpr (std::is_same_v<VertexType, char>)
                    label.setString(std::string(1, vertex));
                else
                    label.setString(std::to_string(vertex));

                sf::FloatRect text_bounds = label.getLocalBounds();
                label.setOrigin(text_bounds.width / 2.f, text_bounds.height / 2.f);
                label.setPosition(pos);
                window.draw(label);
            }

            window.display();
        }
    }

  private:
      AdjListType _adj_list;
  };
} 
