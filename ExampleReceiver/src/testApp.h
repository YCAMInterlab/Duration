#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ControlCircle.h"
#include "ControlCircleTrail.h"

// listen on port 12345
#define PORT 12348
#define NUM_MSG_STRINGS 20

class testApp : public ofBaseApp, public ofThread {
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
	void exit();

	void threadedFunction();
	ofTrueTypeFont font;
	ofFbo fbo;
	ofxOscReceiver receiver;

	vector<ControlCircleTrail> trails;
	map<string, ControlCircle> circles;
	bool hasCircleWithName(string name);
	ControlCircle& circleWithName(string name);

	float maxDistance;
	float minDistance;
	float maxWidth;
	float minWidth;
	
	bool doGraph;
	vector<float> xs;
	vector<float> ys;
};
