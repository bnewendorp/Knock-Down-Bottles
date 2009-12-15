/*
 *  OSGNavigatorGLUT.cpp
 *  Boeing Demo
 *
 *  Created by WATCH on 8/19/09.
 *  Copyright 2009 Iowa State University. All rights reserved.
 *
 */

#include "OSGNavigatorGLUT.h"

OsgNavigatorGLUT::OsgNavigatorGLUT() : mActive(false), mMode(FLY)
{
	// Nothing for now
}

void OsgNavigatorGLUT::init()
{
	mActive = true;
}

void OsgNavigatorGLUT::setWalkMode(bool walk_mode)
{
	if (walk_mode)
		mMode = WALK;
	else
		mMode = FLY;
}

void OsgNavigatorGLUT::setVelocity(const gmtl::Vec3f& vel)
{
	mVelocity = vel;
}

void OsgNavigatorGLUT::setRotationalVelocity(const gmtl::Matrix44f& rot_vel)
{
	mRotVelocity = rot_vel;
}

void OsgNavigatorGLUT::update(float delta)
{
	// Don't update if not active or we have a very large delta
	if (delta > 2.0 || !mActive)
		return;
				
	// Clamp delta
	if (delta > 1.0)
		delta = 1.0f;
	
	// ---------- ROTATION ----------- //
	// Scale the rotation velocity (slerp) to get a time based change
	gmtl::Quatf qrot = gmtl::make<gmtl::Quatf>(mRotVelocity);
	gmtl::Quatf scaled_qrot, src_rot;   // scaled rotation and Identity rotation
	gmtl::Matrix44f delta_rot;          // The delta rotation to use
	
	// Only compute if we don't have identity rotation
	if (!gmtl::isEqual(gmtl::MAT_IDENTITY44F, mRotVelocity, 0.001f))
	{
		gmtl::slerp(scaled_qrot, delta, src_rot, qrot);
		gmtl::set(delta_rot, scaled_qrot);
	}
	
	if (mMode == WALK)
	{
		gmtl::EulerAngleXYZf euler( 0.0f, gmtl::makeYRot(delta_rot), 0.0f );// Only allow Yaw (rot y)
		delta_rot = gmtl::makeRot<gmtl::Matrix44f>( euler );
	}
	gmtl::postMult(_navMatrix, delta_rot);
	
	// ------- TRANSLATION ---- //
	gmtl::Vec3f trans_delta;
	if (mMode == WALK)
		mVelocity[1] = 0.0f;
		
	trans_delta =  mVelocity * delta;
	
	// Post multiply the delta translation
	gmtl::Matrix44f trans_matrix = gmtl::makeTrans<gmtl::Matrix44f>(trans_delta);
	gmtl::postMult(_navMatrix, trans_matrix);
}

gmtl::Matrix44f OsgNavigatorGLUT::getCurPos()
{
	if (!mActive)
		return gmtl::Matrix44f();
	else
		return _navMatrix;
}

void OsgNavigatorGLUT::setCurPos(const gmtl::Matrix44f pos)
{
	if (mActive)
		_navMatrix = pos;
}










