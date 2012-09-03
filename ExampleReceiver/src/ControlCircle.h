//
//  ControlCircle.h
//  ExampleReceiver
//
//  Created by Jim on 9/2/12.
//
//

#pragma once

#include "ofMain.h"

class ControlCircle {
  public:
	ControlCircle();
	
	void update();
	void setPosition(ofVec2f pos);
	void setX(float x);
	void setY(float y);
	ofVec2f getPosition();
	
	void setName(string name);
	string getName();
	
  protected:
	
	ofVec2f position;
	float lastTimeMoved;
	string name;
};


