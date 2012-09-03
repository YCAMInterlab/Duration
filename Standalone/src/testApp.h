#pragma once

#include "ofMain.h"
//#include "ofxTimeline.h"
//#include "ofxOsc.h"
//#include "ofxTLUIHeader.h"
#include "DurationController.h"

#define DURATION_VERSION_STRING string("001")
#define DURATION_VERSION_INT 1

class testApp : public ofBaseApp{
  public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
	
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
	
	DurationController controller;
};
