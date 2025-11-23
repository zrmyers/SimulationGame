#include "SimulationGame.hpp"
#include "core/AssetLoader.hpp"
#include "core/IGame.hpp"
#include "core/Logger.hpp"
#include "graphics/Renderable.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/ShaderCross.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <array>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iomanip>
#include <sstream>
#include <string>

#include "glm/mat4x4.hpp"
#include "graphics/TextRenderer.hpp"

SimulationGame::SimulationGame(Core::Engine& engine)
    : Core::IGame(engine) {
    Core::Logger::Info("Initializing Game.");
    Core::AssetLoader& assetLoader = engine.GetAssetLoader();
    Graphics::Renderer& renderer = engine.GetRenderer();
    Graphics::TextRenderer& textRenderer = engine.GetTextRenderer();

    m_uniform.projview = glm::perspective(glm::pi<float>() / 2.0f, 1024.0F / 768.0F, 0.1F, 100.0F); // NOLINT

    m_font = textRenderer.CreateFont(assetLoader.GetFontDir() + "/Oblegg-Regular.otf", 50.0F); // NOLINT
    m_font.SetHorizontalAlignment(TTF_HORIZONTAL_ALIGN_CENTER);
    m_font.SetSDF(false);

    m_text = textRenderer.CreateText(m_font, "Hello Text!\n1234");
}

void SimulationGame::Update() {

    std::stringstream msgStream;
    msgStream << std::fixed << std::setprecision(2);
    msgStream << "FPS: " << 1.0F / GetEngine().GetDeltaTimeSec();

    std::string message = msgStream.str();
    m_text.text.SetString(message);

    int textWidth = 0;
    int textHeight = 0;
    m_text.text.GetSize(textWidth, textHeight);
    // text pipeline
    m_uniform.model = glm::mat4(1.0F);
    m_uniform.model = glm::translate(m_uniform.model, glm::vec3(0.0F, 0.0F, -80.0F)); // NOLINT
    m_uniform.model = glm::scale(m_uniform.model, glm::vec3(0.3F, 0.3F, 0.3F)); // NOLINT

    std::list<Graphics::TextComponent*> textComponents;
    textComponents.push_back(&m_text);

    std::vector<Graphics::Renderable> renderables = GetEngine().GetTextRenderer().BuildRenderables(textComponents);
    GetEngine().GetRenderer().Update(renderables, m_uniform);
}

