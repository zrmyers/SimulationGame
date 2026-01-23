#pragma once

#include "TectonicPlate.hpp"
#include <cstdint>
#include <functional>

namespace World {

    class World;
    class Region;

    using RegionId_t = int32_t;

    static constexpr RegionId_t INVALID_REGION_ID = -1;

    // A group of tiles that form a fundamental unit of land in the game. Determines biome and ownership for a range of
    // tiles.
    class Region {

        public:
            Region(World& world, glm::vec2 centroid, std::vector<RegionId_t>&& neighbors);

            void SetPlateId(PlateId_t plate_id);
            PlateId_t GetPlateId() const;

            glm::vec2 GetCentroid() const;

            //! Get the neighbors of this region
            const std::vector<RegionId_t>& GetNeighbors() const;

            void SetIsBoundary(bool is_boundary);
            bool GetIsBoundary() const;

            std::pair<PlateBoundaryType, PlateId_t> GetPlateBoundaryType() const;

            // check for subduction
            void SetHasSubduction(bool has_subduction);
            bool GetHasSubduction() const;

            // Set the absolute height, relative to magma level.
            void SetAbsoluteHeight(float height);
            float GetAbsoluteHeight() const;

            // Water properties
            void SetIsOcean(bool is_ocean);
            bool GetIsOcean() const;

            void SetIsWater(bool is_water);
            bool GetIsWater() const;

            void SetIsLake(bool is_lake);
            bool GetIsLake() const;

            void SetWaterLevel(float water_level);
            float GetWaterLevel() const;

            void SetFlowAccumulation(float accumulation);
            float GetFlowAccumulation() const;

            void SetFlowDirection(RegionId_t direction);
            RegionId_t GetFlowDirection() const;

            void SetHasRiver(bool has_river);
            bool GetHasRiver() const;

            // Climate properties
            void SetTemperature(float temperature);
            float GetTemperature() const;

            void SetMoisture(float moisture);
            float GetMoisture() const;

        private:

            //! Reference to world that region belongs to.
            World* m_p_world {nullptr};

            //! The point that represents the center of the region.
            glm::vec2 m_centroid {0.0F, 0.0F};

            //! The list of neighbors for the region.
            std::vector<RegionId_t> m_neighbors;

            //! The ID of the plate associated with region.
            PlateId_t m_plate_id {INVALID_PLATE_ID};

            //! The absolute height of the region
            float m_height {0.0F};

            //! Whether the region is at plate boundary.
            bool m_is_boundary {false};

            //! Whether the region is part of subduction zone.
            bool m_has_subduction {false};

            //! Whether the region is ocean (below ocean level)
            bool m_is_ocean {false};

            //! Whether the region contains water
            bool m_is_water {false};

            //! Whether the region is a lake
            bool m_is_lake {false};

            //! Water level of the region (for oceans and lakes)
            float m_water_level {0.0F};

            //! Flow accumulation from upstream regions
            float m_flow_accumulation {1.0F};

            //! Direction of water flow (to which region ID)
            RegionId_t m_flow_direction {INVALID_REGION_ID};

            //! Whether the region contains river(s)
            bool m_has_river {false};

            //! Temperature of the region in degrees Celsius
            float m_temperature {0.0F};

            //! Moisture level of the region (0-100)
            float m_moisture {0.0F};
    };
}