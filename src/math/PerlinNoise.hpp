#pragma once

#include <glm/vec2.hpp>
#include <vector>
#include <cstdint>

namespace Math {

    //! Perlin noise generator for procedural terrain and feature generation
    //! Implements classic 2D Perlin noise with configurable octaves and persistence
    class PerlinNoise {

        public:
            //! Constructor
            //! @param[in] seed Random seed for permutation table generation
            explicit PerlinNoise(uint32_t seed = 0);

            //! Generate Perlin noise value at given position
            //! @param[in] position 2D position to sample noise at
            //! @returns Noise value in range [0.0, 1.0]
            float Sample(glm::vec2 position) const;

            //! Generate fractional Brownian motion (fBm) noise
            //! Combines multiple octaves of Perlin noise for more natural-looking results
            //! @param[in] position 2D position to sample noise at
            //! @param[in] octaves Number of noise octaves to combine
            //! @param[in] persistence Controls amplitude decrease per octave (0.0-1.0)
            //! @param[in] lacunarity Controls frequency increase per octave (typically 2.0)
            //! @returns Noise value in range [0.0, 1.0]
            float Fbm(glm::vec2 position, int octaves = 4, float persistence = 0.5F, float lacunarity = 2.0F) const;

            //! Generate ridge noise (creates ridge-like features)
            //! @param[in] position 2D position to sample noise at
            //! @param[in] octaves Number of noise octaves to combine
            //! @param[in] persistence Controls amplitude decrease per octave (0.0-1.0)
            //! @param[in] lacunarity Controls frequency increase per octave (typically 2.0)
            //! @returns Noise value in range [0.0, 1.0]
            float Ridge(glm::vec2 position, int octaves = 4, float persistence = 0.5F, float lacunarity = 2.0F) const;

            //! @brief Generate an RGBA pixel buffer visualization of Perlin noise
            //!
            //! Creates a 2D grid of RGBA pixels where each pixel's color is determined by the Perlin noise
            //! value at that location. The output is grayscale, with the noise intensity mapped to all
            //! color channels equally and alpha always set to opaque.
            //!
            //! @param[in] width Width of the output buffer in pixels
            //! @param[in] height Height of the output buffer in pixels
            //! @param[in] use_ridge If true, uses ridge noise for sharper features; if false, uses FBm
            //!                       noise for smoother, more natural-looking results. Default: false
            //! @param[in] scale Frequency scale factor applied to coordinates. Lower values (0.001-0.01)
            //!                  produce larger features, higher values produce finer detail. Default: 0.01F
            //! @param[in] octaves Number of noise octaves to combine. Higher values (4-8) create more
            //!                     complex, detailed results. Default: 4
            //!
            //! @returns std::vector<uint8_t> containing RGBA pixel data. The buffer size is
            //!          width * height * 4 bytes. Pixels are stored in row-major order (y increases first).
            //!          Each pixel is represented as 4 bytes: [R, G, B, A]
            //!
            //! @note The noise value is normalized to [0.0, 1.0] and converted to [0, 255] for pixel values.
            //!       All channels (R, G, B) receive the same value, creating grayscale output.
            std::vector<uint8_t> GeneratePixelBuffer(uint32_t width, uint32_t height, bool use_ridge = false,
                                                     float scale = 0.01F, int octaves = 4) const;

        private:
            static constexpr int TABLE_SIZE = 256;

            //! Permutation table for noise generation
            std::vector<uint8_t> m_permutation_table;

            //! Interpolation function for smooth gradients
            static float Fade(float t);

            //! Linear interpolation
            static float Lerp(float t, float a, float b);

            //! Pseudo-random gradient selection
            static float Gradient(int hash, float x, float y);

            //! Single octave of Perlin noise
            float SingleOctave(glm::vec2 position) const;
    };

}
