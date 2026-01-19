#include "PerlinNoise.hpp"
#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <algorithm>
#include <random>

namespace Math {

    PerlinNoise::PerlinNoise(uint32_t seed) {
        // Initialize permutation table
        m_permutation_table.resize(TABLE_SIZE * 2);

        // Fill with values 0-255
        for (int i = 0; i < TABLE_SIZE; ++i) {
            m_permutation_table[i] = static_cast<uint8_t>(i);
        }

        // Shuffle using seeded random number generator
        std::mt19937 rng(seed);
        std::shuffle(m_permutation_table.begin(), m_permutation_table.begin() + TABLE_SIZE, rng);

        // Duplicate the permutation table for wrapping
        for (int i = 0; i < TABLE_SIZE; ++i) {
            m_permutation_table[TABLE_SIZE + i] = m_permutation_table[i];
        }
    }

    float PerlinNoise::Fade(float t) {
        // Smootherstep interpolation (6t^5 - 15t^4 + 10t^3)
        return t * t * t * (t * (t * 6.0F - 15.0F) + 10.0F);
    }

    float PerlinNoise::Lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    float PerlinNoise::Gradient(int hash, float x, float y) {
        // Select gradient vector based on hash
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 8 ? y : x;

        // Apply different signs based on hash
        float result = ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
        return result;
    }

    float PerlinNoise::SingleOctave(glm::vec2 position) const {
        // Get integer and fractional parts
        int xi = static_cast<int>(glm::floor(position.x)) & (TABLE_SIZE - 1);
        int yi = static_cast<int>(glm::floor(position.y)) & (TABLE_SIZE - 1);

        float xf = position.x - glm::floor(position.x);
        float yf = position.y - glm::floor(position.y);

        // Fade curves for smooth transitions
        float u = Fade(xf);
        float v = Fade(yf);

        // Hash coordinates of the four corners
        int p00 = m_permutation_table[m_permutation_table[xi] + yi];
        int p10 = m_permutation_table[m_permutation_table[xi + 1] + yi];
        int p01 = m_permutation_table[m_permutation_table[xi] + yi + 1];
        int p11 = m_permutation_table[m_permutation_table[xi + 1] + yi + 1];

        // Calculate gradient values at four corners
        float g00 = Gradient(p00, xf, yf);
        float g10 = Gradient(p10, xf - 1.0F, yf);
        float g01 = Gradient(p01, xf, yf - 1.0F);
        float g11 = Gradient(p11, xf - 1.0F, yf - 1.0F);

        // Interpolate between corners
        float nx0 = Lerp(u, g00, g10);
        float nx1 = Lerp(u, g01, g11);
        float result = Lerp(v, nx0, nx1);

        // Normalize to approximately [0, 1]
        return glm::clamp((result + 1.0F) * 0.5F, 0.0F, 1.0F);
    }

    float PerlinNoise::Sample(glm::vec2 position) const {
        return SingleOctave(position);
    }

    float PerlinNoise::Fbm(glm::vec2 position, int octaves, float persistence, float lacunarity) const {
        float result = 0.0F;
        float amplitude = 1.0F;
        float frequency = 1.0F;
        float max_value = 0.0F;

        for (int i = 0; i < octaves; ++i) {
            result += SingleOctave(position * frequency) * amplitude;
            max_value += amplitude;

            amplitude *= persistence;
            frequency *= lacunarity;
        }

        return result / max_value;
    }

    float PerlinNoise::Ridge(glm::vec2 position, int octaves, float persistence, float lacunarity) const {
        float result = 0.0F;
        float amplitude = 1.0F;
        float frequency = 1.0F;
        float max_value = 0.0F;

        for (int i = 0; i < octaves; ++i) {
            float sample = SingleOctave(position * frequency);
            // Create ridge by inverting and scaling
            float ridge = 1.0F - glm::abs(2.0F * sample - 1.0F);

            result += ridge * amplitude;
            max_value += amplitude;

            amplitude *= persistence;
            frequency *= lacunarity;
        }

        return result / max_value;
    }

    std::vector<uint8_t> PerlinNoise::GeneratePixelBuffer(uint32_t width, uint32_t height, bool use_ridge,
                                                           float scale, int octaves) const {
        std::vector<uint8_t> buffer(width * height * 4);

        for (uint32_t y = 0; y < height; ++y) {
            for (uint32_t x = 0; x < width; ++x) {
                glm::vec2 position(x * scale, y * scale);

                float noise_value = use_ridge ? Ridge(position, octaves) : Fbm(position, octaves);

                // Convert to 0-255 range
                uint8_t pixel_value = static_cast<uint8_t>(glm::clamp(noise_value * 255.0F, 0.0F, 255.0F));

                // Calculate buffer index (RGBA format)
                size_t index = (y * width + x) * 4;

                // Set RGBA values (grayscale from noise value)
                buffer[index + 0] = pixel_value;      // Red
                buffer[index + 1] = pixel_value;      // Green
                buffer[index + 2] = pixel_value;      // Blue
                buffer[index + 3] = 255;              // Alpha (opaque)
            }
        }

        return buffer;
    }

}
