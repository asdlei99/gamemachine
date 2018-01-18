﻿#ifndef __DEMO_COLLISION_H__
#define __DEMO_COLLISION_H__

#include <gamemachine.h>
#include <gmdemogameworld.h>
#include "demostration_world.h"

GM_PRIVATE_OBJECT(Demo_Collision)
{
	gm::GMDemoGameWorld* demoWorld = nullptr;
	gm::GMGameObject* ground = nullptr;
	Vector<gm::GMGameObject*> cubes;
};

class Demo_Collision : public DemoHandler
{
	DECLARE_PRIVATE_AND_BASE(Demo_Collision, DemoHandler)

public:
	using Base::Base;
	~Demo_Collision();

public:
	virtual void init() override;
	virtual void event(gm::GameMachineEvent evt) override;

protected:
	virtual void setLookAt() override;
	virtual void setDefaultLights() override;
};

#endif