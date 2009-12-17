/*
 *  BDScene.h
 *  Boeing Demo
 *
 *  Created by WATCH on 8/18/09.
 *  Copyright 2009 Iowa State University. All rights reserved.
 *
 */

#ifndef _BDSCENE_H_
#define _BDSCENE_H_

#include "LightsGroup.h"
#include "DeviceInputController.h"


class BDScene : public aq::KVObserver
{
protected:
	// Constructor
	BDScene();
	
public:
	// BDScene is a singleton instance
	static BDScene& instance() { static BDScene scene;  return scene; }
	
	// Setup the scenegraph
	void setMaster(bool isMaster);
	void init();
	void initPhysics();
	
	// Override KVObserver methods
	void didChangeValueForKey(double value, aq::String key);
	
	// Accessor for the head and wand matrices
	void setHeadMatrix(osg::Matrixf m);
	void setWandMatrix(osg::Matrixf m);
	osg::Matrixf getHeadMatrix();
	osg::Matrixf getWandMatrix();

	// Accessors for the navigation matrix
	void setNavigationMatrix(osg::Matrixf matrix);
	osg::Matrixf getNavigationMatrix();

	// Accessors for scenegraph nodes
	osg::Group* getRootNode();
	osg::Group* getModels();
	
	// Error checking version of OSG"s finddatafile function
	static std::string findDataFile(std::string name);
	
	// Update method called in every juggler latePreFrame and every GLUT display
	void update(double dt);
	
	// Update button info
	void buttonInput(int button, bool pressed);
	
	// Navigate up and down
	void moveUp();
	void moveDown();
	
	void dropBall();
	void setupBoxes();
	
private:
	osg::MatrixTransform* createOSGBox( osg::Vec3 size );
	
	osg::Vec3 _aimingVector;
	btScalar _mass;
	
	// Private variables
	DeviceInputController* _deviceInputController;
	osg::ref_ptr<osg::Group> _rootNode;
	osg::ref_ptr<osg::MatrixTransform> _navTrans;
	osg::ref_ptr<osg::Group> _models;
	osg::ref_ptr<osg::MatrixTransform> _wandTrans;
	osg::Matrixf _wandMatrix;
	osg::Matrixf _headMatrix;
	osg::ref_ptr<LightsGroup> _lightsGroup;
	
	double _totalTime;
	double _step;
	bool _isMaster;
	
	btCollisionShape *sphereShape;
	btDiscreteDynamicsWorld *_dynamicsWorld;
};


#endif


















