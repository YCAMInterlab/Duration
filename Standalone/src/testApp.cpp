#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
    //setup OF
//    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    ofBackground(.15*255);
    ofEnableAlphaBlending();
    ofEnableSmoothing();

	controller.setup();
	
	/*
    shouldCreateNewProject = false;
    shouldLoadProject = false;
    
    //populate projects
    vector<string> projects;
    projects.push_back(NEW_PROJECT_TEXT);
    projects.push_back(OPEN_PROJECT_TEXT);
    
    defaultProjectDirectoryPath = ofToDataPath(ofFilePath::getUserHomeDir() + "/Documents/Duration/");
    ofDirectory projectDirectory = ofDirectory(defaultProjectDirectoryPath);

    if(!projectDirectory.exists()){
        projectDirectory.create();
    }
    
    projectDirectory.listDir();
    for(int i = 0; i < projectDirectory.size(); i++){
        if(projectDirectory.getFile(i).isDirectory()){
            ofDirectory subDir = ofDirectory(projectDirectory.getPath(i));
//            cout << "checking path " << projectDirectory.getPath(i) << endl;
            subDir.allowExt("durationproj");
            subDir.setShowHidden(true);
            subDir.listDir();
            if(subDir.size() > 0){
                projects.push_back(projectDirectory.getName(i));
            }
        }
    }
    
    
    //Set up top GUI
    gui = new ofxUICanvas(0,0,ofGetWidth(), 75);     
    
    //ADD PROJECT DROP DOWN
    projectDropDown = new ofxUIDropDownList(DROP_DOWN_WIDTH, "PROJECT", projects, OFX_UI_FONT_LARGE);
    projectDropDown->setAutoClose(true);
    gui->addWidgetDown(projectDropDown);
    ofxUIMultiImageButton* saveButton = new ofxUIMultiImageButton(32, 32, false, "GUI/save_.png", "SAVE");
    saveButton->setLabelVisible(false);
    gui->addWidgetRight(saveButton);
    
    //ADD TIMECODE
    string zeroTimecode = "00:00:00:000";
    timeLabel = new ofxUILabel(zeroTimecode, OFX_UI_FONT_LARGE);
    gui->addWidgetRight(timeLabel); 
	//durationLabel = new ofxUILabel(" / "+zeroTimecode, OFX_UI_FONT_SMALL);
    durationLabel = new ofxUITextInput("DURATION", zeroTimecode, timeLabel->getRect()->width,0,0,0, OFX_UI_FONT_SMALL);
    durationLabel->setAutoClear(false);
    gui->addWidgetSouthOf(durationLabel, zeroTimecode);
    
    //ADD PLAY/PAUSE
    playpauseToggle = new ofxUIMultiImageToggle(32, 32, false, "GUI/play_.png", "PLAYPAUSE");
    playpauseToggle->setLabelVisible(false);
    gui->addWidgetEastOf(playpauseToggle, zeroTimecode);
    ofxUIMultiImageButton* stopButton = new ofxUIMultiImageButton(32, 32, false, "GUI/stop_.png", "STOP");
    stopButton->setLabelVisible(false);
    gui->addWidgetRight(stopButton);
    gui->addWidgetRight(loopToggle = new ofxUIMultiImageToggle(32, 32, false, "GUI/loop_.png", "LOOP"));  
	loopToggle->setLabelVisible(false);
    
    //ADD TRACKS
    vector<string> trackTypes; 
    trackTypes.push_back("BANGS");    
    trackTypes.push_back("FLAGS");    
    trackTypes.push_back("SWITCHES");    
    trackTypes.push_back("CURVES");    
    
    addTrackDropDown = new ofxUIDropDownList(DROP_DOWN_WIDTH, "ADD TRACK", trackTypes, OFX_UI_FONT_MEDIUM); 
    addTrackDropDown->setAllowMultiple(false);
    addTrackDropDown->setAutoClose(true);
    gui->addWidgetRight(addTrackDropDown);
    
    //SETUP BPM CONTROLS

	useBPMToggle = new ofxUILabelToggle(false, "BPM", OFX_UI_FONT_MEDIUM);
    gui->addWidgetEastOf(useBPMToggle, "ADD TRACK");     
	bpmDialer = new ofxUINumberDialer(0., 250., 120., 2, "BPM_VALUE", OFX_UI_FONT_MEDIUM);
    gui->addWidgetEastOf(bpmDialer, "BPM"); 

    //figure out where to put these
    snapToBPMToggle = new ofxUILabelToggle(false, "Snap to BPM", OFX_UI_FONT_SMALL);
//    gui->addWidgetSouthOf(snapToBPM, "BPM"); 
    snapToKeysToggle = new ofxUILabelToggle(false, "Snap to Keys", OFX_UI_FONT_SMALL);
//    gui->addWidgetRight(snapToKeys); 
    
    //SETUP OSC CONTROLS
    useOSCToggle = new ofxUILabelToggle(false, "OSC", OFX_UI_FONT_MEDIUM);
    oscIPInput = new ofxUITextInput("OSCIP", "127.0.0.1",TEXT_INPUT_WIDTH,0,0,0, OFX_UI_FONT_MEDIUM);
    oscIPInput->setAutoClear(false);
    oscPortInput = new ofxUITextInput("OSCPORT", "12345",TEXT_INPUT_WIDTH,0,0,0, OFX_UI_FONT_MEDIUM);
    oscPortInput->setAutoClear(false);
    
    gui->addWidgetEastOf(useOSCToggle, "BPM_VALUE");
    gui->addWidgetRight(oscIPInput);
    gui->addWidgetRight(oscPortInput);
    
    //SET UP LISENTERS
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);	
    
    //setup timeline
	timeline.setup();
    timeline.setFrameRate(30);
	timeline.setDurationInSeconds(30);
	timeline.setOffset(ofVec2f(0, 75));
    timeline.getColors().load("defaultColors.xml");
    timeline.setBPM(120.f);
	timeline.moveToThread(); //increases accuracy of bang call backs
	
    ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);

    ofxXmlSettings defaultSettings;
    if(defaultSettings.loadFile("settings.xml")){
        string lastProjectPath = defaultSettings.getValue("lastProjectPath", "");
        string lastProjectName = defaultSettings.getValue("lastProjectName", "");
        if(lastProjectPath != "" && lastProjectName != "" && ofDirectory(lastProjectPath).exists()){
            loadProject(lastProjectPath, lastProjectName);
        }
        else{
            ofLogError() << "Duration -- Last project was not found, creating a new project";
            loadProject(ofToDataPath(defaultProjectDirectoryPath+"/Sample Project"), "Sample Project", true);
        }
    }
    else {
        cout << "Loading sample project " << defaultProjectDirectoryPath << endl;
        loadProject(ofToDataPath(defaultProjectDirectoryPath+"/Sample Project"), "Sample Project", true);
    }
	*/
}

void testApp::update(){
	
}

void testApp::draw(){
	
}


/*
//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& bang){
// 	ofLogNotice() << "Bang from " << bang.track->getDisplayName() << " at time " << bang.currentTime << " with flag " << bang.flag;
//
//    string trackType = bang.track->getTrackType();
//    if(!headers[bang.track->getName()]->isOSCEnabled()){
//        return;
//    }
//    ofxOscMessage m;
//    m.setAddress("/duration/" + DURATION_VERSION_STRING + "/" + ofToLower(trackType) + "/" + bang.track->getDisplayName());
//    m.addIntArg(bang.currentMillis);
//    if(trackType == "Flags"){
//        m.addStringArg(bang.flag);
//    }
//	bangsReceived.push_back(m);
}
*/

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
