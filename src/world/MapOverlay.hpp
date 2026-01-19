#pragma once

#include <vector>

namespace World {

    class World;

    //! The type of overlay
    enum class OverlayType : uint8_t {
        PLATE_TECTONICS = 0,
        HEIGHT_MAP = 1
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
    };
};