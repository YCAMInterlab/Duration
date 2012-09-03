//
//  ControlCircle.cpp
//  ExampleReceiver
//
//  Created by Jim on 9/2/12.
//
//

#include "ControlCircle.h"

ControlCircle::ControlCircle(){
	position = ofVec2f(0,0);
}

void ControlCircle::update(){
	
}

ofVec2f ControlCircle::getPosition(){
	return position;
}

void ControlCircle::setPosition(ofVec2f pos){
	position = pos;
}

void ControlCircle::setX(float x){
	position.x = x;
}

void ControlCircle::setY(float y){
	position.y = y;
}

void ControlCircle::setName(string newName){
	name = newName;
}

string ControlCircle::getName(){
	return name;
}
