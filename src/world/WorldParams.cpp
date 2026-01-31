#include "WorldParams.hpp"
#include "math/Hash.hpp"
#include <cstdint>

namespace World {

    void WorldParams::SetName(const std::string& name) {
        m_name = name;
    }

    const std::string& WorldParams::GetName() const {
        return m_name;
    }

    void WorldParams::SetSeedAscii(const std::string& seed) {
        m_seed_ascii = seed;

        // update the seed value to be the hash of the ascii seed
        m_seed = Math::HashFNV1A(m_seed_ascii);
    }

    const std::string& WorldParams::GetSeedAscii() const {
        return m_seed_ascii;
    }

    uint32_t WorldParams::GetSeed() const {
        return m_seed;
    }

    void WorldParams::SetDimension(size_t dimension) {
        m_dimmension = dimension;
    }

    size_t WorldParams::GetDimension() const {
        return m_dimmension;
    }

    glm::uvec2 WorldParams::GetWorldExtent() const {
        return {m_dimmension, m_dimmension};
    }

    void WorldParams::SetNumContinents(size_t numContinents) {
        m_num_continents = numContinents;
    }

    size_t WorldParams::GetNumContinents() const {
        return m_num_continents;
    }

    void WorldParams::SetPercentLand(float percentLand) {
        m_percent_land = percentLand;
    }

    float WorldParams::GetPercentLand() const {
        return m_percent_land;
    }

    void WorldParams::SetRegionSize(size_t regionSize) {
        m_region_size = regionSize;
    }

    size_t WorldParams::GetRegionSize() const {
        return m_region_size;
    }

    int32_t WorldParams::CalculateNumPlates() const {

        float numPlates = static_cast<float>(m_num_continents);
        numPlates *= 100.0F;
        numPlates /= m_percent_land;

        return static_cast<int32_t>(numPlates);
    }

    int32_t WorldParams::CalculateNumRegions() const {

        float numRegions = static_cast<float>(m_dimmension);
        numRegions *= numRegions;
        numRegions /= static_cast<float>(m_region_size);

        return static_cast<int32_t>(numRegions);
    }
}
