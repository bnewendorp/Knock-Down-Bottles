/*
 *  BDScene.cpp
 *  Boeing Demo
 *
 *  Created by WATCH on 8/18/09.
 *  Copyright 2009 Iowa State University. All rights reserved.
 *
 */

#include "BDScene.h"

extern float _navSpeed;

BDScene::BDScene()
{
	// Create the device and network input controllers
	_deviceInputController = new DeviceInputController();

	// Initialize the audio manager and audio flags
	_isMaster = false;
	_totalTime = 0.0;
	
	// Register listening keys with KVReflector
	aq::KVReflector::instance()->addObserverWithKey(this, "Update_Wand_Matrix");
	aq::KVReflector::instance()->addObserverWithKey(this, "Decrease_Nav_Speed");
	aq::KVReflector::instance()->addObserverWithKey(this, "Increase_Nav_Speed");
	aq::KVReflector::instance()->addObserverWithKey(this, "Drop ball");
}

void BDScene::setMaster(bool isMaster)
{
	_isMaster = isMaster;
}

void BDScene::init()
{
	/*
	 
	 _rootNode			/-- _lightsGroup
			\          /             \
			 \-- _navTrans            \-- _lightSource1, 2, 3, 4
					  \							\
			           \						 \-- _light1, 2, 3, 4
				        \-- _models
							    \
				                 \-- ...
	 */

	_rootNode = new osg::Group();
	_navTrans = new osg::MatrixTransform();
	
	_models = new osg::Group();
	_wandTrans = new osg::MatrixTransform();
	
	// Set the models node to normal scaling
	_models->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	
	// Setup the scenegraph hierarchy
	_rootNode->addChild(_navTrans.get());
	_navTrans->addChild(_models.get());
	_models->addChild(_wandTrans.get());
	
	//=========================================================================
	// RIGHT HERE IS WHERE YOU SHOULD START PUMPING MODELS INTO THE SCENEGRAPH
	//=========================================================================
	initPhysics();
	
	// Initialize the lights group for KVO notifications
	_lightsGroup = new LightsGroup(_models->getOrCreateStateSet());
	
	// Add the lighting to the scene
	_lightsGroup->init();
	_navTrans->addChild(_lightsGroup.get());

	// Run optimization over the scene graph.
	// NOTE: Using the optimizer may cause problems with textures not showing
	// up on all walls.
	osgUtil::Optimizer optimizer;
	optimizer.optimize(_rootNode.get());
}

void BDScene::initPhysics()
{
	// setup the broadphase pass
	btVector3 worldAabbMin(-10000, -10000, -10000);
	btVector3 worldAabbMax(10000, 10000, 10000);
	int maxProxies = 1024;
//	btBroadphaseInterface *broadphase = new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies); // better for more static scenes
	btBroadphaseInterface *broadphase = new btDbvtBroadphase(); // better for scenes with more movement
	
	btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver;
	
	_dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	_dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
	
	btCollisionShape *groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
	btRigidBody::btRigidBodyConstructionInfo
	groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	_dynamicsWorld->addRigidBody(groundRigidBody);
	
//	sphereShape = new btSphereShape(1);
	
	_models->addChild(createOSGBox(osg::Vec3(10000,.1,10000)));
}

