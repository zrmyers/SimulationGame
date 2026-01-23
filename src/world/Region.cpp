#include "Region.hpp"
#include "World.hpp"
#include "world/TectonicPlate.hpp"

namespace World {

    Region::Region(World& world, glm::vec2 centroid, std::vector<RegionId_t>&& neighbors)
        : m_p_world(&world), m_centroid(centroid), m_neighbors(std::move(neighbors)), m_plate_id(INVALID_PLATE_ID) {
    }

    void Region::SetPlateId(PlateId_t plate_id) {
        m_plate_id = plate_id;
    }

    PlateId_t Region::GetPlateId() const {
        return m_plate_id;
    }

    glm::vec2 Region::GetCentroid() const {
        return m_centroid;
    }

    const std::vector<RegionId_t>& Region::GetNeighbors() const {
        return m_neighbors;
    }

    void Region::SetIsBoundary(bool is_boundary) {
        m_is_boundary = is_boundary;
    }

    bool Region::GetIsBoundary() const {
        return m_is_boundary;
    }

    std::pair<PlateBoundaryType,PlateId_t> Region::GetPlateBoundaryType() const {
        PlateBoundaryType boundaryType = PlateBoundaryType::NONE;
        PlateId_t foundId = INVALID_PLATE_ID;

        if (m_is_boundary) {
            TectonicPlate& plate = m_p_world->GetPlate(m_plate_id);

            for (RegionId_t neighborID : m_neighbors) {

                Region& neighbor = m_p_world->GetRegion(neighborID);
                PlateId_t neighborPlateId = neighbor.GetPlateId();

                if (neighborPlateId != m_plate_id) {

                    // Get boundary type
                    PlateBoundaryType neighborBoundaryType = plate.GetBoundaryType(neighborPlateId);

                    if (static_cast<uint8_t>(neighborBoundaryType) > static_cast<uint8_t>(boundaryType)) {
                        boundaryType = neighborBoundaryType;
                        foundId = neighborPlateId;
                    }
                }
            }
        }

        return {boundaryType, foundId};
    }

    void Region::SetHasSubduction(bool has_subduction) {
        m_has_subduction = has_subduction;
    }

    bool Region::GetHasSubduction() const {
        return m_has_subduction;
    }

    void Region::SetAbsoluteHeight(float abs_height) {
        m_height = abs_height;
    }

    float Region::GetAbsoluteHeight() const {
        return m_height;
    }

    void Region::SetIsOcean(bool is_ocean) {
        m_is_ocean = is_ocean;
    }

    bool Region::GetIsOcean() const {
        return m_is_ocean;
    }

    void Region::SetIsWater(bool is_water) {
        m_is_water = is_water;
    }

    bool Region::GetIsWater() const {
        return m_is_water;
    }

    void Region::SetIsLake(bool is_lake) {
        m_is_lake = is_lake;
    }

    bool Region::GetIsLake() const {
        return m_is_lake;
    }

    void Region::SetWaterLevel(float water_level) {
        m_water_level = water_level;
    }

    float Region::GetWaterLevel() const {
        return m_water_level;
    }

    void Region::SetFlowAccumulation(float accumulation) {
        m_flow_accumulation = accumulation;
    }

    float Region::GetFlowAccumulation() const {
        return m_flow_accumulation;
    }

    void Region::SetFlowDirection(RegionId_t direction) {
        m_flow_direction = direction;
    }

    RegionId_t Region::GetFlowDirection() const {
        return m_flow_direction;
    }

    void Region::SetHasRiver(bool has_river) {
        m_has_river = has_river;
    }

    bool Region::GetHasRiver() const {
        return m_has_river;
    }

    void Region::SetTemperature(float temperature) {
        m_temperature = temperature;
    }

    float Region::GetTemperature() const {
        return m_temperature;
    }

    void Region::SetMoisture(float moisture) {
        m_moisture = moisture;
    }

    float Region::GetMoisture() const {
        return m_moisture;
    }

}
