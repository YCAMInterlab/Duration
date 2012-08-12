
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

    
  protected:
    
    ofxUICanvas* gui;
    ofxTLTrackHeader* trackHeader;
    
};