/*
 *  DeviceInputController.h
 *  Boeing Demo
 *
 *  Created by WATCH on 10/5/09.
 *  Copyright 2009 Iowa State University. All rights reserved.
 *
 */

#ifndef _DEVICEINPUTCONTROLLER_H_
#define _DEVICEINPUTCONTROLLER_H_

class DeviceInputController
{
public:
	// Constructor
	DeviceInputController() {;}
	
	// The four possible states of your standard button
	enum ButtonState { OFF, ON, TOGGLE_OFF, TOGGLE_ON };
	
	void buttonInput(int button, bool pressed);
	void update(double dt);
	
private:
	// Remove toggle status from our buttons after each frame 
	// to prevent events from happening repeatedly
	void _deToggleButtons();
	
	// Private variables
	static const int NUMBUTTONS = 11;
	int _buttons[NUMBUTTONS];
};

#endif



