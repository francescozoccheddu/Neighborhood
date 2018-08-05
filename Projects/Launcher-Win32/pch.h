#pragma once

#include <WinSDKVer.h>
#include <SDKDDKVer.h>

#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <winrt/base.h>

using namespace winrt;

#pragma comment(lib,"d3d11")
#pragma comment(lib,"windowsapp")

#include <d3d11_3.h>
#include <DirectXMath.h>

#ifdef DXTK_AUDIO
#include "Audio.h"
#endif

#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PrimitiveBatch.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"