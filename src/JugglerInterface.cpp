/*
 *  JugglerInterface.cpp
 *  Boeing Demo
 *
 *  Created by WATCH on 8/18/09.
 *  Copyright 2009 Iowa State University All rights reserved.
 *
 */

#include "JugglerInterface.h"

// Global variables
float _navSpeed = 1.0;
float _rotSpeed = 100.0;
cluster::UserData< vpr::SerializableObjectMixin<myType> > mMyData;

JugglerInterface::JugglerInterface(vrj::Kernel* kern, int& argc, char** argv) : vrj::OsgApp(kern)
{ 
	_initialTime = 0.0;
	_previousFrameTime = 0.0;
	_timeDelta = 0.0;
	_initialTimeIsSet = false;
	_frameCount = 0;
	_navType = WAND_AND_GAMEPAD;
	_wandIsFlying = false;
}

JugglerInterface::~JugglerInterface()
{ ; }

void JugglerInterface::init()
{
	_head.init("VJHead");		
	_wand.init("VJWand");
	_wandButtons[0].init("VJButton0");
	_wandButtons[1].init("VJButton1");
	_wandButtons[2].init("VJButton2");
	_wandButtons[3].init("VJButton3");
	_wandButtons[4].init("VJButton4");	
	_wandButtons[5].init("VJButton5");

	_button[0].init("Button 1");
	_button[1].init("Button 2");
	_button[2].init("Button 3");
	_button[3].init("Button 4");
	_button[4].init("Button 5");
	_button[5].init("Button 6");
	_button[6].init("Button 7");
	_button[7].init("Button 8");
	_button[8].init("Button 9");
	_button[9].init("Button 10");
	_axis0.init("Left Joystick Horizontal Axis");
	_axis1.init("Left Joystick Vertical Axis");
	_axis2.init("Right Joystick Vertical Axis");
	_axis3.init("Right Joystick Horizontal Axis");
	_axis4.init("DirectionPad Horizontal Axis");
	_axis5.init("DirectionPad Vertical Axis");

	_wandNavSpeed = -0.3f;
	
	vrj::osg::App::init();
}

void JugglerInterface::initScene()
{
	aq::String::print("______initing scene_______");
	
	// Initialize the serializable data structure
	vpr::GUID new_guid("72bc5a7b-c28a-4fc3-8d04-384b847fff31");
    mMyData.init(new_guid);

	// Initialize the scene and audio
	if (mMyData.isLocal())
		BDScene::instance().setMaster(true);
	BDScene::instance().init();
	
	// Initialize the navigation class
	_osgNavigator.init();
}

void JugglerInterface::preFrame()
{
	// Get the initial time
	if (_initialTimeIsSet == false && _frameCount != 0)
	{
		_initialTime = _wand->getTimeStamp().secd();
		_initialTimeIsSet = true;
	}

	// Update the time
	_totalTime = _wand->getTimeStamp().secd() - _initialTime;
	_timeDelta = _totalTime - _previousFrameTime;
	_previousFrameTime = _totalTime;

	// Correct Time Delta
	if (_timeDelta > 1.0 || _timeDelta < 0.0)
		_timeDelta = 0.0;
	
	//================== UPDATE WAND NAVIGATION ==================
	if (_navType == WAND || _navType == WAND_AND_GAMEPAD)
	{
		if (_wandIsFlying == false)
		{
			// Set navigation mode to WALK
			_osgNavigator.setWalkMode(true);
		}

		// Get wand data
		gmtl::Matrix44f wandMatrix = _wand->getData();      // Get the wand matrix

		if ( _wandButtons[3]->getData() == gadget::Digital::TOGGLE_ON )
			_wandNavSpeed -= 0.025f;
		if ( _wandButtons[2]->getData() == gadget::Digital::TOGGLE_ON )
		{
			_wandNavSpeed += 0.025f;
			if (_wandNavSpeed == 0.0f)
				_wandNavSpeed = -0.025f;
		}
		if (_wandButtons[4]->getData() == gadget::Digital::TOGGLE_ON)
		{
			if (_wandIsFlying == false)
			{
				_wandIsFlying = true;
				_osgNavigator.setWalkMode(false);
			}
			else
			{
				_wandIsFlying = false;
				_osgNavigator.setWalkMode(true);			
			}
		}

		// If we are pressing button 1 then translate in the direction the wand is
		// pointing.
		if ( _wandButtons[5]->getData() == gadget::Digital::ON )
		{
		  gmtl::Vec3f direction;
		  gmtl::Vec3f Zdir = gmtl::Vec3f(0.0f, 0.0f, _wandNavSpeed);
		  gmtl::xform(direction, wandMatrix, Zdir);

		  _osgNavigator.setVelocity(direction);
		}  // Make sure to reset the velocity when we stop pressing the button.
		else if ( _wandButtons[5]->getData() == gadget::Digital::TOGGLE_OFF)
		{
		  _osgNavigator.setVelocity(gmtl::Vec3f(0.0, 0.0, 0.0));
		}

		/*  Removed wand rotation for the time being
		
		// If we are pressing button 2 then rotate in the direction the wand is
		// pointing.
		if ( _wandButtons[0]->getData() == gadget::Digital::ON )
		{
			// Only allow Yaw (rot y)
			gmtl::EulerAngleXYZf euler(0.0f, gmtl::makeYRot(_wand->getData()), 0.0f);
			gmtl::Matrix44f rot_mat = gmtl::makeRot<gmtl::Matrix44f>(euler);
			_osgNavigator.setRotationalVelocity(rot_mat);
		}
		// Make sure to reset the rotational velocity when we stop pressing the
		// button.
		else if(_wandButtons[0]->getData() == gadget::Digital::TOGGLE_OFF)
		{
			_osgNavigator.setRotationalVelocity(gmtl::Matrix44f());
		}
		
		*/
	}
	if (_navType == GAMEPAD || _navType == WAND_AND_GAMEPAD) // Update gamepad navigation
	{
		// User navigation with the joysticks
		float axis0 = _axis0->getData();
		float axis1 = _axis1->getData();
		float axis2 = _axis2->getData();
		float axis3 = _axis3->getData();
		float axis4 = _axis4->getData();
		float axis5 = _axis5->getData();

		// The gamepad provides 0.0 - 1.0, this makes the origin 0.0 instead of 0.5
		axis0 -= 0.5; axis1 -= 0.5; axis2 -= 0.5; axis3 -= 0.5; axis4 -= 0.5; axis5 -= 0.5;

		if (axis0 != 0.0 || axis1 != 0.0 || axis2 != 0.0 || axis3 != 0.0 || axis4 != 0.0 || axis5 != 0.0)
		{
			// Set navigation mode to FLY
			_osgNavigator.setWalkMode(false);

			// Scale the values nicely
			axis0 *= _navSpeed; axis1 *= _navSpeed; axis2 *= _navSpeed/2; axis4 /= 20; axis5 /= 20;

			// This ties rotation speed to navSpeed until navSpeed gets too high
			if (_navSpeed < 0.8)
				axis3 /= _rotSpeed * (1 - _navSpeed); 
			else
				axis3 /= _rotSpeed * (1 - 0.5);
			
			// Change navigator to fly mode if we are trying to fly up
			if (axis2 != 0.0 && _osgNavigator.getWalkMode() == OsgNavigator::WALK)
				_osgNavigator.setWalkMode(false);

			// Only allow Yaw (rot y) for navigation
			gmtl::EulerAngleXYZf euler(0.0f, -axis3, 0.0f);
			_osgNavigator.setRotationalVelocity(gmtl::makeRot<gmtl::Matrix44f>(euler));
			_osgNavigator.setVelocity(gmtl::Vec3f(axis0, -axis2, axis1));

			// Only allow Pitch (rot x) and Roll (rot z) for tilt
			gmtl::EulerAngleXYZf eulerTilt(-axis5, 0.0f, axis4);
			_osgNavigator.setTiltVelocity(gmtl::makeRot<gmtl::Matrix44f>(eulerTilt));
		}
	}
	
	// Update the navigator for both the wand and gamepad
	_osgNavigator.update(1.0);
}

