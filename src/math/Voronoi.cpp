/**
 * @file Voronoi.cpp
 * @brief Implementation of the VoronoiGenerator.
 */

#include "Voronoi.hpp"
#include "core/Engine.hpp"

#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <random>
#include <vector>
#include <unordered_set>
#include <limits>

namespace Math {


std::vector<uint8_t> VoronoiGraph::ToPixels(glm::ivec2 resolution) {

    if (m_centroids.empty()) {
        throw Core::EngineException("VoronoiGraph::ToPixels(): m_centroids is empty.");
    }

    std::vector<uint8_t> pixels;
    glm::vec2 pixelScale = m_canvasSize / glm::vec2(resolution);
    size_t numPixels = static_cast<size_t>(resolution.x)*static_cast<size_t>(resolution.y);
    pixels.reserve(numPixels * 4U);

    // owner for each pixel (region index)
    std::vector<size_t> owner(static_cast<size_t>(resolution.x) * static_cast<size_t>(resolution.y), -1);

    // assign ownership per pixel
    for (int pixelY = 0; pixelY < resolution.y; ++pixelY) {
        for (int pixelX = 0; pixelX < resolution.x; ++pixelX) {
            const float worldX = (static_cast<float>(pixelX) + 0.5F) * pixelScale.x;
            const float worldY = (static_cast<float>(pixelY) + 0.5F) * pixelScale.y;

            owner[(pixelY * resolution.x) + pixelX] = GetRegion({worldX, worldY});
        }
    }

    // compute grayscale based on distance to centroid and draw boundary lines where neighbor owners differ
    pixels.resize(numPixels * 4U);
    const float maxDist = glm::length(m_canvasSize);

    for (int pixelY = 0; pixelY < resolution.y; ++pixelY) {
        for (int pixelX = 0; pixelX < resolution.x; ++pixelX) {
            size_t idx = (static_cast<size_t>(pixelY) * static_cast<size_t>(resolution.x)) + static_cast<size_t>(pixelX);
            size_t regionId = owner[idx];

            // compute world pos
            const float worldX = (static_cast<float>(pixelX) + 0.5F) * pixelScale.x;
            const float worldY = (static_cast<float>(pixelY) + 0.5F) * pixelScale.y;

            float distanceX = worldX - m_centroids[regionId].x;
            float distanceY = worldY - m_centroids[regionId].y;
            float dist = std::sqrt((distanceX*distanceX) + (distanceY*distanceY));

            // normalized inverse distance -> brighter near centroid
            float norm = 1.0F - std::min(dist / std::max(1e-6F, maxDist), 1.0F); // NOLINT
            uint8_t gray = static_cast<uint8_t>(std::round(norm * 255.0F)); // NOLINT

            // check 4-neighborhood for boundary, if neighbor region differs mark as black
            bool isBoundary = false;
            if (((pixelX > 0) && (owner[idx - 1] != regionId)) ||
                ((pixelX + 1 < resolution.x) && (owner[idx + 1] != regionId)) ||
                ((pixelY > 0) && (owner[idx - resolution.x] != regionId)) ||
                ((pixelY + 1 < resolution.y) && (owner[idx + resolution.x] != regionId))) {
                isBoundary = true;
            }

            uint8_t color = isBoundary ? 0 : gray;
            uint8_t a = 255; // NOLINT set color transparent transparent

            size_t pixelOffset = idx * 4U;
            pixels[pixelOffset + 0] = color;
            pixels[pixelOffset + 1] = color;
            pixels[pixelOffset + 2] = color;
            pixels[pixelOffset + 3] = color;
        }
    }
    return pixels;
}


size_t VoronoiGraph::GetRegion(glm::vec2 position) {

    if (m_centroids.empty()) {
        throw Core::EngineException("VoronoiGraph::GetRegion(): m_centroids is empty.");
    }

    float bestDist = std::numeric_limits<float>::max();
    size_t bestIdx = -1;
    for (size_t i = 0; i < m_centroids.size(); ++i) {
        float distanceX = position.x - m_centroids[i].x;
        float distanceY = position.y - m_centroids[i].y;
        float distanceSumSquare = (distanceX*distanceX) + (distanceY*distanceY);
        if (distanceSumSquare < bestDist) {
            bestDist = distanceSumSquare;
            bestIdx = i;
        }
    }

    return bestIdx;
}

VoronoiGraph VoronoiGenerator::Generate(
    int regionCount,
    const glm::ivec2& canvasSize,
    int sampleResolution,
    uint32_t rngSeed) {

    VoronoiGraph out;

    if (regionCount <= 0) {
        return out;
    }

    std::mt19937 rng;
    if (rngSeed == 0U) {
        std::random_device random_device;
        rng.seed(random_device());
    }
    else {
        rng.seed(rngSeed);
    }

    std::uniform_real_distribution<float> uniform_dist_x(0.0F, static_cast<float>(canvasSize.x));
    std::uniform_real_distribution<float> uniform_dist_y(0.0F, static_cast<float>(canvasSize.y));

    std::vector<glm::vec2> seeds;
    seeds.reserve(static_cast<size_t>(regionCount));
    for (int i = 0; i < regionCount; ++i) {
        seeds.push_back({uniform_dist_x(rng), uniform_dist_y(rng)});
    }

    // divide the canvas into a grid with the given resolution.
    const int gridW = sampleResolution;
    const int gridH = sampleResolution;

    // for each cell, this variable tracks which region owns the cell.
    std::vector<int> owner(static_cast<size_t>(gridW) * static_cast<size_t>(gridH), -1);

    // define the scale of each cell in grid
    const float gridScaleX = static_cast<float>(canvasSize.x) / static_cast<float>(gridW);
    const float gridScaleY = static_cast<float>(canvasSize.y) / static_cast<float>(gridH);

    for (int gridY = 0; gridY < gridH; ++gridY) {
        for (int gridX = 0; gridX < gridW; ++gridX) {

            // get the center of the cell
            const float gridCenterX = (static_cast<float>(gridX) + 0.5F) * gridScaleX;
            const float gridCenterY = (static_cast<float>(gridY) + 0.5F) * gridScaleY;

            // find the closest seed to the center of the cell and assign ownership to grid.
            owner[(gridY * gridW) + gridX] = GetClosestSeed(seeds, gridCenterX, gridCenterY);
        }
    }

    std::unordered_map<int, std::unordered_set<int>> adjacencySet = CalculateAdjacency(owner, gridW, gridH);

    out.m_centroids = std::move(seeds);
    out.m_adjacency.resize(out.m_centroids.size());

    for (auto &keyValue : adjacencySet) {
        int regionId = keyValue.first;
        const auto &neighborSet = keyValue.second;
        std::vector<int> neighbors;
        neighbors.reserve(neighborSet.size());
        for (int neighborIdx : neighborSet) {
            neighbors.push_back(neighborIdx);
        }
        out.m_adjacency[regionId] = std::move(neighbors);
    }

    out.m_canvasSize = canvasSize;

    return out;
}

int VoronoiGenerator::GetClosestSeed(const std::vector<glm::vec2>& seeds, float posX, float posY) {

    float bestDist = std::numeric_limits<float>::max();
    int bestIdx = -1;
    for (int si = 0; si < static_cast<int>(seeds.size()); ++si) {

        float distanceX = posX - seeds[si].x;
        float distanceY = posY - seeds[si].y;
        float distanceSumSquare = (distanceX*distanceX) + (distanceY*distanceY);

        // note: we skip calculating the square root, since comparison of sum square will yield the same result.
        if (distanceSumSquare < bestDist) {
            bestDist = distanceSumSquare;
            bestIdx = si;
        }
    }

    return bestIdx;
}

std::unordered_map<int, std::unordered_set<int>> VoronoiGenerator::CalculateAdjacency(const std::vector<int>& owner, int gridW, int gridH) {

    std::unordered_map<int, std::unordered_set<int>> adjacencySet;
    for (int gridY = 0; gridY < gridH; ++gridY) {
        for (int gridX = 0; gridX < gridW; ++gridX) {

            // Get the ID of the region assigned to the cell
            int idx = (gridY * gridW) + gridX;
            int gridOwnerA = owner[idx];
            if (gridOwnerA < 0) {
                throw Core::EngineException("Error, failed to assign cell to region.");
            }

            // Check ownership of neighboring cells.  Only check neighbors to right of or to south of the cell, since
            // adjacency will have already been determined for left and north.
            if (gridX + 1 < gridW) {
                int gridOwnerB = owner[(gridY * gridW) + (gridX+1)];
                if (gridOwnerB >= 0 && gridOwnerB != gridOwnerA) {
                    adjacencySet[gridOwnerA].insert(gridOwnerB);
                    adjacencySet[gridOwnerB].insert(gridOwnerA);
                }
            }
            if (gridY + 1 < gridH) {
                int gridOwnerB = owner[((gridY+1) * gridW) + gridX];
                if (gridOwnerB >= 0 && gridOwnerB != gridOwnerA) {
                    adjacencySet[gridOwnerA].insert(gridOwnerB);
                    adjacencySet[gridOwnerB].insert(gridOwnerA);
                }
            }
        }
    }

    return adjacencySet;
}

} // namespace Math

