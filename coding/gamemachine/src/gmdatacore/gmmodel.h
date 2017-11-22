﻿#ifndef __GMMODEL_H__
#define __GMMODEL_H__
#include <gmcommon.h>
#include "../foundation/vector.h"
#include <utilities.h>
#include <linearmath.h>
#include <gmimage.h>
#include <gmshader.h>

#define BEGIN_FOREACH_MESH(obj, mesh) for (auto iter = (obj)->getAllMeshes().begin(); iter != (obj)->getAllMeshes().end(); iter++) { GMMesh* mesh = *iter;
#define END_FOREACH_MESH }

BEGIN_NS

class GMModel;
class GMGLShaderProgram;

GM_PRIVATE_OBJECT(GMModelPainter)
{
	GMModel* model = nullptr;
};

class GMMesh;
class GMModelPainter : public GMObject
{
	DECLARE_PRIVATE(GMModelPainter)

public:
	GMModelPainter(GMModel* obj)
	{
		D(d);
		d->model = obj;
	}

public:
	virtual void transfer() = 0;
	virtual void draw(GMfloat* modelTransform) = 0;
	virtual void dispose() = 0;

// 提供修改缓存的方法
	virtual void beginUpdateBuffer(GMMesh* mesh) = 0;
	virtual void endUpdateBuffer() = 0;
	virtual void* getBuffer() = 0;

protected:
	inline GMModel* getModel() { D(d); return d->model; }
};

GM_PRIVATE_OBJECT(GMComponent)
{
	GMuint offset = 0;
	// 绘制图元数量
	GMuint primitiveCount = 0;

	// 图元顶点数量
	Vector<GMint> primitiveVertices;
	// 顶点在ChildObject的偏移
	Vector<GMint> vertexOffsets;

	GMMesh* parentMesh;
	GMuint currentFaceVerticesCount;
	GMShader shader;
};

class GMComponent : public GMObject
{
	DECLARE_PRIVATE(GMComponent)

	friend class GMMesh;

public:
	GMComponent(GMMesh* parent);

	inline GMShader& getShader() { D(d); return d->shader; }
	inline void setShader(const GMShader& shader) { D(d); d->shader = shader; }
	inline GMint* getOffsetPtr() { D(d); return d->vertexOffsets.data(); }
	inline GMint* getPrimitiveVerticesCountPtr() { D(d); return d->primitiveVertices.data(); }
	inline GMuint getPrimitiveCount() { D(d); return d->primitiveCount; }

	void clear();
	void setVertexOffset(GMuint offset);
	void beginFace();
	void vertex(GMfloat x, GMfloat y, GMfloat z);
	void normal(GMfloat x, GMfloat y, GMfloat z);
	void uv(GMfloat u, GMfloat v);
	void lightmap(GMfloat u, GMfloat v);
	void color(GMfloat r, GMfloat g, GMfloat b, GMfloat a = 1.0f);
	void endFace();
	void expand(GMuint count);
};

enum class GMUsageHint
{
	StaticDraw,
	DynamicDraw,
};

GM_PRIVATE_OBJECT(GMModel)
{
	GMUsageHint hint = GMUsageHint::StaticDraw;
	Vector<GMMesh*> meshes;
	AutoPtr<GMModelPainter> painter;
};

// 所有的顶点属性类型
enum class GMVertexDataType
{
	Position = 0,
	Normal,
	UV,
	Tangent,
	Bitangent,
	Lightmap,
	Color,

	// ---
	EndOfVertexDataType
};

#define gmVertexIndex(i) ((GMuint)i)

class GMModel : public GMObject
{
	DECLARE_PRIVATE(GMModel)

public:
	typedef GMfloat DataType;

	enum {
		// 一个位置坐标的维度
		PositionDimension = 4,
	};

public:
	~GMModel();

public:
	inline void setPainter(AUTORELEASE GMModelPainter* painter) { D(d); d->painter.reset(painter); }
	inline GMModelPainter* getPainter() { D(d); return d->painter; }
	inline Vector<GMMesh*>& getAllMeshes() { D(d); return d->meshes; }
	inline void append(AUTORELEASE GMMesh* obj) { D(d); d->meshes.push_back(obj); }

