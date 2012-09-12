//
//  ControlCircleTrail.h
//  ExampleReceiver
//
//  Created by Jim on 9/12/12.
//
//

#pragma once
#include "ofMain.h"

class ControlCircleTrail {
  public:
	ControlCircleTrail(){
		birthTime = ofGetElapsedTimef();
		left = right = ofVec3f(0,0,0);
		color = ofColor(0,0,0);
	}
	
	ofColor color;
	float birthTime;
	float width;
	
	ofVec3f left;
	ofVec3f right;
	
};