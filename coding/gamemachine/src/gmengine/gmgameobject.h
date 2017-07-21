﻿#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__
#include "common.h"
#include "foundation/vector.h"
#include "foundation/utilities/utilities.h"
#include "gmdatacore/object.h"

BEGIN_NS

GM_PRIVATE_OBJECT(GMGameObject)
{
	GMuint id = 0;
	GMGameWorld* world = nullptr;
	Object* object = false;
	bool manualDelete = false;
	linear_math::Matrix4x4 scaling;
	linear_math::Matrix4x4 translation;
	linear_math::Quaternion rotation;
	linear_math::Matrix4x4 transformMatrix;
};

enum class GMGameObjectType
{
	Static,
	Entity,
	Sprite,
	Particles,
	Custom,
};

class GMGameObject : public GMObject
{
	DECLARE_PRIVATE(GMGameObject)

public:
	GMGameObject(AUTORELEASE Object* obj);
	virtual ~GMGameObject();

public:
	void setObject(AUTORELEASE Object* obj);
	Object* getObject();

	virtual void setWorld(GMGameWorld* world);
	GMGameWorld* getWorld();
	virtual void onAppendingObjectToWorld();
	virtual GMGameObjectType getType() { return GMGameObjectType::Static; }
	virtual void draw();
	virtual void simulate() {}
	virtual void updateAfterSimulate() {}

public:
	inline void setScaling(const linear_math::Matrix4x4& scaling) { D(d); updateMatrix(); d->scaling = scaling; }
	inline void setTranslate(const linear_math::Matrix4x4& translation) { D(d); updateMatrix(); d->translation = translation; }
	inline void setRotation(const linear_math::Quaternion& rotation) { D(d); updateMatrix(); d->rotation = rotation; }
	inline const linear_math::Matrix4x4& getTransform() { D(d); return d->transformMatrix; }

protected:
	void setManualDelete() { D(d); d->manualDelete = true; }

private:
	inline void updateMatrix();
};

//GMGlyphObject
struct GlyphProperties
{
};

GM_PRIVATE_OBJECT(GMGlyphObject)
{
	GMString lastRenderText;
	GMString text;
	GlyphProperties properties;
	GMfloat left, bottom, width, height;
	ITexture* texture;
};

class Component;
class GMGlyphObject : public GMGameObject
{
	DECLARE_PRIVATE(GMGlyphObject)

public:
	GMGlyphObject();

public:
	void setText(const GMWchar* text);
	void setGeometry(GMfloat left, GMfloat bottom, GMfloat width, GMfloat height);
	void updateObject();

private:
	virtual void draw() override;
	virtual void onAppendingObjectToWorld() override;

private:
	void constructObject();
};

//GMEntityObject
enum { EntityPlaneNum = 6 };

GM_PRIVATE_OBJECT(GMEntityObject)
{
	linear_math::Vector3 mins, maxs;
	Plane planes[EntityPlaneNum];
};

class GMEntityObject : public GMGameObject
{
	DECLARE_PRIVATE(GMEntityObject)

public:
	GMEntityObject(AUTORELEASE Object* obj);

public:
	virtual GMGameObjectType getType() { return GMGameObjectType::Entity; }

	Plane* getPlanes();
	void getBounds(REF linear_math::Vector3& mins, REF linear_math::Vector3& maxs);

private:
	void calc();
	void makePlanes();
};

// GMSkyObject
GM_PRIVATE_OBJECT(GMSkyGameObject)
{
	linear_math::Vector3 min;
	linear_math::Vector3 max;
	Shader shader;
};

class GMSkyGameObject : public GMGameObject
{
	DECLARE_PRIVATE(GMSkyGameObject)

public:
	GMSkyGameObject(const Shader& shader, const linear_math::Vector3& min, const linear_math::Vector3& max);

private:
	void createSkyBox(OUT Object** obj);
};

END_NS
#endif
