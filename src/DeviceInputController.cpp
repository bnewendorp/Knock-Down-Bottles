/*
 *  DeviceInputController.cpp
 *  Boeing Demo
 *
 *  Created by WATCH on 10/5/09.
 *  Copyright 2009 Iowa State University. All rights reserved.
 *
 */

#include "DeviceInputController.h"

void DeviceInputController::buttonInput(int button, bool pressed)
{
	if (button < NUMBUTTONS)
		_buttons[button] = pressed ? TOGGLE_ON : TOGGLE_OFF;
}

void DeviceInputController::update(double dt)
{
	// Process device inputs
	if (_buttons[0] == ON)
		aq::KVReflector::instance()->didUpdateValueForKey(dt, "Update_Wand_Matrix");
	if (_buttons[1] == TOGGLE_ON)
		printf("Empty right now\n");
	if (_buttons[2] == TOGGLE_ON)
		printf("Start the plane animation\n");
	if (_buttons[3] == TOGGLE_ON)
		printf("Empty right now\n");
	if (_buttons[4] == TOGGLE_ON)
		printf("Empty right now\n");
	if (_buttons[5] == TOGGLE_ON)
		aq::KVReflector::instance()->didUpdateValueForKey((double)-1.0, "Decrease_Nav_Speed");
	if (_buttons[6] == TOGGLE_ON)
		aq::KVReflector::instance()->didUpdateValueForKey((double)-1.0, "Increase_Nav_Speed");
	if (_buttons[7] == TOGGLE_ON)
		printf("Empty right now\n");
	if (_buttons[8] == TOGGLE_ON)
		printf("Empty right now\n");
	if (_buttons[9] == TOGGLE_ON)
		printf("Empty right now\n");
	if (_buttons[10] == TOGGLE_ON)
		printf("Empty right now\n");
		
	// Detoggle the button (it's important that this be called every frame)
	_deToggleButtons();
}

void DeviceInputController::_deToggleButtons()
{
	// Remove any toggleness from the buttons. This is important if we want 
	// to attach events to the moment a button is pressed
	for (int i = 0; i < NUMBUTTONS; i++)
	{
		if (_buttons[i] == TOGGLE_ON)
			_buttons[i] = ON;
		else if (_buttons[i] == TOGGLE_OFF)
			_buttons[i] = OFF;
	}
}
















