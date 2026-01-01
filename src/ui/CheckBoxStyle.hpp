/**
 * @file CheckBoxStyle.hpp
 * @brief Styling container for checkbox textures per state.
 */

#pragma once

#include "graphics/Font.hpp"
#include "NineSliceStyle.hpp"
#include "graphics/Texture2D.hpp"

namespace UI {

    /**
     * @enum CheckBoxState
     * @brief All visual/interaction states used by checkboxes.
     */
    enum class CheckBoxState : int8_t {
        UNKNOWN = -1, // Check box state is unknown.
        ON, // Check box is on
        OFF, // Check box is off
        ON_FOCUSED, // Check box is on, with user focus
        OFF_FOCUSED, // Check box is off, with user focus
        ON_ACTIVATED, // User has pressed the check box
        OFF_ACTIVATED, // User has pressed the check box
    };

    static const constexpr size_t NUM_CHECKBOX_STATES = 6U;

    /**
     * @class CheckBoxStyle
     * @brief Holds per-state textures for checkboxes.
     */
    class CheckBoxStyle {

        public:

            CheckBoxStyle();

            /**
             * @brief Set the texture for a given checkbox state.
             * @param state The checkbox state to set the texture for.
             * @param p_style Shared pointer to the texture to use.
             */
            void SetImage(CheckBoxState state, std::shared_ptr<Graphics::Texture2D> p_style);
            /**
             * @brief Get the texture for a given checkbox state.
             * @param state The checkbox state to query.
             * @return Shared pointer to the texture associated with @p state.
             */
            const std::shared_ptr<Graphics::Texture2D>& GetImage(CheckBoxState state);

        private:

            std::array<std::shared_ptr<Graphics::Texture2D>, NUM_CHECKBOX_STATES> m_checkbox_images {};
    };
}