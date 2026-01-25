#pragma once

#include <vector>

namespace World {

    class World;

    //! The type of overlay
    enum class OverlayType : uint8_t {
        PLATE_TECTONICS = 0,
        HEIGHT_MAP = 1,
        WATER_MAP = 2,
        HEAT_MAP = 3,
        MOISTURE_MAP = 4,
        BIOME_MAP = 5,
    };

    class MapOverlay {

        public:

            static std::vector<uint8_t> GetOverlay(World& world, OverlayType overlayType);

        private:

            //! Returns buffer of pixels where:
            //! - black maps to pixels on edge of regions
            //! - red maps to pixels within regions on transform plate boundaries
            //! - green maps to pixels within regions on convergent plate boundaries
            //! - blue maps to pixels within regions on divergent plate boundaries
            //! - white maps to pixels within regions that are not on plate boundaries
            //!
            //! Alpha channel is set to opaque.
            static std::vector<uint8_t> GetPlateTectonicsOverlay(World& world);

            //! Returns a greyscale buffer of pixels, where rgb channels are used to indicate height. Height is normalized
            //! between black and white, where black is lowest elevation, and white is highest elevation.
            static std::vector<uint8_t> GetHeightMapOverlay(World& world);

            //! Returns a colored buffer of pixels representing water and land features:
            //! - Dark blue for ocean water
            //! - Light blue for rivers
            //! - Cyan for lakes
            //! - Green for land
            //!
            //! Alpha channel is set to opaque.
            static std::vector<uint8_t> GetWaterMapOverlay(World& world);

            //! Returns a colored buffer of pixels where temperature values are interpolated between blue and red colors.
            //! Temperatures greater than 0 Celsius are red.
            //! Temperatures less than 0 Celsius are blue.
            static std::vector<uint8_t> GetHeatMapOverlay(World& world);

            //! Returns a colored buffer of pixels where moisture values are represented in grayscale.
            //! Moisture of 0 is black.
            //! Moisture of 100 is white.
            static std::vector<uint8_t> GetMoistureOverlay(World& world);

            //! Returns a colored buffer of pixels where biome values are represented as colors
            static std::vector<uint8_t> GetBiomeOverlay(World& world);
    };
};