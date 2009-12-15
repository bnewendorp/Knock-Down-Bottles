/*
 *  LightsGroup.h
 *  Boeing Demo
 *
 *  Created by WATCH on 10/6/09.
 *  Copyright 2009 Iowa State University. All rights reserved.
 *
 */

#ifndef _LIGHTSGROUP_H_
#define _LIGHTSGROUP_H_

class LightsGroup : public osg::Group, public aq::KVObserver
{
public:
	// Constructor
	LightsGroup(osg::StateSet* modelsStateSet);
	
	// Setup and updates for all lights
	void init();
	void updateLights(float step);
	
private:
	// Private variables
	osg::ref_ptr<osg::Light> _light1;
	osg::ref_ptr<osg::Light> _light2;
	osg::ref_ptr<osg::Light> _light3;
	osg::ref_ptr<osg::StateSet> _modelsStateSet;
};

#endif


