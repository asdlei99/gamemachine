﻿#ifndef __GMDISCRETEDYNAMICSWORLD_H__
#define __GMDISCRETEDYNAMICSWORLD_H__
#include <gmcommon.h>
#include "gmphysicsworld.h"
#include "gmbulletforward.h"

BEGIN_NS
class GMConstraint;

GM_PRIVATE_CLASS(GMDiscreteDynamicsWorld);
class GM_EXPORT GMDiscreteDynamicsWorld : public GMPhysicsWorld
{
	GM_DECLARE_PRIVATE(GMDiscreteDynamicsWorld);
	GM_DECLARE_BASE(GMPhysicsWorld)

public:
	GMDiscreteDynamicsWorld(GMGameWorld* world);
	~GMDiscreteDynamicsWorld();

public:
	void setGravity(const GMVec3& gravity);
	void addRigidObject(AUTORELEASE GMRigidPhysicsObject* rigidObj);
	void addConstraint(GMConstraint* constraint, bool disableCollisionsBetweenLinkedBodies = false);

	//! 将一个约束从本物理世界移除。
	/*!
	  被移除的约束的生命周期将不再由本物理世界管理。用户需要自己管理此约束的生命周期。
	  \param constraint 需要移除的约束。
	*/
	void removeConstraint(GMConstraint* constraint);
	GMPhysicsRayTestResult rayTest(const GMVec3& rayFromWorld, const GMVec3& rayToWorld);

public:
	virtual void update(GMDuration dt, GMGameObject* obj) override;

private:
	void syncTransform();
};

END_NS
#endif