﻿#include "stdafx.h"
#include "gmdx11modelpainter.h"
#include <gamemachine.h>
#include "gmdx11graphic_engine.h"

GMDx11ModelPainter::GMDx11ModelPainter(GMDx11GraphicEngine* engine, GMModel* model)
	: GMModelPainter(model)
{
	D(d);
	d->engine = engine;
}

bool GMDx11ModelPainter::getInterface(GameMachineInterfaceID id, void** out)
{
	D(d);
	if (id == GameMachineInterfaceID::D3D11Buffer)
	{
		d->buffer->AddRef();
		(*out) = d->buffer;
		return true;
	}
	return false;
}

void GMDx11ModelPainter::packData(Vector<GMDx11VertexData>& packedData)
{
	D(d);
	GMDx11VertexData vd = { 0 };
	GMModel* model = getModel();
	// 按照position的size()/3来分配顶点
	// 先不考虑崩溃什么的情况
	GM_ASSERT(model->getMesh()->positions().size() % 3 == 0);
	for (GMuint i = 0; i < model->getMesh()->positions().size() / 3; ++i)
	{
		{
			auto& data_ref = model->getMesh()->positions();
			vd.vertices[0] = data_ref[i * 3];
			vd.vertices[1] = data_ref[i * 3 + 1];
			vd.vertices[2] = data_ref[i * 3 + 2];
		}

		{
			auto& data_ref = model->getMesh()->normals();
			vd.normal[0] = data_ref[i * 3];
			vd.normal[1] = data_ref[i * 3 + 1];
			vd.normal[2] = data_ref[i * 3 + 2];
		}

		{
			auto& data_ref = model->getMesh()->texcoords();
			vd.texcoord[0] = data_ref[i * 2];
			vd.texcoord[1] = data_ref[i * 2 + 1];
		}

		packedData.push_back(vd);
	}
}

void GMDx11ModelPainter::transfer()
{
	D(d);
	if (d->inited)
		return;

	GMModel* model = getModel();
	if (!model->isNeedTransfer())
		return;

	GMMesh* mesh = model->getMesh();
	mesh->calculateTangentSpace();

	Vector<GMDx11VertexData> packedData;
	// 把数据打入顶点数组
	packData(packedData);

	D3D11_USAGE usage = model->getUsageHint() == GMUsageHint::StaticDraw ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
	D3D11_BUFFER_DESC bufDesc;
 	bufDesc.Usage = usage;
	bufDesc.ByteWidth = packedData.size() * sizeof(decltype(packedData)::value_type);
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA bufData;
	bufData.pSysMem = packedData.data();
	bufData.SysMemPitch = bufData.SysMemSlicePitch = 0;

	GMComPtr<ID3D11Device> device = d->engine->getDevice();
	GM_DX_HR(device->CreateBuffer(&bufDesc, &bufData, &d->buffer));

	d->inited = true;
	model->needNotTransferAnymore();
}

void GMDx11ModelPainter::draw(const GMGameObject* parent)
{
	D(d);
	GMModel* model = getModel();
	IRenderer* renderer = d->engine->getRenderer(model->getType());
	renderer->beginModel(model, parent);

	GMMesh* mesh = getModel()->getMesh();
	for (auto component : mesh->getComponents())
	{
		GMShader& shader = component->getShader();
		if (shader.getNodraw())
			continue;

		draw(renderer, component, mesh);
	}
	renderer->endModel();
}

void GMDx11ModelPainter::dispose(GMMeshData* md)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void GMDx11ModelPainter::beginUpdateBuffer(GMMesh* mesh)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void GMDx11ModelPainter::endUpdateBuffer()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void* GMDx11ModelPainter::getBuffer()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void GMDx11ModelPainter::draw(IRenderer* renderer, GMComponent* component, GMMesh* mesh)
{
	renderer->draw(this, component, mesh);
}