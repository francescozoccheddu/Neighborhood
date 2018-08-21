#pragma once

#include <Game/Resources/Resource.hpp>

#define RENDERINGPASS_PIXSHADER_FILENAME(_passName) RES_SHADER_FILENAME(_passName "_pixel")
#define RENDERINGPASS_VERTSHADER_FILENAME(_passName) RES_SHADER_FILENAME(_passName "_vertex")
#define RENDERINGPASS_SHADERPASS(_passName,_descs) {RENDERINGPASS_VERTSHADER_FILENAME(_passName), RENDERINGPASS_PIXSHADER_FILENAME(_passName), _descs, ARRAYSIZE(_descs)}
#define RENDERINGPASS_PIXSHADER(_passName) {RENDERINGPASS_PIXSHADER_FILENAME(_passName)}
#define RENDERINGPASS_VERTSHADER(_passName,_descs) {RENDERINGPASS_VERTSHADER_FILENAME(_passName), _descs, ARRAYSIZE(_descs)}

class RenderingPass : public LoadableResource
{};