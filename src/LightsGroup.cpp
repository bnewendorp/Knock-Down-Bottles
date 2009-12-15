/*
 *  LightsGroup.cpp
 *  Boeing Demo
 *
 *  Created by WATCH on 10/6/09.
 *  Copyright 2009 Iowa State University. All rights reserved.
 *
 */

#include "LightsGroup.h"

LightsGroup::LightsGroup(osg::StateSet* modelsStateSet)
{
	_modelsStateSet = modelsStateSet;
}

void LightsGroup::init()
{
	//=================== Setup Light 1 ===================
	_light1 = new osg::Light();
	_light1->setLightNum(0);
	_light1->setPosition(osg::Vec4(-1.0, 1.0, -1.0, 0.0f));
	_light1->setAmbient(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	_light1->setDiffuse(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	_light1->setSpecular(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	
	// Create the light source
	osg::ref_ptr<osg::LightSource> lightSource1 = new osg::LightSource();
	lightSource1->setLight(_light1.get());
	lightSource1->setLocalStateSetModes(osg::StateAttribute::ON);
	lightSource1->setStateSetModes(*_modelsStateSet, osg::StateAttribute::ON);
	
	// Add the light source to the lights group
	this->addChild(lightSource1.get());
	
	//=================== Setup Light 2 ===================
	_light2 = new osg::Light();
	_light2->setLightNum(1);
	_light2->setPosition(osg::Vec4(1.0, 0.4, -1.0, 0.0f));
	_light2->setDiffuse(osg::Vec4(0.4f, 0.4f, 0.4f, 1.0f));
	_light2->setSpecular(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
	_light2->setAmbient(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
	
	// Create the light source
	osg::ref_ptr<osg::LightSource> lightSource2 = new osg::LightSource();
	lightSource2->setLight(_light2.get());
	lightSource2->setLocalStateSetModes(osg::StateAttribute::ON);
	lightSource2->setStateSetModes(*_modelsStateSet, osg::StateAttribute::ON);	
	
	// Add the light source to the lights group
	this->addChild(lightSource2.get());
	
	//=================== Setup Light 3 ===================
	_light3 = new osg::Light();
	_light3->setLightNum(2);
	_light3->setPosition(osg::Vec4(-1.0, -0.4, 1.0, 0.0f));
	_light3->setDiffuse(osg::Vec4(0.6f, 0.6f, 0.6f, 1.0f));
	_light3->setSpecular(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
	_light3->setAmbient(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
	
	// Create the light source
	osg::ref_ptr<osg::LightSource> lightSource3 = new osg::LightSource();
	lightSource3->setLight(_light3.get());
	lightSource3->setLocalStateSetModes(osg::StateAttribute::ON);
	lightSource3->setStateSetModes(*_modelsStateSet, osg::StateAttribute::ON);	
	
	// Add the light source to the lights group
	this->addChild(lightSource3.get());
}

void LightsGroup::updateLights(float totalTime)
{
	return;
	if (totalTime >= 1.0 && totalTime <= 1.5)
	{
		float totalFadeInTime = 0.5;
		float step = (totalTime - 1.0) / totalFadeInTime;

		_light1->setAmbient(osg::Vec4(step * 0.3f, step * 0.3f, step * 0.3f, 1.0f));
		_light1->setDiffuse(osg::Vec4(step * 1.0f, step * 1.0f, step * 1.0f, 1.0f));
		_light1->setSpecular(osg::Vec4(step * 1.0f, step * 1.0f, step * 1.0f, 1.0f));
		
		_light2->setDiffuse(osg::Vec4(step * 0.4f, step * 0.4f, step * 0.4f, 1.0f));
		_light2->setSpecular(osg::Vec4(step * 0.2f, step * 0.2f, step * 0.2f, 1.0f));
		_light2->setAmbient(osg::Vec4(step * 0.1f, step * 0.1f, step * 0.1f, 1.0f));
		
		_light3->setDiffuse(osg::Vec4(step * 0.6f, step * 0.6f, step * 0.6f, 1.0f));
		_light3->setSpecular(osg::Vec4(step * 0.2f, step * 0.2f, step * 0.2f, 1.0f));
		_light3->setAmbient(osg::Vec4(step * 0.1f, step * 0.1f, step * 0.1f, 1.0f));
	}
}
























