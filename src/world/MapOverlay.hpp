#pragma once

#include <vector>

namespace World {

    class World;

    //! The type of overlay
    enum class OverlayType : uint8_t {
        PLATE_TECTONICS = 0,
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
    };
};