#pragma once

#include "ofMain.h"
#include "ofxTimeline.h"
#include "ofxUI.h"

typedef struct {
    string path; //full project path
    string name;
    
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
	
    ofRectangle canvasRectangle;
    ofxUICanvas* gui;   	
	void guiEvent(ofxUIEventArgs &e);
    
    //control elements
    ofxUIDropDownList* projectDropDown; 
    ofxUILabel* timeLabel;
    ofxUILabel* durationLabel;
    ofxUILabel* bpmLabel;
    ofxUIMultiImageToggle* playpauseToggle;

	//project settings elements
    ofxUIImageToggle* loopToggle;
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

    void newProject(string newProjectName, string newProjectPath);
    void loadProject(string projectName, string projectDirectory);    
    void saveProject();
    
    DurationProjectSettings defaultProjectSettings();
    
    bool shouldCreateNewProject;
	string createNewProjectName;
    string createNewProjectPath;
    
    bool shouldLoadProject;
    string loadNewProjectName;
    string loadNewProjectPath;

};
