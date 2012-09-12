//
//  ControlCircle.h
//  ExampleReceiver
//
//  Created by Jim on 9/2/12.
//
//

#pragma once

#include "ofMain.h"
#include "ControlCircleTrail.h"

class ControlCircle {
  public:
	ControlCircle();
	
	void update();
	void setPosition(ofVec3f pos);
	void setX(float x);
	void setY(float y);
	void setZ(float z);
	
	ofVec3f getPosition();
	ofVec3f getLastPosition();
	
	void setName(string name);
	string getName();
	
	ofColor getColor();
	ofColor getLastColor();
	
	void setColor(ofColor color);
	

	void bang();
	void setQuote(string quote);
	ControlCircleTrail lastTrail;
	
  protected:
	ofVec3f lastPosition;
	ofVec3f position;
	
	string currentQuote;
	float quoteReceivedTime;
	float lastBangTime;
	
	float lastTimeMoved;
	string name;
	ofColor color;
	ofColor lastColor;
};


