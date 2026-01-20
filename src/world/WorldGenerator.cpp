/**
 * @file WorldGenerator.cpp
 * @brief WorldGenerator utilities (Voronoi region graph generation).
 */

#include "WorldGenerator.hpp"
#include "Region.hpp"
#include "TectonicPlate.hpp"
#include "World.hpp"
#include "WorldParams.hpp"
#include "passes/Passes.hpp"

namespace World {


std::unique_ptr<World> WorldGenerator::Generate(const WorldParams& params) {

    Extent_t worldExtent = params.GetWorldExtent();
    std::unique_ptr<World> p_world = std::make_unique<World>(worldExtent);

    Passes::RunTectonicsPass(*p_world, params);
    Passes::RunElevationPass(*p_world, params);
    Passes::RunHydrologyPass(*p_world, params);

    return p_world;
}



} // namespace World
