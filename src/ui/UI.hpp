#pragma once

#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "sdl/SDL.hpp"
#include "sdl/TTF.hpp"
#include "graphics/Texture2D.hpp"
#include <SDL3/SDL_stdinc.h>
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace UI {

    enum class LayoutMode : uint8_t {
        RELATIVE_TO_PARENT = 0,
        FIT_TO_CHILDREN,
        FIXED
    };
    //! Element.
    class Element {

        public:
            Element() = default;
            Element(const Element& other) = delete;
            Element(Element&& other) = default;
            Element& operator=(const Element& other) = delete;
            Element& operator=(Element&& other) = default;
            virtual ~Element() = default;

            Element& SetOrigin(glm::vec2 origin);
            Element& SetRelativeSize(glm::vec2 relative_size);
            Element& SetRelativePosition(glm::vec2 relative_position);
            Element& SetOffsetSize(glm::vec2 offset_size);
            Element& SetOffsetPosition(glm::vec2 offset_position);
            Element& SetFixedSize(glm::vec2 fixed_size);
            Element& SetLayoutMode(LayoutMode mode);

            glm::vec2 GetOrigin() const;
            glm::vec2 GetRelativeSize() const;
            glm::vec2 GetRelativePosition() const;
            glm::vec2 GetOffsetSize() const;
            glm::vec2 GetOffsetPosition() const;
            glm::vec2 GetFixedSize() const;
            LayoutMode GetLayoutMode() const;

            //! Calculate the size of the element.
            //!
            //! @param[in] parent_size The absolute size of the parent element.
            virtual void CalculateSize(glm::vec2 parent_size);

            //! Calculate the position of the element
            //!
            //! @param[in] parent_size The absolute size of the parent element.
            //! @param[in] parent_position The absolute position of the parent element.
            virtual void CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position);

            //! Update the element. If element is visible, updates sprite and text primitives.
            virtual void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) = 0;

            glm::vec2 GetAbsoluteSize() const;
            glm::vec2 GetAbsolutePosition() const;

            void AddChild(std::unique_ptr<Element>&& child);

            template<typename T>
            T& EmplaceChild() {
                std::unique_ptr<Element>& child = m_children.emplace_back(std::make_unique<T>());
                return *(static_cast<T*>(child.get()));
            }

            std::vector<std::unique_ptr<Element>>& GetChildren();

        protected:

            void SetAbsoluteSize(glm::vec2 size);
            void SetAbsolutePosition(glm::vec2 position);

        private:

            // The current layout mode of the element.
            LayoutMode m_layout_mode{LayoutMode::RELATIVE_TO_PARENT};

            // Relative size to the parent.
            //
            // (1.0, 1.0) - element has same size as parent
            // (0.5, 0.5) - element is half the size of parent.
            glm::vec2 m_relative_size{1.0F, 1.0F};

            // Relative position to parent.
            //
            // (0.5, 0.5) - element is centered on parent.
            // (1.0, 1.0) - element is located at bottom right corner of parent.
            glm::vec2 m_relative_position {0.0F, 0.0F};

            // Offset in pixels to add to the size during calculation.
            glm::vec2 m_offset_size {0.0F, 0.0F};

            // Offset in pixels to add to the position during calculation.
            glm::vec2 m_offset_position {0.0F, 0.0F};

            // Origin of the element relative to its own size.
            //
            // (0.0, 0.0) - top left corner
            // (0.5, 0.5) - center
            // (1.0, 0.5) - right center
            // (1.0, 1.0) - bottom right corner
            glm::vec2 m_origin {0.0F, 0.0F};

            // Absolute Position. (calculated)
            //
            // Absolute position of the element in pixels, relative to the top-left corner of the screen.
            glm::vec2 m_absolute_position { 0.0F, 0.0F};

            // Absolute Size. (calculated)
            //
            // Absolute size of the element in pixels.
            glm::vec2 m_absolute_size {0.0F, 0.0F};

            // Fixed Size. Used to override the calculated size.
            glm::vec2 m_fixed_size {};

            // Set of child elements.
            std::vector<std::unique_ptr<Element>> m_children;
    };

    //! An element that constains a list of elements arranged horizontally
    class HorizontalLayout : public Element {

        public:
            HorizontalLayout();

            //! Calculate the size of the element.
            //!
            //! @param[in] parent_size The absolute size of the parent element.
            void CalculateSize(glm::vec2 parent_size) override;

            //! Calculate the position of the element
            //!
            //! @param[in] parent_size The absolute size of the parent element.
            //! @param[in] parent_position The absolute position of the parent element.
            void CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) override;

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) override;
    };

    //! An element that constains a list of elements arranged vertically
    class VerticalLayout : public Element {

        public:
            VerticalLayout();

            //! Calculate the size of the element.
            //!
            //! @param[in] parent_size The absolute size of the parent element.
            void CalculateSize(glm::vec2 parent_size) override;

            //! Calculate the position of the element
            //!
            //! @param[in] parent_size The absolute size of the parent element.
            //! @param[in] parent_position The absolute position of the parent element.
            void CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) override;

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) override;
    };

    //! An element that contains a single image.
    class ImageElement : public Element {

        public:

            ImageElement();

            ImageElement& SetTexture(std::shared_ptr<Graphics::Texture2D> p_texture);

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) override;
        private:

            std::shared_ptr<Graphics::Texture2D> m_p_texture;

            ECS::Entity m_entity;
    };

    //! An element that contains a single image.
    class TextElement : public Element {

        public:

            TextElement();

            //! Sets the font for the font element.
            TextElement& SetFont(std::shared_ptr<SDL::TTF::Font> p_font);
            TextElement& SetText(std::shared_ptr<SDL::TTF::Text> p_text);

            //! sets the text string
            TextElement& SetTextString(const std::string& str);
            TextElement& SetTextColor(const glm::vec4& color);

            //! Get the pixel size of characters in the text string.
            glm::vec2 GetTextSize() const;

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) override;
        private:

            std::shared_ptr<SDL::TTF::Font> m_p_font;
            std::shared_ptr<SDL::TTF::Text> m_p_text;

            glm::vec4 m_color;
            ECS::Entity m_entity;
    };

    class NineSliceStyle {

        public:
            enum class Region : uint8_t {
                TOP_LEFT_CORNER = 0,
                TOP_RIGHT_CORNER,
                BOTTOM_LEFT_CORNER,
                BOTTOM_RIGHT_CORNER,
                TOP_EDGE,
                LEFT_EDGE,
                RIGHT_EDGE,
                BOTTOM_EDGE,
                CENTER
            };

            // number of textures that make up a nine-slice.
            static constexpr size_t SLICE_COUNT = 9U;

            static NineSliceStyle Load(Core::Engine& engine, const std::vector<std::string>& images);

            NineSliceStyle();

            void SetRegion(std::shared_ptr<Graphics::Texture2D> p_texture, Region region);
            const std::shared_ptr<Graphics::Texture2D>& GetRegion(Region region) const;

            float GetBorderWidth() const;
        private:
            float CalculateBorderWidth();

            std::vector<std::shared_ptr<Graphics::Texture2D>> m_textures;
            float m_border_width;
    };

    class NineSlice : public Element {

        public:
            NineSlice() = default;

            //! Set the NineSlice style.
            NineSlice& SetStyle(const NineSliceStyle& style);

            void CalculateSize(glm::vec2 parent_size) override;

            void CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) override;

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) override;

        private:

            void CalculateSliceSize(glm::vec2 centerSize, float borderWidth);

            NineSliceStyle m_style;

            // elements used for rendering the nineslice.
            std::vector<std::unique_ptr<UI::ImageElement>> m_borders;
    };

    class Style {

        public:

            static Style Load(Core::Engine& engine, const std::string& filename);

            Style() = default;

            void SetFont(const std::string& font_id, std::shared_ptr<SDL::TTF::Font> p_font);
            std::shared_ptr<SDL::TTF::Font>& GetFont(const std::string& font_id);

            void SetNineSliceStyle(const std::string& style_id, NineSliceStyle&& style);
            NineSliceStyle& GetNineSliceStyle(const std::string& nineslice_id);

        private:

            //! Set of fonts that are used in style.
            std::unordered_map<std::string, std::shared_ptr<SDL::TTF::Font>> m_fonts;

            //! Set of Nine-Slice-Elements used in style.
            std::unordered_map<std::string, NineSliceStyle> m_nine_slice_styles;
    };
}