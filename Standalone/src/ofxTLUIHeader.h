
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
    virtual bool isOSCEnabled();
    virtual void setOSCEnabled(bool enableosc);
	
	float lastInputReceivedTime;
	ofxTLTrack* getTrack();
	ofxTLTrackHeader* getTrackHeader();
	
  protected:
    
    ofxUICanvas* gui;
	ofxUINumberDialer* minDialer;
	ofxUINumberDialer* maxDialer;
    ofxUIToggle* oscEnabledToggle;
    ofxTLTrackHeader* trackHeader;
    bool shouldDelete;
};