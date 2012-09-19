#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxOsc.h"

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
	
	void guiEvent(ofxUIEventArgs &e);
	void sendCurrentCommand();
	void clearArgs();
	
	ofColor redColor;
	ofColor blueColor;
	float seekPercent;
	
	ofxOscSender sender;

	ofxUICanvas* gui;
	
	//ROW 0 IP settings
	ofxUITextInput* ipInput;
	ofxUITextInput* portInput;
	
	//ROW 1 commands
	ofxUITextInput* commandInput;
	ofxUITextInput* arg1;
	ofxUITextInput* arg2;
	ofxUITextInput* arg3;
	ofxUILabelButton* sendButton;

	//ROW 2 time and playback
	ofxUILabelButton* playButton;
	ofxUILabelButton* stopButton;
	ofxUILabelButton* setDurationButton;
	ofxUITextInput* durationInput;

	//ROW 3 add track
	ofxUIDropDownList* addTrackDropdown;
	ofxUITextInput* addTrackNameInput;
	ofxUITextInput* filePathInput;
	ofxUILabelButton* addTrackButton;

	//ROW 4 configure track
	
	//ROW 5 remove track
	ofxUITextInput* removeTrackNameInput;
	ofxUILabelButton* removeTrackButton;

	
	ofRectangle seekRect;
	bool hoveringSeek;
	bool draggingSeek;
};
