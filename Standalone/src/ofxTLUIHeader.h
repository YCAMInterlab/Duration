
#pragma once
#include "ofMain.h"

#include "ofxTimeline.h"
#include "ofxUI.h"

class ofxTLUIHeader {
  public:
    ofxTLUIHeader();
    virtual ~ofxTLUIHeader();
    
    void setTrackHeader(ofxTLTrackHeader* trackHeader);
	virtual void viewWasResized(ofEventArgs& args); 
    virtual void guiEvent(ofxUIEventArgs &e);
    
	virtual bool getShouldDelete();
	
    virtual bool sendOSC();
    virtual void setSendOSC(bool enable);
    virtual bool receiveOSC();
    virtual void setReceiveOSC(bool enable);

	//just for drawing the red receiver background
	float lastInputReceivedTime;
	
	//remove duplicate sending and receiving
	bool hasReceivedValue;
	bool hasSentValue;
	
	//different value types for tracking last values sent over OSC
	float lastFloatSent;
	bool lastBoolSent;
	ofColor lastColorSent;
	
	//only receiving floats for now
	float lastValueReceived;
	
	ofxTLTrack* getTrack();
	ofxTLTrackHeader* getTrackHeader();
	
  protected:
    
    ofxUICanvas* gui;
    ofxTLTrackHeader* trackHeader;
	ofxUINumberDialer* minDialer;
	ofxUINumberDialer* maxDialer;
	//Delay dialer?
	
    ofxUIToggle* sendOSCEnable;
	ofxUIToggle* receiveOSCEnable;
	
    bool shouldDelete;
};