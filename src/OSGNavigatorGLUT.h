/*
 *  OSGNavigatorGLUT.h
 *  Boeing Demo
 *
 *  Created by WATCH on 8/19/09.
 *  Copyright 2009 Iowa State University. All rights reserved.
 *
 */


#ifndef _OSG_NAVIGATOR_GLUT_H_
#define _OSG_NAVIGATOR_GLUT_H_

#include <gmtl/EulerAngle.h>
#include <gmtl/EulerAngleOps.h>
#include <gmtl/Generate.h>
#include <gmtl/Vec.h>
#include <gmtl/VecOps.h>
#include <gmtl/Matrix.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Quat.h>
#include <gmtl/QuatOps.h>


class OsgNavigatorGLUT
{
	/** Navigation mode */
	enum NavMode
	{
		WALK,       /**< Constrain to x,y trans and y rotations. */
		FLY         /**< No restrictions */
	};
	
public:

	// Constructor
	OsgNavigatorGLUT();
	
	// Initialization
	void init();
	
	// Set the walk/fly mode
	void setWalkMode(bool walk_mode);
	
	// Set the velocities
	void setVelocity(const gmtl::Vec3f& vel);
	void setRotationalVelocity(const gmtl::Matrix44f& rot_vel);
	
	// Updates the nav matrix
	void update(float delta);
	
	// Accessor methods for the navigation matrix
	gmtl::Matrix44f getCurPos();
	void setCurPos(const gmtl::Matrix44f pos);
	
private:
	bool			mActive;      /**< True if we have been initialized */
	NavMode			mMode;        /**< Current navigation mode */
	gmtl::Vec3f		mVelocity;        /**< Current trans velocity */
	gmtl::Matrix44f	mRotVelocity;     /**< Current angular velocity */
	gmtl::Matrix44f	_navMatrix;
};


#endif
















