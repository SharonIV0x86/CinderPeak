#include "Visualization/VisualizationEngine.hpp"
#include <memory>
#include <iostream>

int main() {
    try {
        CinderPeak::Visualization::VisualizationEngine engine;
        engine.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
