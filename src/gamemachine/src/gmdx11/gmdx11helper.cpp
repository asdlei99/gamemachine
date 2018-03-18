﻿#include "stdafx.h"
#include "gmdx11helper.h"
#include <gmdxincludes.h>
#include "foundation/gamemachine.h"
#include <gmcom.h>
#include "gmdx11shaderprogram.h"

extern "C"
{
	IShaderProgram* GMLoadDx11Shader(
		IGraphicEngine* engine,
		const gm::GMString& filename,
		const gm::GMString& entryPoint,
		const gm::GMString& profile,
		GMShaderType type,
		const IShaderProgram* shaderProgram
	)
	{
		gm::GMComPtr<ID3D10Blob> errorMessage;
		gm::GMComPtr<ID3D10Blob> shaderBuffer;
		HRESULT hr;

		gm::GMBuffer buf;
		gm::GMString path;
		GM.getGamePackageManager()->readFile(gm::GMPackageIndex::Shaders, filename, &buf, &path);
		if (!buf.size)
			return nullptr;

		UINT flag = D3D10_SHADER_ENABLE_STRICTNESS;
#if _DEBUG
		flag |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

		buf.convertToStringBuffer();
		hr = D3DX11CompileFromMemory(
			(char*)buf.buffer,
			buf.size,
			path.toStdString().c_str(),
			NULL,
			NULL,
			entryPoint.toStdString().c_str(),
			profile.toStdString().c_str(),
			flag,
			0,
			NULL,
			&shaderBuffer,
			&errorMessage,
			NULL
		);
		if (FAILED(hr))
		{
			if (errorMessage)
			{
				void* ptr = errorMessage->GetBufferPointer();
				size_t sz = errorMessage->GetBufferSize();
				char* t = ((char*)ptr);
				gm_error(L"Error in Buf");
				GM_ASSERT(false);
			}
			else
			{
				gm_error(L"Cannot find shader file %s", path.c_str());
				GM_ASSERT(false);
			}
			return nullptr;
		}

		GMComPtr<ID3D11Device> device;
		bool b = GM.getGraphicEngine()->getInterface(GameMachineInterfaceID::D3D11Device, (void**)&device);
		if (!b || !device)
			return nullptr;

		if (type == GMShaderType::Effect)
		{
			GMComPtr<ID3DX11Effect> effect;
			GM_DX_HR(D3DX11CreateEffectFromMemory(
				shaderBuffer->GetBufferPointer(),
				shaderBuffer->GetBufferSize(),
				0,
				device,
				&effect
			));

			GMDx11EffectShaderProgram* shaderProgram = new GMDx11EffectShaderProgram(effect);
			b = engine->setInterface(GameMachineInterfaceID::D3D11ShaderProgram, shaderProgram);
			GM_ASSERT(b);

			return shaderProgram;
		}
		else
		{
			return nullptr;
		}

		return nullptr;
	}

	const D3D11_RASTERIZER_DESC& GMGetDefaultRasterizerDesc(
		bool multisampleEnable,
		bool antialiasedLineEnable
	)
	{
		static D3D11_RASTERIZER_DESC desc = {
			D3D11_FILL_SOLID,
			D3D11_CULL_BACK,
			FALSE,
			0,
			0.f,
			0.f,
			TRUE,
			FALSE,
			multisampleEnable ? TRUE : FALSE,
			antialiasedLineEnable ? TRUE : FALSE
		};
		return desc;
	}
}