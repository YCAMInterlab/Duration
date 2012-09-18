#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 12346

//--------------------------------------------------------
class testApp : public ofBaseApp {

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

	ofTrueTypeFont font;
	ofxOscSender sender;

	ofRectangle sineWave;
	ofRectangle freeDraw;
	ofRectangle clickBang;
	ofRectangle noise;
	
	float currentSineValue;
	float currentNoiseValue;
	float currentSinePosition; //progresses linearly
	float currentNoisePosition; //progresses linearly
	
	ofVec2f currentFreeDrawCursor;
	ofVec2f currentNoiseCursor;
	ofVec2f currentSineCursor;
	
	bool sendBang;
	float bangTime;
	ofColor redColor;
	ofColor blueColor;
	
	//preview buffers
	vector<float> sinbuf;
	vector<float> noisebuf;
	vector<ofVec2f> freedrawbuf;
	
	void updateCursorPoint(ofVec2f mousePoint, ofRectangle rect, ofVec2f& cursor);
	float lastTime;
};