void BDScene::dropBall()
{
	osg::ref_ptr<osg::Node> nodeDB = osgDB::readNodeFile("/Users/brandon/Programming/OpenSceneGraph-Data-2.8.0/glider.osg");
	osg::ref_ptr<osg::MatrixTransform> node = new osg::MatrixTransform();
	
	if (nodeDB != NULL)
		node->addChild(nodeDB.get());
	else
	{
		std::cout << "Problem with finding the model" << std::endl;
		return;
	}

	osgbBullet::MotionState *motion = new osgbBullet::MotionState;
	motion->setTransform(node.get());
	static btCollisionShape *cShape;
	if (cShape == NULL)
		cShape = osgbBullet::btConvexTriMeshCollisionShapeFromOSG(node.get());
	osg::Node* debugNode = osgbBullet::osgNodeFromBtCollisionShape( cShape );
    node->addChild( debugNode );
	
	// Set debug node state.
    osg::StateSet* state = debugNode->getOrCreateStateSet();
    osg::PolygonMode* pm = new osg::PolygonMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
    state->setAttributeAndModes( pm );
    osg::PolygonOffset* po = new osg::PolygonOffset( -1, -1 );
    state->setAttributeAndModes( po );
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF );	
	
	btTransform shapeTransform;
	shapeTransform.setIdentity();
	shapeTransform.setOrigin(btVector3(10, 0, -10)); // change this to move the initial position of the object
	motion->setWorldTransform(shapeTransform);
	
	btScalar mass(1.0);
	btVector3 inertia;
	cShape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, motion, cShape, inertia);
	btRigidBody *body = new btRigidBody(rbinfo);
	body->setLinearVelocity( btVector3( -5, -1, 0 ) );
	body->setAngularVelocity( btVector3( 1, 0, 0 ) );
	_dynamicsWorld->addRigidBody(body);
	
	_models->addChild(node.get());
}

void BDScene::didChangeValueForKey(double value, aq::String key)
{
	if (key == "Update_Wand_Matrix")
	{
		osg::Vec3 direction(_wandMatrix.ptr()[8], _wandMatrix.ptr()[9],_wandMatrix.ptr()[10]);
		_navTrans->setMatrix(_navTrans->getMatrix() * osg::Matrix::translate(direction * value * 4.0));
	}
	else if (key == "Decrease_Nav_Speed")
	{
		if (_navSpeed > 0.12)
			_navSpeed -= 0.1;
	}
	else if (key == "Increase_Nav_Speed")
	{
		_navSpeed += 0.1;
	}
	else if (key == "Drop ball")
	{
		dropBall();
	}
}

void BDScene::setHeadMatrix(osg::Matrixf mat)
{
	// Transform the head and wand matrix into navigation 
	// space (optional, depends on how you're using them)
	_headMatrix = mat * _navTrans->getInverseMatrix();
}

void BDScene::setWandMatrix(osg::Matrixf mat)
{
	_wandMatrix = mat * _navTrans->getInverseMatrix();
	_wandTrans->setMatrix(osg::Matrixf::scale(0.25, 0.125, 1.0) * _wandMatrix);
}

osg::Matrixf BDScene::getHeadMatrix()
{
	return _headMatrix;
}
osg::Matrixf BDScene::getWandMatrix()
{
	return _wandMatrix;
}

void BDScene::setNavigationMatrix(osg::Matrixf matrix)
{
	_navTrans->setMatrix(matrix);
}

osg::Matrixf BDScene::getNavigationMatrix()
{
	return _navTrans->getMatrix();
}

osg::Group* BDScene::getRootNode()
{
	return _rootNode.get();
}

osg::Group* BDScene::getModels()
{
	return _models.get();
}

void BDScene::buttonInput(int button, bool pressed)
{
	_deviceInputController->buttonInput(button, pressed);
}

void BDScene::update(double dt)
{
	// Update the total run-time
	_totalTime += dt;
	
	// Process all device inputs
	_deviceInputController->update(dt);
	
	// Update lighting
//	_lightsGroup->updateLights(_totalTime);
	
	// update physics
	_dynamicsWorld->stepSimulation(dt, 2);
}

osg::MatrixTransform* BDScene::createOSGBox( osg::Vec3 size )
{
    osg::Box * box = new osg::Box();
	
    box->setHalfLengths( size );
	
    osg::ShapeDrawable * shape = new osg::ShapeDrawable( box );
	
    osg::Geode * geode = new osg::Geode();
    geode->addDrawable( shape );
	
    osg::MatrixTransform * transform = new osg::MatrixTransform();
    transform->addChild( geode );
	
    return( transform );
}
