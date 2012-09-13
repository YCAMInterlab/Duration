#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxOsc.h"
#include "ofxUI.h"

class testApp : public ofBaseApp {
  public:
	
	void setup();
	void update();
	void draw();
	void exit();

	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	void guiEvent(ofxUIEventArgs &e);

	ofxUICanvas* ui;
	ofxOscSender sender;
	
	ofxKinect* kinect;
		
	ofxCvColorImage colorImg;
	ofxUITextInput* ip;
	ofxUITextInput* port;
	string currentIp;
	int currentPort;
	
	void setupOSC();
	
	bool shouldCaptureBackground;
	ofxCvGrayscaleImage background;
	ofxCvGrayscaleImage subtracted;
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	
	float nearThreshold;
	float farThreshold;
	
	int angle;
	ofRectangle drawingRect;
	bool drawCursor;
	vector<ofVec2f> sentPoints;
	ofRectangle kinectRect;
};
