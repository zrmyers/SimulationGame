#include "ShaderCross.hpp"
#include <SDL3/SDL_gpu.h>
#include <SPIRV/GlslangToSpv.h>
#include <SPIRV/Logger.h>
#include <algorithm>
#include <fstream>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <ios>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <array>
#include "core/Filesystem.hpp"
#include "core/Logger.hpp"
#include "core/Engine.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Byte Code

Graphics::ByteCode::ByteCode()
    : m_format(0)
    , m_stage(SDL_GPU_SHADERSTAGE_VERTEX) {
}

Graphics::ByteCode::ByteCode(SDL_GPUShaderFormat format, SDL_GPUShaderStage stage, std::vector<uint32_t> code, std::string entry_point)
    : m_format(format)
    , m_stage(stage)
    , m_code(std::move(code))
    , m_entry(std::move(entry_point)) {
}

SDL_GPUShaderFormat Graphics::ByteCode::GetFormat() const {
    return m_format;
}

SDL_GPUShaderStage Graphics::ByteCode::GetStage() const {
    return m_stage;
}

const void* Graphics::ByteCode::Get() const {
    return m_code.data();
}

//! Get the size of the byte code.
size_t Graphics::ByteCode::GetSize() const {
    return m_code.size();
}

//! Get the entrypoint for the byte code.
const char* Graphics::ByteCode::GetEntrypoint() const {
    return m_entry.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
// Shader Cross Compiler

Graphics::ShaderCross::ShaderCross() {

    glslang::InitializeProcess();
}


Graphics::ShaderCross::~ShaderCross() {
    glslang::FinalizeProcess();
}


Graphics::ByteCode Graphics::ShaderCross::CompileToSpirv( // NOLINT
    EShLanguage stage,
    const std::string& filename,
    const std::string& entryPoint,
    const std::string& includeDir,
    const std::list<Define>& defines) {

    // Process #define macros
    std::string preamble;
    if (!defines.empty()) {
        std::stringstream preamblestream;
        for (const Define& define : defines) {
            preamblestream << "#define " << define.first;
            if (!define.second.empty()) {
                preamblestream << " " << define.second << "\n";
            }
        }

        preamble = preamblestream.str();
    }

    // Process file
    std::string filedata = Core::Filesystem::LoadFileAsString(filename);

    // Setup includes
    ShaderIncluder include(includeDir);

    // Individual shader compilation.
    glslang::TShader shader(stage);

    shader.setPreamble(preamble.c_str());
    shader.setEntryPoint(entryPoint.c_str());

    // source language
    shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 100);// NOLINT
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_4);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_6);

    std::array<const char*,1> textArray = {filedata.c_str()};
    std::array<const char*,1> fileNameList = {filename.c_str()};

    shader.setStringsWithLengthsAndNames(textArray.data(), nullptr, fileNameList.data(), 1);
    if(!shader.parse(GetDefaultResources(), 100, false, EShMsgDefault, include)) {
        Core::Logger::Info(shader.getInfoLog());
        Core::Logger::Info(shader.getInfoDebugLog());

        throw Core::EngineException("Failed to compile " + filename);
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(EShMsgDefault)) {
        Core::Logger::Info(program.getInfoLog());
        Core::Logger::Info(program.getInfoDebugLog());
        throw Core::EngineException("Failed to link " + filename);
    }

    program.buildReflection(EShReflectionOptions::EShReflectionDefault);
    program.dumpReflection();

    glslang::TIntermediate* p_intermediat = program.getIntermediate(stage);

    // now we can create the bytecode instance.
    if (p_intermediat == nullptr) {
        throw Core::EngineException("CompileToSpirv(): p_binary is null");
    }

    spv::SpvBuildLogger logger;
    std::vector<uint32_t> spv;
    glslang::GlslangToSpv(*p_intermediat, spv, &logger);

    Core::Logger::Info(logger.getAllMessages());

    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_SPIRV;
    SDL_GPUShaderStage sdlStage = (stage == EShLangVertex)? SDL_GPU_SHADERSTAGE_VERTEX : SDL_GPU_SHADERSTAGE_FRAGMENT;

    return ByteCode(format, sdlStage, std::move(spv), entryPoint);
}

SDL::Shader Graphics::ShaderCross::CompileSpirvToGraphicsShader(const ByteCode& code, SDL::GpuDevice& gpu) {

    return {};
}

SDL::Alloc<Graphics::GraphicsShaderMetadata> Graphics::ShaderCross::ReflectGraphicsMetadata(const ByteCode& code) { // NOLINT(readability-convert-member-functions-to-static)

    return {};
}

//----------------------------------------------------------------------------------------------------------------------
// Shader Include Preprocessor

Graphics::ShaderIncluder::ShaderIncluder(std::string include)
    : m_include_dir(std::move(include)) {
}


glslang::TShader::Includer::IncludeResult* Graphics::ShaderIncluder::includeLocal(
    const char* p_header_name,
    const char* p_includer_name,
    size_t depth) {

    IncludeResult* p_result = nullptr;

    std::string resolve = m_include_dir + "/" + std::string(p_header_name);
    std::replace(resolve.rbegin(), resolve.rend(), '\\', '/');
    std::ifstream file(resolve, std::ios_base::binary | std::ios_base::ate);

    if (file) {

        IncludeIndex index;
        std::ifstream::pos_type fileSize = file.tellg();
        index.buffer.resize(fileSize);

        file.read(index.buffer.data(), fileSize);

        index.p_result = std::make_unique<IncludeResult>(p_header_name, index.buffer.data(), index.buffer.size(), nullptr);
        p_result = index.p_result.get();
    }

    return p_result;
}

// Release the include result. Called when shader processer has finished pre-processing.
void Graphics::ShaderIncluder::releaseInclude(IncludeResult * p_result) {

    // Do nothing. Will let ShaderIncluder cleanup on destruction.
}