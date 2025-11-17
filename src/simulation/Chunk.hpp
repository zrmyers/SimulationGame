#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

//! This class represents the high level types that are used to run the simulation.
namespace Simulation {

    //! Type of blocks in simulation.
    enum class BlockType : uint8_t {
        AIR = 0U,
        ROCK
    };

    //! CHUNK Size, total size of chunk in each direction.
    static const constexpr uint16_t CHUNK_WIDTH = 16; // 4 bits
    static const constexpr uint16_t CHUNK_LENGTH = 16; // 4 bits
    static const constexpr uint16_t CHUNK_HEIGHT = 256; // 8 bits

    //! Number of blocks in a chunk.
    static const constexpr uint32_t NUM_BLOCKS = CHUNK_WIDTH*CHUNK_LENGTH*CHUNK_HEIGHT;

    //! Get the X coordinate from a block ID
    static constexpr uint16_t X_FROM_BLOCK_ID(uint16_t block_id) {
        return ((block_id >> 12) & 0x0F); // NOLINT bits 12-15 of ID are x coordinate.
    }

    //! Get the Z coordinate from a block ID
    static constexpr uint16_t Z_FROM_BLOCK_ID(uint16_t block_id) {
        return ((block_id >> 8) & 0x0F); // NOLINT bits 8-11 of ID are z coordiante.
    }

    //! Get the Y coordinate from a block ID
    static constexpr uint16_t Y_FROM_BLOCK_ID(uint16_t block_id) {
        return (block_id & 0xFFU); // NOLINT bits 0-7 of ID are y coordinate.
    }

    //! Make a block ID.
    static constexpr uint16_t BLOCK_ID_FROM_PARTS(uint16_t x_coord, uint16_t z_coord, uint16_t y_coord) {
        return ((x_coord & 0x0F) << 12) | ((z_coord & 0x0F) << 8) | (y_coord & 0xFFU); // NOLINT
    }

    //! The chunk represents a part of the world that is currently being simulated.
    //!
    //! The chunk is composed of cells that contain stacks of objects, including terrain.
    //!
    //! Additionally groups of cells can be occupied by objects that take more space than the cell.
    //!
    //! Each block is 1x1x1 m in size. Following is how coordinates are interpreted:
    //!    -X == West
    //!    +X == East
    //!    -Z == North
    //!    +Z == South
    //!    -Y == Down
    //!    +Z == Up
    class Chunk {

        public:

            //! @brief Constructor for a chunk
            Chunk(uint32_t dimmension);

            //! @brief Get block.
            //!
            //! @param[in] x_coord Coordinate on X axis. [0, CHUNK_WIDTH)
            //! @param[in] z_coord Coordinate on Z axis. [0, CHUNK_LENGTH)
            //! @param[in] y_coord Coordinate on Y axis. [0, CHUNK_HEIGHT)
            //!
            //! @returns The type of block at the position.
            BlockType GetBlock(uint16_t x_coord, uint16_t z_coord, uint16_t y_coord);

            //! @brief Set block
            //!
            //! @param[in] x_coord Coordinate on X axis. [0, CHUNK_WIDTH)
            //! @param[in] z_coord Coordinate on Z axis. [0, CHUNK_LENGTH)
            //! @param[in] y_coord Coordinate on Y axis. [0, CHUNK_HEIGHT)
            //! @param[in] type    The type of block at the position.
            void SetBlock(uint16_t x_coord, uint16_t z_coord, uint16_t y_coord, BlockType type);

        private:

            //! The array of cells that make up the chunk. This contains data that is used for representing terrain features.
            //!
            //! Consider using RLE encoding for storage.
            std::array<BlockType, NUM_BLOCKS> m_blocks;
    };

};