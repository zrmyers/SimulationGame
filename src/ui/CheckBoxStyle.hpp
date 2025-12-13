#pragma once

#include "graphics/Font.hpp"
#include "NineSliceStyle.hpp"
#include "graphics/Texture2D.hpp"

namespace UI {

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

    class CheckBoxStyle {

        public:

            CheckBoxStyle();

            void SetImage(CheckBoxState state, std::shared_ptr<Graphics::Texture2D> p_style);
            const std::shared_ptr<Graphics::Texture2D>& GetImage(CheckBoxState state);

        private:

            std::array<std::shared_ptr<Graphics::Texture2D>, NUM_CHECKBOX_STATES> m_checkbox_images {};
    };
}