// Temporarily Depreciated. Unused.

#pragma once
#include "ArialFontDataEmbed.hpp"
// #include <SFML/Graphics.hpp>  // Commented out - SFML not available in CI
#include <cmath> // for std::cos, std::sin
#include <iostream>
#include <optional>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <vector>
namespace CinderPeak {
// GraphVisualizer disabled due to SFML dependency not available in CI
#ifdef SFML_AVAILABLE
template <typename VertexType, typename EdgeType> class GraphVisualizer {
public:
  using AdjListType =
      std::unordered_map<VertexType,
                         std::vector<std::pair<VertexType, EdgeType>>,
                         VertexHasher<VertexType>>;

  explicit GraphVisualizer(AdjListType adj_list) : _adj_list(adj_list) {
    LOG_DEBUG("GOT ADJACENCY LIST WITH SIZE: ");
    LOG_DEBUG(std::to_string(_adj_list.size()));
  }

  void visualize_primitives_graph() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Graph Visualization");
    window.setFramerateLimit(60);

    std::unordered_map<VertexType, sf::Vector2f> vertex_positions;
    const float center_x = 400.f;
    const float center_y = 300.f;
    const float radius = 200.f;

    std::cout << "Adjacency list size: " << _adj_list.size() << std::endl;

    if (_adj_list.size() == 0) {
      std::cerr << "Empty adjacency list!" << std::endl;
      return;
    }

    float angle_step = 2 * 3.14159265f / _adj_list.size();
    float current_angle = 0.f;

    int vertex_count = 0;
    for (const auto &[vertex, _] : _adj_list) {
      float x = center_x + radius * std::cos(current_angle);
      float y = center_y + radius * std::sin(current_angle);
      vertex_positions[vertex] = sf::Vector2f(x, y);

      std::cout << "Vertex " << vertex << " positioned at (" << x << ", " << y
                << ")" << std::endl;

      current_angle += angle_step;
      vertex_count++;
    }

    std::cout << "Total vertices positioned: " << vertex_count << std::endl;

    sf::Font font;
    if (!font.openFromMemory(Arial_ttf, Arial_ttf_len)) {
      std::cerr << "Failed to load font from memory" << std::endl;
      return;
    }

    std::unordered_map<VertexType, sf::Color> vertex_colors;
    const std::vector<sf::Color> color_palette = {
        sf::Color(102, 197, 204), sf::Color(246, 207, 113),
        sf::Color(248, 156, 116), sf::Color(220, 176, 242),
        sf::Color(135, 197, 125), sf::Color(158, 185, 243),
        sf::Color(254, 136, 177)};
    int color_index = 0;
    for (const auto &[vertex, _] : _adj_list) {
      vertex_colors[vertex] = color_palette[color_index % color_palette.size()];
      color_index++;
    }

    while (window.isOpen()) {
      while (std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
          window.close();
        }
      }

      window.clear(sf::Color(240, 240, 240));

      for (const auto &[src_vertex, edges] : _adj_list) {
        sf::Vector2f src_pos = vertex_positions[src_vertex];
        for (const auto &[dest_vertex, edge_weight] : edges) {
          if (vertex_positions.find(dest_vertex) == vertex_positions.end()) {
            std::cerr << "Warning: Destination vertex " << dest_vertex
                      << " not found in positions!" << std::endl;
            continue;
          }

          sf::Vector2f dest_pos = vertex_positions[dest_vertex];
          sf::Vector2f direction = dest_pos - src_pos;
          float length =
              std::sqrt(direction.x * direction.x + direction.y * direction.y);
          sf::Vector2f unit_direction = direction / length;

          sf::Vector2f adjusted_src = src_pos + unit_direction * 20.f;
          sf::Vector2f adjusted_dest = dest_pos - unit_direction * 20.f;

          const float control_offset = length * 0.2f;
          sf::Vector2f control_point1 =
              src_pos + sf::Vector2f(-unit_direction.y, unit_direction.x) *
                            control_offset;
          sf::Vector2f control_point2 =
              dest_pos + sf::Vector2f(-unit_direction.y, unit_direction.x) *
                             control_offset;

          sf::VertexArray curve(sf::PrimitiveType::LineStrip);
          const int segments = 20;
          for (int i = 0; i <= segments; ++i) {
            float t = static_cast<float>(i) / segments;
            float u = 1.f - t;
            float w1 = u * u * u;
            float w2 = 3.f * u * u * t;
            float w3 = 3.f * u * t * t;
            float w4 = t * t * t;

            sf::Vector2f point = w1 * adjusted_src + w2 * control_point1 +
                                 w3 * control_point2 + w4 * adjusted_dest;
            curve.append(sf::Vertex{
                point, sf::Color(100, 100, 100, 180)}); // Semi-transparent gray
          }
          window.draw(curve);

          // // Draw arrow head
          // if (length > 40.f)
          // {
          //   sf::Vector2f arrow_head = adjusted_dest;
          //   sf::Vector2f arrow_left = arrow_head - unit_direction * 10.f +
          //   sf::Vector2f(-unit_direction.y, unit_direction.x) * 5.f;
          //   sf::Vector2f arrow_right = arrow_head - unit_direction * 10.f +
          //   sf::Vector2f(unit_direction.y, -unit_direction.x) * 5.f;

          //   sf::VertexArray arrow(sf::PrimitiveType::Triangles, 3);
          //   arrow[0].position = arrow_head;
          //   arrow[1].position = arrow_left;
          //   arrow[2].position = arrow_right;
          //   arrow[0].color = arrow[1].color = arrow[2].color = sf::Color(100,
          //   100, 100, 180); window.draw(arrow);
          // }

          sf::Vector2f mid_point =
              (adjusted_src + adjusted_dest) / 2.f +
              sf::Vector2f(-unit_direction.y, unit_direction.x) *
                  (control_offset * 0.7f);
          sf::Text weight_text{font, "", 16};
          weight_text.setFillColor(sf::Color(70, 70, 70));

          if constexpr (std::is_same_v<EdgeType, char>) {
            weight_text.setString(std::string(1, edge_weight));
          } else {
            weight_text.setString(std::to_string(edge_weight));
          }

          sf::FloatRect text_bounds = weight_text.getLocalBounds();
          weight_text.setOrigin(
              sf::Vector2f{text_bounds.position.x + text_bounds.size.x / 2.f,
                           text_bounds.position.y + text_bounds.size.y / 2.f});

          weight_text.setPosition(mid_point);

          sf::RectangleShape text_bg(
              sf::Vector2f(text_bounds.size.x + 8.f, text_bounds.size.y + 4.f));
          // sf::RectangleShape text_bg(sf::Vector2f(text_bounds.size.x + 8.f,
          // text_bounds.size.y + 4.f));
          text_bg.setFillColor(sf::Color(240, 240, 240, 220));
          text_bg.setOrigin(sf::Vector2f(text_bg.getSize().x / 2.f,
                                         text_bg.getSize().y / 2.f));
          text_bg.setPosition(mid_point);
          window.draw(text_bg);

          window.draw(weight_text);
        }
      }

      int drawn_vertices = 0;
      for (const auto &[vertex, pos] : vertex_positions) {
        const float radius = 22.f;
        sf::CircleShape vertex_circle(radius);
        vertex_circle.setFillColor(vertex_colors[vertex]);
        vertex_circle.setOutlineThickness(2.f);
        vertex_circle.setOutlineColor(sf::Color(80, 80, 80));
        vertex_circle.setOrigin(sf::Vector2f(radius, radius));
        vertex_circle.setPosition(pos);

        sf::CircleShape highlight(radius * 0.7f);
        highlight.setFillColor(sf::Color(255, 255, 255, 60));
        highlight.setOrigin(
            sf::Vector2f(highlight.getRadius(), highlight.getRadius()));
        highlight.setPosition(
            sf::Vector2f{pos.x - radius * 0.15f, pos.y - radius * 0.15f});

        window.draw(vertex_circle);
        window.draw(highlight);

        sf::Text vertex_text{font, "", 20};
        vertex_text.setFillColor(sf::Color(30, 30, 30));
        if constexpr (std::is_same_v<VertexType, char>) {
          vertex_text.setString(std::string(1, vertex));
        } else {
          vertex_text.setString(std::to_string(vertex));
        }

        sf::FloatRect text_bounds = vertex_text.getLocalBounds();
        vertex_text.setOrigin(
            sf::Vector2f{text_bounds.position.x + text_bounds.size.x / 2.f,
                         text_bounds.position.y + text_bounds.size.y / 2.f});
        vertex_text.setPosition(pos);
        window.draw(vertex_text);

        drawn_vertices++;
      }

      static bool first_frame = true;
      if (first_frame) {
        std::cout << "Drew " << drawn_vertices << " vertices on screen"
                  << std::endl;
        first_frame = false;
      }

      window.display();
    }
  }
  void visualize() {
    sf::RenderWindow window(sf::VideoMode({1200, 800}), "Graph Visualization");

    while (window.isOpen()) {
      while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
          window.close();
      }

      window.clear(sf::Color::White);
      window.display();
    }
  }
  void print_adj_list() {
    for (const auto &[first, second] : _adj_list) {
      std::cout << "Vertex: " << first << "'s adj list:\n";
      for (const auto &pr : second) {
        std::cout << "  Neighbor: " << pr.first << " Weight: " << pr.second
                  << "\n";
      }
    }
  }

private:
  AdjListType _adj_list;
};
#endif // SFML_AVAILABLE

} // namespace CinderPeak
