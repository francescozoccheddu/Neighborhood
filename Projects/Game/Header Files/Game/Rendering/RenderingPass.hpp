#pragma once

#include <Game/Resources/Resource.hpp>

#define RENDERINGPASS_PIXSHADER_FILENAME(_passName) RES_SHADER_FILENAME(_passName "_pixel")
#define RENDERINGPASS_VERTSHADER_FILENAME(_passName) RES_SHADER_FILENAME(_passName "_vertex")
#define RENDERINGPASS_VERTSHADER_DESCS(_meshClass) _meshClass::s_aInputElementDesc, ARRAYSIZE(_meshClass::s_aInputElementDesc)
#define RENDERINGPASS_SHADERPASS(_passName,_meshClass) {RENDERINGPASS_VERTSHADER_FILENAME(_passName), RENDERINGPASS_PIXSHADER_FILENAME(_passName), RENDERINGPASS_VERTSHADER_DESCS(_meshClass)}
#define RENDERINGPASS_PIXSHADER(_passName) {RENDERINGPASS_PIXSHADER_FILENAME(_passName)}
#define RENDERINGPASS_VERTSHADER(_passName,_meshClass) {RENDERINGPASS_VERTSHADER_FILENAME(_passName), RENDERINGPASS_VERTSHADER_DESCS(_meshClass)}

class RenderingPass : public LoadableResource
{};