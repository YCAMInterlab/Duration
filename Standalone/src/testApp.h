#pragma once

#include "ofMain.h"
#include "ofxTimeline.h"
#include "ofxUI.h"
#include "ofxOsc.h"
#include "ofxTLUIHeader.h"

#define DURATION_VERSION_STRING string("001")
#define DURATION_VERSION_INT 1

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

	ofxTimeline timeline;
    void bangFired(ofxTLBangEventArgs& bang);
	
    ofxOscSender sender;
    
    ofRectangle canvasRectangle;
    ofxUICanvas* gui;   	
	void guiEvent(ofxUIEventArgs &e);
    
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
    void loadProject(string projectPath, string projectName);
    void saveProject();
    
    DurationProjectSettings defaultProjectSettings();
    
    bool shouldCreateNewProject;    
    bool shouldLoadProject;

    vector<ofxOscMessage> bangsReceived;
	map<string, ofPtr<ofxTLUIHeader> > headers;
    
	ofxTLUIHeader* createHeaderForTrack(ofxTLTrack* track);
    
};