	// 绘制方式
	void setUsageHint(GMUsageHint hint) { D(d); d->hint = hint; }
	GMUsageHint getUsageHint() { D(d); return d->hint; }
};

// 绘制时候的排列方式
enum class GMArrangementMode
{
	// 默认排列，按照Components，并按照一个个三角形来画
	Triangle_Fan,
	Triangle_Strip,
	Triangles,
	Lines,
};

enum class GMMeshType
{
	MeshTypeBegin,
	Model3D = MeshTypeBegin,
	Model2D,
	Glyph,
	Particles,
	MeshTypeEnd,
};

#define GM_DEFINE_VERTEX_DATA(name) \
	Vector<GMModel::DataType> name; \
	GMuint transferred_##name##_byte_size = 0;

#define GM_DEFINE_VERTEX_PROPERTY(name) \
	inline auto& name() { D(d); return d->name; } \
	inline void clear_##name##_and_save_byte_size() {D(d); set_transferred_##name##_byte_size(name().size() * sizeof(GMModel::DataType)); name().clear(); } \
	inline GMuint get_transferred_##name##_byte_size() { D(d); return d->transferred_##name##_byte_size; } \
	inline void set_transferred_##name##_byte_size(GMuint size) { D(d); d->transferred_##name##_byte_size = size; }

GM_PRIVATE_OBJECT(GMMesh)
{
	GM_DEFINE_VERTEX_DATA(positions);
	GM_DEFINE_VERTEX_DATA(normals);
	GM_DEFINE_VERTEX_DATA(uvs);
	GM_DEFINE_VERTEX_DATA(tangents);
	GM_DEFINE_VERTEX_DATA(bitangents);
	GM_DEFINE_VERTEX_DATA(lightmaps);
	GM_DEFINE_VERTEX_DATA(colors); //顶点颜色，一般渲染不会用到这个，用于粒子绘制

	bool disabledData[gmVertexIndex(GMVertexDataType::EndOfVertexDataType)] = { 0 };
	GMuint arrayId = 0;
	GMuint bufferId = 0;
	Vector<GMComponent*> components;
	GMMeshType type = GMMeshType::Model3D;
	GMArrangementMode mode = GMArrangementMode::Triangle_Fan;
	GMString name = _L("default");
};

class GMMesh : public GMObject
{
	DECLARE_PRIVATE(GMMesh)

	friend class Object_Less;

public:
	GMMesh();
	~GMMesh();

public:
	void clone(OUT GMMesh** childObject);
	void appendComponent(AUTORELEASE GMComponent* component);
	void calculateTangentSpace();

public:
	GM_DEFINE_VERTEX_PROPERTY(positions);
	GM_DEFINE_VERTEX_PROPERTY(normals);
	GM_DEFINE_VERTEX_PROPERTY(uvs);
	GM_DEFINE_VERTEX_PROPERTY(tangents);
	GM_DEFINE_VERTEX_PROPERTY(bitangents);
	GM_DEFINE_VERTEX_PROPERTY(lightmaps);
	GM_DEFINE_VERTEX_PROPERTY(colors);

	inline void disableData(GMVertexDataType type) { D(d); d->disabledData[gmVertexIndex(type)] = true; }
	inline bool isDataDisabled(GMVertexDataType type) { D(d); return d->disabledData[gmVertexIndex(type)]; }
	inline Vector<GMComponent*>& getComponents() { D(d); return d->components; }
	inline GMMeshType getType() { D(d); return d->type; }
	inline void setType(GMMeshType type) { D(d); d->type = type; }
	inline void setArrangementMode(GMArrangementMode mode) { D(d); d->mode = mode; }
	inline GMArrangementMode getArrangementMode() { D(d); return d->mode; }
	inline void setName(const char* name) { D(d); d->name = name; }
	inline const GMString& getName() { D(d); return d->name; }
	inline GMuint getBufferId() { D(d); return d->bufferId; }
	inline GMuint getArrayId() { D(d); return d->arrayId; }
	inline void setBufferId(GMuint id) { D(d); d->bufferId = id; }
	inline void setArrayId(GMuint id) { D(d); d->arrayId = id; }
};

#define IF_ENABLED(mesh, type) if (!mesh->isDataDisabled(type))

END_NS
#endif