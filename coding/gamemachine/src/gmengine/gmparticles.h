﻿#ifndef __GMPARTICLEGAMEOBJECT_H__
#define __GMPARTICLEGAMEOBJECT_H__
#include "common.h"
#include "foundation/linearmath.h"
#include "gmgameobject.h"
BEGIN_NS

class GMParticles;
GM_PRIVATE_OBJECT(GMParticleGameObject)
{
	linear_math::Vector2 position = linear_math::Vector2(0, 0);
	linear_math::Vector2 velocity = linear_math::Vector2(0, 0);
	linear_math::Vector4 color = linear_math::Vector4(0, 0);
	GMfloat life = 0;
	GMParticles* parentParticles = nullptr;
};

class GMParticleGameObject : public GMGameObject, public IDestructor
{
	DECLARE_PRIVATE(GMParticleGameObject)

public:
	GMParticleGameObject(Object* prototype);

public:
	inline GMfloat life() { D(d); return d->life; }
	inline Object* getPrototype() { D_BASE(d, GMGameObject); return d->object; }

private:
	virtual void destruct() override {} // 什么都不用做，覆盖基类自动删除prototype的行为，因为它的prototype由GMParticles管理
};

GM_INTERFACE(IParticleHandler)
{
	virtual GMParticleGameObject* createParticle(const GMint index) = 0;
	virtual void update(GMParticleGameObject* particle) = 0;
};

GM_PRIVATE_OBJECT(GMParticles)
{
	Set<Object*> prototypes;
	Vector<GMParticleGameObject*> particles;
	GMuint lastUsedParticle = 0;
	GMint particlesCount = 0;
	IParticleHandler* particleHandler = nullptr;
};

class GMParticles : public GMGameObject
{
	DECLARE_PRIVATE(GMParticles)
public:
	GMParticles(GMint particlesCount, IParticleHandler* handler);
	~GMParticles();

public:
	virtual GMGameObjectType getType() { return GMGameObjectType::Particles; }
	virtual void simulate() override;
	virtual void draw() override;

public:
	inline GMParticleGameObject* getParticle(GMint index);
	GMint findFirstUnusedParticle();

private:
	void addPrototype(AUTORELEASE Object* prototype);
	bool containsPrototype(Object* prototype);
};

END_NS
#endif