//
//  DurationController.h
//  Duration
//
//  Duration is an application for time.
//  Made at YCAM InterLab
//
//

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxTimeline.h"
#include "ofxMSATimer.h"
#include "ofxTLUIHeader.h"
#include "ofxUI.h"
//#include "ofxTLAudioTrack.h"

typedef struct {
    string path; //full project path
    string name;
    string settingsPath;
    
    bool useBPM;
    float bpm;
    bool snapToBPM;
    bool snapToKeys;
    
    bool useOSC;
    string oscIP;
    int oscPort;
    
} DurationProjectSettings;

typedef struct{
	ofRectangle sourceRect;
	ofVec2f displayPoint;
	string text;
} ToolTip;

class DurationController : public ofThread  {
  public:
	DurationController();
	~DurationController();
	
	void setup();
	void update(ofEventArgs& args);
	void draw(ofEventArgs& args);
	
	void keyPressed(ofKeyEventArgs& keyArgs);
		
    ofRectangle canvasRectangle;
    ofxUICanvas* gui;
	void guiEvent(ofxUIEventArgs& e);
    void exit(ofEventArgs& e);
	
    //control elements
    ofxUIDropDownList* projectDropDown;
    ofxUILabel* timeLabel;
    //ofxUILabel* durationLabel;
    ofxUITextInput* durationLabel;
    ofxUILabel* bpmLabel;
    ofxUIMultiImageToggle* playpauseToggle;
	
	//project settings elements
    ofxUIMultiImageToggle* loopToggle;
    ofxUILabelToggle* useBPMToggle;
	ofxUINumberDialer* bpmDialer;
    ofxUILabelToggle* snapToBPMToggle;
    ofxUILabelToggle* snapToKeysToggle;
    
    ofxUILabelToggle* useOSCToggle;
    ofxUITextInput* oscIPInput;
    ofxUITextInput* oscPortInput;
    
    vector<string> trackTypes;
    ofxUIDropDownList* addTrackDropDown;
	
    string defaultProjectDirectoryPath;
	DurationProjectSettings settings;
	
    void newProject(string newProjectPath, string newProjectName);
    void loadProject(string projectPath, bool forceCreate = false);
    void loadProject(string projectPath, string projectName, bool forceCreate = false);
    void saveProject();
    
    DurationProjectSettings defaultProjectSettings();
    	
	void startRecording();
	void stopRecording();

  protected:
	ofxTimeline timeline;
    void bangFired(ofxTLBangEventArgs& bang);

	vector<string> trackAddresses;
	
    bool shouldCreateNewProject;
    bool shouldLoadProject;
	
	bool recordingIsEnabled;
	bool oscIsEnabled;
	
	ofxOscReceiver receiver;
	ofxOscSender sender;
	
	void threadedFunction();
	void handleOscOut();
	void handleOscIn();
	unsigned long recordTimeOffset;
	ofxMSATimer recordTimer;
	
	//only can have one of these!
//	ofxTLAudioTrack* audioTrack;
	
	vector<ofxOscMessage> bangsReceived;
	map<string, ofPtr<ofxTLUIHeader> > headers;
    
	ofxTLUIHeader* createHeaderForTrack(ofxTLTrack* track);
	
	float oscRate;
	unsigned long lastOSCBundleSent;
};

