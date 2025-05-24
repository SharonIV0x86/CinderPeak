#pragma once
#include "ArialFontDataEmbed.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <cmath> // for std::cos, std::sin
namespace CinderPeak
{
  template <typename VertexType, typename EdgeType>
  class GraphVisualizer
  {
  public:
    using AdjListType = std::unordered_map<VertexType, std::vector<std::pair<VertexType, EdgeType>>, VertexHasher<VertexType>>;

    explicit GraphVisualizer(AdjListType adj_list): _adj_list(adj_list)
    {
      LOG_DEBUG("GOT ADJACENCY LIST WITH SIZE: ");
      LOG_DEBUG(std::to_string(_adj_list.size()));


    }

    void visualize_primitives_graph()
    {
      sf::RenderWindow window(sf::VideoMode({800, 600}), "Graph Visualization");
      window.setFramerateLimit(60);

      std::unordered_map<VertexType, sf::Vector2f> vertex_positions;
      const float center_x = 400.f;
      const float center_y = 300.f;
      const float radius = 200.f;

      std::cout << "Adjacency list size: " << _adj_list.size() << std::endl;

      if (_adj_list.size() == 0)
      {
        std::cerr << "Empty adjacency list!" << std::endl;
        return;
      }

      float angle_step = 2 * 3.14159265f / _adj_list.size();
      float current_angle = 0.f;

      int vertex_count = 0;
      for (const auto &[vertex, _] : _adj_list)
      {
        float x = center_x + radius * std::cos(current_angle);
        float y = center_y + radius * std::sin(current_angle);
        vertex_positions[vertex] = sf::Vector2f(x, y);

        std::cout << "Vertex " << vertex << " positioned at (" << x << ", " << y << ")" << std::endl;

        current_angle += angle_step;
        vertex_count++;
      }

      std::cout << "Total vertices positioned: " << vertex_count << std::endl;

      sf::Font font;
      if (!font.openFromMemory(Arial_ttf, Arial_ttf_len))
      {
        std::cerr << "Failed to load font from memory" << std::endl;
        return;
      }

      while (window.isOpen())
      {
        while (std::optional<sf::Event> event = window.pollEvent())
        {
          if (event->is<sf::Event::Closed>())
          {
            window.close();
          }
        }

        window.clear(sf::Color::White);

        for (const auto &[src_vertex, edges] : _adj_list)
        {
          sf::Vector2f src_pos = vertex_positions[src_vertex];
          for (const auto &[dest_vertex, edge_weight] : edges)
          {
            if (vertex_positions.find(dest_vertex) == vertex_positions.end())
            {
              std::cerr << "Warning: Destination vertex " << dest_vertex << " not found in positions!" << std::endl;
              continue;
            }

            sf::Vector2f dest_pos = vertex_positions[dest_vertex];

            sf::Vertex line[] = {
                sf::Vertex{src_pos, sf::Color::Black},
                sf::Vertex{dest_pos, sf::Color::Black}};
            window.draw(line, 2, sf::PrimitiveType::Lines);

            sf::Vector2f mid_point = (src_pos + dest_pos) / 2.f;
            sf::Text weight_text{font, "", 14};
            weight_text.setFillColor(sf::Color::Blue);

            if constexpr (std::is_same_v<EdgeType, char>)
            {
              weight_text.setString(std::string(1, edge_weight));
            }
            else
            {
              weight_text.setString(std::to_string(edge_weight));
            }
            weight_text.setPosition(mid_point);
            window.draw(weight_text);
          }
        }

        int drawn_vertices = 0;
        for (const auto &[vertex, pos] : vertex_positions)
        {
          sf::CircleShape vertex_circle(20.f);
          vertex_circle.setFillColor(sf::Color::Green);
          vertex_circle.setOutlineThickness(2.f);
          vertex_circle.setOutlineColor(sf::Color::Black);
          vertex_circle.setOrigin(sf::Vector2f(20.f, 20.f));
          vertex_circle.setPosition(pos);
          window.draw(vertex_circle);

          sf::Text vertex_text{font, "", 20};
          vertex_text.setFillColor(sf::Color::Black);

          if constexpr (std::is_same_v<VertexType, char>)
          {
            vertex_text.setString(std::string(1, vertex));
          }
          else
          {
            vertex_text.setString(std::to_string(vertex));
          }
          sf::FloatRect text_bounds = vertex_text.getLocalBounds();
          vertex_text.setOrigin(sf::Vector2f(
              text_bounds.size.x / 2.f,
              text_bounds.size.y / 2.f));
          vertex_text.setPosition(pos);
          window.draw(vertex_text);

          drawn_vertices++;
        }

        static bool first_frame = true;
        if (first_frame)
        {
          std::cout << "Drew " << drawn_vertices << " vertices on screen" << std::endl;
          first_frame = false;
        }

        window.display();
      }
    }
    void visualize()
    {
      sf::RenderWindow window(sf::VideoMode({1200, 800}), "Graph Visualization");

      while (window.isOpen())
      {
        while (auto event = window.pollEvent())
        {
          if (event->is<sf::Event::Closed>())
            window.close();
        }

        window.clear(sf::Color::White);
        window.display();
      }
    }
    void print_adj_list()
    {
      for (const auto &[first, second] : _adj_list)
      {
        std::cout << "Vertex: " << first << "'s adj list:\n";
        for (const auto &pr : second)
        {
          std::cout << "  Neighbor: " << pr.first << " Weight: " << pr.second << "\n";
        }
      }
    }

  private:
    AdjListType _adj_list;
  };

} // namespace CinderPeak