void JugglerInterface::latePreFrame()
{
	// Pass changes in button state on to the app
	for (int i = 0; i < 10; i++)
	{
		if (_button[i]->getData() == gadget::Digital::TOGGLE_ON)
			BDScene::instance().buttonInput(i + 1, true);
		else if (_button[i]->getData() == gadget::Digital::TOGGLE_OFF)
			BDScene::instance().buttonInput(i + 1, false);
	}

	// Update BDScene to time delta
	BDScene::instance().update(_timeDelta);

	// Attach the wand's matrix to the wand
	osg::Matrixf wandMatrix(_wand->getData().mData);
	BDScene::instance().setWandMatrix(wandMatrix);
	osg::Matrixf headMatrix	(_head->getData().mData);
	BDScene::instance().setHeadMatrix(headMatrix);


	//================== UPDATE NAVIGATION MATRIX ===============
	
	// Invert the osg navigator matrix
	gmtl::Matrix44f world_transform, currentPosition;
	currentPosition = _osgNavigator.getCurPos();
	gmtl::invertFull(world_transform, currentPosition);

	// Convert to osg matrix
	osg::Matrixf osg_current_matrix;
	osg_current_matrix.set(world_transform.getData());

	// Actually push out the new matrix
	BDScene::instance().setNavigationMatrix(osg_current_matrix);
	
	// Finish updating the scene graph.
	vrj::osg::App::latePreFrame();
}

void JugglerInterface::bufferPreDraw()
{
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Fades in the clear color
	if (_totalTime >= 3.0 && _totalTime <= 11.0)
	{
		float totalFadeInTime = 8.0;
		float step = (_totalTime - 3.0) / totalFadeInTime;
		
		float red = step * 0.5294;
		float green = step * 0.7059;
		float blue = step * 0.9216;
		(*sceneViewer)->setClearColor(osg::Vec4f(red, green, blue, 1.0));
	}
}

void JugglerInterface::configSceneView(osgUtil::SceneView* newSceneViewer)
{
	 newSceneViewer->setDefaults();
  	 newSceneViewer->setClearColor(osg::Vec4f(0.0, 0.0, 0.0, 1.0));
	 
	 // Needed for stereo to work.
	 newSceneViewer->setDrawBufferValue(GL_NONE);

	 _frameStamp = new osg::FrameStamp;
	 newSceneViewer->setFrameStamp(_frameStamp.get());
}

void JugglerInterface::draw()
{
	vrj::osg::App::draw();
	_frameCount++;
}

osg::Group* JugglerInterface::getScene()
{
	return BDScene::instance().getRootNode();
}








