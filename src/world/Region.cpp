#include "Region.hpp"
#include "World.hpp"

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

}
