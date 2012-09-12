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
		startLeft = startRight = ofVec2f(0,0);
		endLeft = endRight = ofVec2f(0,0);
		startWidth = 0;
		endWidth = 0;
		lastColor = firstColor = ofColor(0,0,0);
	}
	
	ofColor firstColor;
	ofColor lastColor;
	float birthTime;
	
	ofVec2f startLeft;
	ofVec2f startRight;
	ofVec2f endLeft;
	ofVec2f endRight;
	
	float startWidth;
	float endWidth;
};