#pragma once

#include "core/Engine.hpp"
#include "sdl/SDL.hpp"
#include <unordered_map>
#include <memory>

namespace Systems {
    class RenderSystem;
}

namespace Graphics {

    using PipelineId_t = uint32_t;

    static constexpr uint32_t MAX_PIPELINES = 256; // can't need this many can I?
    static constexpr PipelineId_t NULL_PIPELINE = MAX_PIPELINES;

    class IPipeline {

        public:

            IPipeline() = default;
            IPipeline(const IPipeline& other) = delete;
            IPipeline(IPipeline&& other) = default;
            IPipeline& operator=(const IPipeline& other) = delete;
            IPipeline& operator=(IPipeline&& other) = default;
            virtual ~IPipeline() = default;

            SDL::GraphicsPipeline& Get() {
                return m_pipeline;
            }

            const SDL::GraphicsPipeline& Get() const {
                return m_pipeline;
            }

            virtual void Build(Systems::RenderSystem& rendersys, const std::string& shaderpath) = 0;

        private:

            SDL::GraphicsPipeline m_pipeline;
    };

    // Use to cache pipelines that have previously been built.
    class PipelineCache {

        public:

            PipelineCache() {
                m_pipelines.reserve(MAX_PIPELINES);
            }

            template<typename T>
            IPipeline* Build(Systems::RenderSystem& rendersys, const std::string& shaderpath) {
                PipelineId_t pipelineId = NULL_PIPELINE;
                const char* pipelineName = typeid(T).name();

                const auto& idIter = m_ids.find(pipelineName);
                if (idIter != m_ids.end()) {

                    // Pipeline is already built, no need to build again.
                    pipelineId = idIter->second;
                }
                else if (m_pipelines.size() < MAX_PIPELINES) {

                    // Pipeline hasn't been built. so let's instantiate it.
                    std::unique_ptr<IPipeline> pipeline = std::make_unique<T>();
                    pipeline->Build(rendersys, shaderpath);
                    pipelineId = m_pipelines.size();
                    m_ids[pipelineName] = pipelineId;
                    m_pipelines.push_back(std::move(pipeline));
                }
                else {
                    throw Core::EngineException("Too many pipelines already created. Consider increasing MAX_PIPELINES.");
                }

                return m_pipelines.at(pipelineId).get();
            }

        private:
            std::unordered_map<const char*, PipelineId_t> m_ids;
            std::vector<std::unique_ptr<IPipeline>> m_pipelines;
    };
}