#pragma once

#include <cstddef>
#include <list>
#include <memory>
#include <string>

#include "sdl/SDL.hpp"
#include "glslang/Public/ShaderLang.h"

namespace Graphics {

    //! Used to represent a #define when compiling an HLSL program.
    using Define = std::pair<std::string, std::string>;

    //! ByteCode exported from the cross compiler.
    class ByteCode {

        public:

            //! Compiled bytecode.
            ByteCode();
            ByteCode(SDL_GPUShaderFormat format, SDL_GPUShaderStage stage, std::vector<uint32_t> code, std::string entry_point);

            //! The format of the compiled shader code.
            SDL_GPUShaderFormat GetFormat() const;

            //! The stage of the pipeline that the code is loaded.
            SDL_GPUShaderStage GetStage() const;

            //! Get a pointer to the compiled bytecode.
            const void* Get() const;

            //! Get the size of the byte code.
            size_t GetSize() const;

            //! Get the entrypoint for the byte code.
            const char* GetEntrypoint() const;

        private:

            //! The format.
            SDL_GPUShaderFormat m_format;

            //! Pipeline stage.
            SDL_GPUShaderStage m_stage;

            //! ByteCode
            std::vector<uint32_t> m_code;

            //! The entry point of the program.
            std::string m_entry;
    };

    struct GraphicsShaderMetadata {
        uint32_t metadata;
    };

    //! Shader Cross Compiler.
    //!
    //! This wrapper is used for enabling cross-compilation of shaders during build.
    class ShaderCross {

        public:

            //! Initializes the library.
            ShaderCross();
            ShaderCross(const ShaderCross& other) = delete;
            ShaderCross(ShaderCross&& other) = delete;
            ShaderCross& operator=(const ShaderCross& other) = delete;
            ShaderCross& operator=(ShaderCross&& other) = delete;

            //! De-initializes the library.
            ~ShaderCross();

            //! Do compilation. returns byte code object.
            ByteCode CompileToSpirv(
                EShLanguage stage,
                const std::string& filename,
                const std::string& entryPoint,
                const std::string& includeDir,
                const std::list<Define>& defines);

            //! Compile the ByteCode to a GPU shader.
            SDL::Shader CompileSpirvToGraphicsShader(const ByteCode& code, SDL::GpuDevice& gpu);

            //! Reflect metadata from the bytecode. This is used to compile the SPIRV to the GPU shader.
            SDL::Alloc<GraphicsShaderMetadata> ReflectGraphicsMetadata(const ByteCode& code);


    };

    //! The includer used for resolving includes
    class ShaderIncluder : public glslang::TShader::Includer {

        public:
            //! Include directory.
            ShaderIncluder(std::string include);

            // For the "local"-only aspect of a "" include. Should not search in the
            // "system" paths, because on returning a failure, the parser will
            // call includeSystem() to look in the "system" locations.
            IncludeResult* includeLocal(
                const char* p_header_name,
                const char* p_includer_name,
                size_t depth) override;

            // Release the include result. Called when shader processer has finished pre-processing.
            void releaseInclude(IncludeResult * p_result) override;

        private:

            struct IncludeIndex {
                std::vector<char> buffer;
                std::unique_ptr<IncludeResult> p_result;
            };

            // List of directories
            std::string m_include_dir;

            // Include indexing
            std::list<IncludeIndex> m_include_index;
    };
}