//
//  ControlCircle.cpp
//  ExampleReceiver
//
//  Created by Jim on 9/2/12.
//
//

#include "ControlCircle.h"

ControlCircle::ControlCircle(){
	position = lastPosition = ofVec3f(0,0);
	color = ofColor(200, 200, 50);
	lastMessageReceived = ofGetElapsedTimef();
	receivedbang = true;
}

void ControlCircle::update(){
	lastPosition = position;
	lastColor = color;
}

ofVec3f ControlCircle::getLastPosition(){
	return lastPosition;
}

ofColor ControlCircle::getLastColor(){
	return lastColor;
}

ofVec3f ControlCircle::getPosition(){
	return position;
}

void ControlCircle::setPosition(ofVec3f pos){
	position = pos;
}

void ControlCircle::setX(float x){
	position.x = x;
}

void ControlCircle::setY(float y){
	position.y = y;
}

void ControlCircle::setZ(float z){
	position.z = z;
}

void ControlCircle::setName(string newName){
	name = newName;
}

string ControlCircle::getName(){
	return name;
}

ofColor ControlCircle::getColor(){
	return color;
}

void ControlCircle::setColor(ofColor newColor){
	color = newColor;
}

bool ControlCircle::didBang(){
	bool b = receivedbang;
	receivedbang = false;
	return b;
}

void ControlCircle::bang(){
	receivedbang = true;
	lastBangTime = ofGetElapsedTimef();
}

void ControlCircle::setQuote(string quote){
	quoteReceivedTime = ofGetElapsedTimef();
	currentQuote = quote;
}
