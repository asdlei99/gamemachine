﻿#ifndef __BSPPATCH_H__
#define __BSPPATCH_H__
#include "common.h"
#include "foundation/linearmath.h"
#include "foundation/vector.h"

BEGIN_NS

struct GMBSP_Physics_Patch;
GM_PRIVATE_OBJECT(GMBSPPatch)
{
	~GM_PRIVATE_CONSTRUCT(GMBSPPatch);
	AUTORELEASE AlignedVector<GMBSP_Physics_Patch*> patches;
};

struct GMBSPPatchCollide;
GM_ALIGNED_16(class) GMBSPPatch : public GMObject
{
	DECLARE_PRIVATE(GMBSPPatch)

public:
	void alloc(GMint num);
	GMBSP_Physics_Patch* patches(GMint at);
	void generatePatchCollide(GMint index, GMint width, GMint height, const linear_math::Vector3* points, AUTORELEASE GMBSP_Physics_Patch* patch);
};

END_NS
#endif