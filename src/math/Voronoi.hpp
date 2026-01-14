#pragma once

#include <glm/vec2.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Math {

    struct VoronoiGraph {

        public:
            std::vector<glm::vec2> m_centroids;
            std::vector<std::vector<int>> m_adjacency;
            glm::vec2 m_canvasSize{0.0F, 0.0F};

            //! Create pixel array from the graph to use for displaying on texture.
            //!
            //! This is grayscale image, where each pixel is encoded with its distance to assigned
            std::vector<uint8_t> ToPixels(glm::ivec2 resolution);

            //! Get the closest region to the given point.
            size_t GetRegion(glm::vec2 position);
    };

    /**
     * @brief Simple Voronoi generator and graph representation.
     */
    class VoronoiGenerator {
        public:


            static VoronoiGraph Generate(
                int regionCount,
                const glm::ivec2& canvasSize,
                int sampleResolution,
                uint32_t rngSeed);

        private:

            //! Find the closest seed to a point.
            static int GetClosestSeed(
                const std::vector<glm::vec2>& seeds,
                float posX,
                float posY);

            //! Find adjacency for each region
            static std::unordered_map<int, std::unordered_set<int>> CalculateAdjacency(
                const std::vector<int>& owner,
                int gridW,
                int gridH);
    };

}
