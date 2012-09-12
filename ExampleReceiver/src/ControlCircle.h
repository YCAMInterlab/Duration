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
	void setPosition(ofVec2f pos);
	void setX(float x);
	void setY(float y);
	
	ofVec2f getPosition();
	ofVec2f getLastPosition();
	
	void setName(string name);
	string getName();
	
	ofColor getColor();
	ofColor getLastColor();
	
	void setColor(ofColor color);
	

	void bang();
	void setQuote(string quote);
	ControlCircleTrail lastTrail;
	
  protected:
	ofVec2f lastPosition;
	string currentQuote;
	float quoteReceivedTime;
	float lastBangTime;
	
	ofVec2f position;
	float lastTimeMoved;
	string name;
	ofColor color;
	ofColor lastColor;
};


