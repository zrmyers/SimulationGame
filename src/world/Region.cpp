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

    PlateBoundaryType Region::GetPlateBoundaryType() const {
        PlateBoundaryType boundaryType = PlateBoundaryType::NONE;

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
                    }
                }
            }
        }

        return boundaryType;
    }
}
