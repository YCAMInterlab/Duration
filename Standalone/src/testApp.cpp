#include "testApp.h"


#define DROP_DOWN_WIDTH 200
#define TEXT_INPUT_WIDTH 100

#define NEW_PROJECT_TEXT "new project..."
#define OPEN_PROJECT_TEXT "open project..."

//--------------------------------------------------------------
void testApp::setup(){
	
    //setup OF
    ofSetFrameRate(120);
    ofBackground(.15*255);
    ofEnableAlphaBlending();
    ofEnableSmoothing();

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
    gui->addWidgetRight(new ofxUIImageButton(32, 32, false, "GUI/save.png", "SAVE"));
    
    //ADD TIMECODE
    string zeroTimecode = "00:00:00:000";
    timeLabel = new ofxUILabel(zeroTimecode, OFX_UI_FONT_LARGE);
    gui->addWidgetRight(timeLabel); 
	//durationLabel = new ofxUILabel(" / "+zeroTimecode, OFX_UI_FONT_SMALL);
    durationLabel = new ofxUITextInput("DURATION", zeroTimecode, timeLabel->getRect()->width,0,0,0, OFX_UI_FONT_SMALL);
    gui->addWidgetSouthOf(durationLabel, zeroTimecode);
    
    //ADD PLAY/PAUSE
    playpauseToggle = new ofxUIMultiImageToggle(32, 32, false, "GUI/play_.png", "PLAYPAUSE");
    playpauseToggle->setLabelVisible(false);
    gui->addWidgetEastOf(playpauseToggle, zeroTimecode); 
    gui->addWidgetRight(new ofxUIImageButton(32, 32, false, "GUI/stop.png", "STOP"));  
    gui->addWidgetRight(loopToggle = new ofxUIImageToggle(32, 32, false, "GUI/loop.png", "LOOP"));  
	
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
    oscPortInput = new ofxUITextInput("OSCPORT", "12345",TEXT_INPUT_WIDTH,0,0,0, OFX_UI_FONT_MEDIUM);

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


    ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);

    ofxXmlSettings defaultSettings;
    if(defaultSettings.loadFile("settings.xml")){
        string lastProjectPath = defaultSettings.getValue("lastProjectPath", "");
        string lastProjectName = defaultSettings.getValue("lastProjectName", "");
        cout << "loading last project " << lastProjectPath << " and name " << lastProjectName << endl;
        if(lastProjectPath != "" && lastProjectName != "" && ofDirectory(lastProjectPath).exists()){
            loadProject(lastProjectPath, lastProjectName);
        }
    }
    else {
        cout << "Loading sample project " << defaultProjectDirectoryPath << endl;
        loadProject(ofToDataPath(defaultProjectDirectoryPath+"/Sample Project"), "Sample Project");
    }
}

//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& bang){
 	ofLogNotice() << "Bang Fired from track " << bang.track->getDisplayName() << " at time " << bang.currentTime << " with flag " << bang.flag;

    string trackType = bang.track->getTrackType();
    ofxOscMessage m;
    m.setAddress("/duration/" + DURATION_VERSION_STRING + "/" + ofToLower(trackType) + "/" + bang.track->getDisplayName());
    m.addIntArg(bang.currentMillis);
    if(trackType == "Flags"){
        m.addStringArg(bang.flag);
    }
	bangsReceived.push_back(m);
}

//--------------------------------------------------------------
void  testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
    
	cout << "name is " << name << " kind is " << kind << endl;    
    
	if(name == "STOP"){
        timeline.stop();
        timeline.setCurrentTimeMillis(0);
    }
    else if(name == "PLAYPAUSE"){
        timeline.togglePlay();
    }
    else if(name == "DURATION"){
        string newDuration = durationLabel->getTextString();
        timeline.setDurationInTimecode(newDuration);
        durationLabel->setTextString(timeline.getDurationInTimecode());
    }
    else if(e.widget == addTrackDropDown){
        if(addTrackDropDown->isOpen()){
            timeline.disable();
        }
        else {
            timeline.enable();
            if(addTrackDropDown->getSelected().size() > 0){
                string selectedTrackType = addTrackDropDown->getSelected()[0]->getName();
                ofxTLTrack* newTrack = NULL;
                if(selectedTrackType == "BANGS"){
                    string name = timeline.confirmedUniqueName("Bangs");
                    string xmlFile = ofToDataPath(settings.path + "/" + name + "_.xml");
                    newTrack = timeline.addBangs(name, xmlFile);
                }
                else if(selectedTrackType == "FLAGS"){
                	string name = timeline.confirmedUniqueName("Flags");
                    string xmlFile = ofToDataPath(settings.path + "/" + name + "_.xml");
                    newTrack = timeline.addFlags(name, xmlFile);                
                }
                else if(selectedTrackType == "CURVES"){
                	string name = timeline.confirmedUniqueName("Curves");
                    string xmlFile = ofToDataPath(settings.path + "/" + name + "_.xml");
                    newTrack = timeline.addKeyframes(name, xmlFile);                                
                }
                else if(selectedTrackType == "SWITCHES"){
                	string name = timeline.confirmedUniqueName("Switches");
                    string xmlFile = ofToDataPath(settings.path + "/" + name + "_.xml");
                    newTrack = timeline.addSwitcher(name, xmlFile);                
                }
                
                if(newTrack != NULL){
                    createHeaderForTrack(newTrack);
                }
                addTrackDropDown->clearSelected();
            }
        }
    }
    else if(e.widget == projectDropDown){
        if(projectDropDown->isOpen()){
            timeline.disable();
        }
		else {
            timeline.enable();
            if(projectDropDown->getSelected().size() > 0){
                string selectedProjectName = projectDropDown->getSelected()[0]->getName();
                if(selectedProjectName == NEW_PROJECT_TEXT){
                    shouldCreateNewProject = true;
                }
                else if(selectedProjectName == OPEN_PROJECT_TEXT){
                    shouldLoadProject = true;
                }
                else {
                    loadProject(ofToDataPath(defaultProjectDirectoryPath+"/"+selectedProjectName), selectedProjectName);
                }
                projectDropDown->clearSelected();
            }
        }
    }
    else if(name == "SAVE"){
        saveProject();
    }
    //LOOP
    else if(e.widget == loopToggle){
        timeline.setLoopType(loopToggle->getValue() ? OF_LOOP_NORMAL : OF_LOOP_NONE);
    }
    //BPM
	else if(e.widget == bpmDialer){        
    	timeline.setBPM(settings.bpm = bpmDialer->getValue());   
	}
    else if(e.widget == useBPMToggle){
        settings.useBPM = useBPMToggle->getValue();
        timeline.setShowBPMGrid(settings.useBPM);
        timeline.enableSnapToBPM(settings.useBPM);
    }
    //OSC
    else if(e.widget == useOSCToggle){
		settings.useOSC = useOSCToggle->getValue();
        if(settings.useOSC){
            //TODO validate address
            sender.setup(settings.oscIP, settings.oscPort);
        }
    }
    else if(e.widget == oscIPInput){
        string newIP = oscIPInput->getTextString();
        if(newIP == settings.oscIP){
            return;
        }
        vector<string> ipComponents = ofSplitString(newIP, ".");
        bool valid = false;
        if(ipComponents.size() == 4){
			for(int i = 0; i < 4; i++){
                int component = ofToInt(ipComponents[i]);
                if (component < 0 || component > 255){
                    valid = false;
                    break;
                }
            }
            if(valid){
                settings.oscIP = newIP;
                sender.setup(settings.oscIP, settings.oscPort);                
            }
            else{
				oscIPInput->setTextString(settings.oscIP);
            }
        }
    }
    else if(e.widget == oscPortInput){
        int newPort = ofToInt(oscPortInput->getTextString());
        if(newPort != settings.oscPort && newPort > 0 && newPort < 65535){
	        sender.setup(settings.oscIP, newPort);
            settings.oscPort = newPort;
        }
        else {
            oscPortInput->setTextString( ofToString(settings.oscPort) );
        }
    }
}

//--------------------------------------------------------------
void testApp::update(){
    
	timeLabel->setLabel(timeline.getCurrentTimecode());
    
    if(shouldLoadProject){
        shouldLoadProject = false;
        ofFileDialogResult r = ofSystemLoadDialog("Load Project", true);
        if(r.bSuccess){
	        loadProject(r.getPath(), r.getName());    
        }        
    }
    
    if(shouldCreateNewProject){
        shouldCreateNewProject = false;
        ofFileDialogResult r = ofSystemSaveDialog("New Project", "NewDuration");
        if(r.bSuccess){
            newProject(r.getPath(), r.getName());
        }
    }
    
    if(timeline.getIsPlaying()){
        if(settings.useOSC){
            int numMessages = 0;
            ofxOscBundle bundle;
			string addressPrefix = "/duration/" + DURATION_VERSION_STRING + "/";
            vector<ofxTLPage*>& pages = timeline.getPages();
            for(int i = 0; i < pages.size(); i++){
            	vector<ofxTLTrack*> tracks = pages[i]->getTracks();
                for(int t = 0; t < tracks.size(); t++){
                    string trackType = tracks[t]->getTrackType();
                    if(trackType == "Tweens" || trackType == "Switches"){
        	            ofxOscMessage m;
	                    m.setAddress(addressPrefix + ofToLower(trackType) + "/" + tracks[t]->getDisplayName());
    	                m.addIntArg(timeline.getCurrentTimeMillis());
						if(trackType == "Tweens"){
                            ofxTLTweener* tweens = (ofxTLTweener*)tracks[t];
							m.addFloatArg(tweens->getValueAtTimeInMillis(timeline.getCurrentTimeMillis()));
                            m.addFloatArg(tweens->getValueRange().min);
                            m.addFloatArg(tweens->getValueRange().max);
                		}
                        else if(trackType == "Switches"){
                            ofxTLSwitcher* switches = (ofxTLSwitcher*)tracks[t];
                            m.addIntArg(switches->isOnAtMillis(timeline.getCurrentTimeMillis()));
                        }
                        bundle.addMessage(m);
                        numMessages++;
                    }        
                }                
            }
            
            //any bangs that came our way this frame send them out too
            for(int i = 0; i < bangsReceived.size(); i++){
                bundle.addMessage(bangsReceived[i]);
            }
            numMessages += bangsReceived.size();
            if(numMessages > 0){
	            sender.sendBundle(bundle);
            }
        	bangsReceived.clear();
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    timeline.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(timeline.isModal()){
        return;
    }
    
	if(key == ' '){
        timeline.togglePlay();
    }
    
    if(key == 'i'){
        timeline.setInPointAtPlayhead();
    }
    
    if(key == 'o'){
        timeline.setOutPointAtPlayhead();
    }
}

DurationProjectSettings testApp::defaultProjectSettings(){
    DurationProjectSettings settings;
    
    settings.name = "newProject";
    settings.path = defaultProjectDirectoryPath + settings.name;
    
    settings.useBPM = false;
    settings.bpm = 120.0f;
    settings.snapToBPM = false;
    settings.snapToKeys = true;;
    
    settings.useOSC = true;;
    settings.oscIP = "127.0.0.1";
    settings.oscPort = 12345;
    return settings;

}
//--------------------------------------------------------------
void testApp::newProject(string newProjectPath, string newProjectName){
    DurationProjectSettings newProjectSettings = defaultProjectSettings();
    newProjectSettings.name = newProjectName;
    newProjectSettings.path = ofToDataPath(newProjectPath);
    newProjectSettings.settingsPath = ofToDataPath(newProjectSettings.path + "/.durationproj");
    
    ofDirectory newProjectDirectory(newProjectSettings.path);
    if(newProjectDirectory.exists()){
    	ofSystemAlertDialog("The folder \"" + newProjectName + "\" already exists.");
        return;
    }
    if(!newProjectDirectory.create()){
    	ofSystemAlertDialog("The folder \"" + newProjectSettings.path + "\" could not be created.");        
        return;
    }
    
    //TODO: prompt to save existing project
    settings = newProjectSettings;

    timeline.reset();

    //saves file with default settings to new directory
    saveProject();
    
    loadProject(settings.path, settings.name);
}

//--------------------------------------------------------------
void testApp::loadProject(string projectPath, string projectName){
    
    ofxXmlSettings projectSettings;
    if(!projectSettings.loadFile(ofToDataPath(projectPath+"/.durationproj"))){
    	ofLogError() << " failed to load project " << ofToDataPath(projectPath+"/.durationproj") << endl;
        return;
    }

    timeline.setWorkingFolder(projectPath);
    timeline.reset();

    cout << "successfully loaded project " << projectPath << endl;
    //LOAD ALL TRACKS
    projectSettings.pushTag("tracks");
    int numPages = projectSettings.getNumTags("page");
    for(int p = 0; p < numPages; p++){
        projectSettings.pushTag("page", p);
        string pageName = projectSettings.getValue("name", "defaultPage");
        if(p == 0){
            timeline.setPageName(pageName, 0);
        }
        else{
            timeline.addPage(pageName, true);
        }

        int numTracks = projectSettings.getNumTags("track");
        for(int i = 0; i < numTracks; i++){
            projectSettings.pushTag("track", i);
            string trackType = projectSettings.getValue("type", "");
            ofxTLTrack* newTrack = NULL;
            string xmlFileName = projectSettings.getValue("xmlFileName", "");
            string trackName = projectSettings.getValue("trackName","");
            string trackFilePath = ofToDataPath(projectPath + "/" + xmlFileName);

            if(trackType == "Bangs"){
                newTrack = timeline.addBangs(trackName, trackFilePath);
            }
            else if(trackType == "Flags"){
                newTrack = timeline.addFlags(trackName, trackFilePath);                
            }
            else if(trackType == "Tweens"){
                newTrack = timeline.addKeyframes(trackName, trackFilePath);                                
            }
            else if(trackType == "Switches"){
                newTrack = timeline.addSwitcher(trackName, trackFilePath);                
            }        
            if(newTrack != NULL){
                string displayName = projectSettings.getValue("displayName","");
                if(displayName != ""){
	                newTrack->setDisplayName(displayName);
                }
                createHeaderForTrack(newTrack);
            }
            projectSettings.popTag(); //track
        }
        projectSettings.popTag(); //page
    }
    timeline.setCurrentPage(0);
    projectSettings.popTag(); //tracks
    
    //LOAD OTHER SETTINGS
    projectSettings.pushTag("timelineSettings");
    timeline.setDurationInTimecode(projectSettings.getValue("duration", "00:00:00:000"));
    timeline.setCurrentTimecode(projectSettings.getValue("playhead", "00:00:00:000"));
    timeline.setInPointAtTimecode(projectSettings.getValue("inpoint", "00:00:00:000"));
    timeline.setOutPointAtTimecode(projectSettings.getValue("outpoint", "00:00:00:000"));
    bool loops = projectSettings.getValue("loop", true);
    timeline.setLoopType(loops ? OF_LOOP_NORMAL : OF_LOOP_NONE);

    durationLabel->setTextString(timeline.getDurationInTimecode());
    loopToggle->setValue( loops );
    projectSettings.popTag(); //timeline settings;

    DurationProjectSettings newSettings;
    projectSettings.pushTag("projectSettings");
    
    useBPMToggle->setValue( newSettings.useBPM = projectSettings.getValue("useBPM", true) );
    bpmDialer->setValue( newSettings.bpm = projectSettings.getValue("bpm", 120.0f) );
    snapToBPMToggle->setValue( newSettings.snapToBPM = projectSettings.getValue("snapToBPM", true) );
    snapToKeysToggle->setValue( newSettings.snapToKeys = projectSettings.getValue("snapToKeys", true) );
    useOSCToggle->setValue( newSettings.useOSC = projectSettings.getValue("useOSC", true) );
    oscIPInput->setTextString( newSettings.oscIP = projectSettings.getValue("OSCIP", "127.0.0.1") );        
    newSettings.oscPort = projectSettings.getValue("OSCPort", 12345);        
    oscPortInput->setTextString( ofToString(newSettings.oscPort) );
    projectSettings.popTag(); //project settings;

    newSettings.path = projectPath;
    newSettings.name = projectName;
    newSettings.settingsPath = ofToDataPath(newSettings.path + "/.durationproj");
    settings = newSettings;

    projectDropDown->setLabelText(projectName);
    timeline.setShowBPMGrid(newSettings.useBPM);
    timeline.enableSnapToBPM(newSettings.useBPM);
	timeline.setBPM(newSettings.bpm);
    
	if(settings.useOSC){
        sender.setup(settings.oscIP, settings.oscPort);
    }
    ofxXmlSettings defaultSettings;
    defaultSettings.loadFile("settings.xml");
    defaultSettings.setValue("lastProjectPath", settings.path);
    defaultSettings.setValue("lastProjectName", settings.name);
    defaultSettings.saveFile();
}

//--------------------------------------------------------------
void testApp::saveProject(){
    ofxXmlSettings projectSettings;
    
    //SAVE ALL TRACKS
    projectSettings.addTag("tracks");
    projectSettings.pushTag("tracks");
    vector<ofxTLPage*>& pages = timeline.getPages();
    for(int i = 0; i < pages.size(); i++){
        projectSettings.addTag("page");
        projectSettings.pushTag("page");
        projectSettings.addValue("name", pages[i]->getName());
        vector<ofxTLTrack*> tracks = pages[i]->getTracks();
        for (int t = 0; t < tracks.size(); t++) {
            projectSettings.addTag("track");
            projectSettings.pushTag("track", t);
            projectSettings.addValue("type", tracks[t]->getTrackType());
            projectSettings.addValue("xmlFileName", tracks[t]->getXMLFileName());
            projectSettings.addValue("trackName",tracks[t]->getName());
            projectSettings.addValue("displayName",tracks[t]->getDisplayName());
            projectSettings.popTag();
        }
        projectSettings.popTag(); //page
    }
	projectSettings.popTag(); //tracks
    
    //LOAD OTHER SETTINGS
    projectSettings.addTag("timelineSettings");
    projectSettings.pushTag("timelineSettings");
    projectSettings.addValue("duration", timeline.getDurationInTimecode());
    projectSettings.addValue("playhead", timeline.getCurrentTimecode());
    projectSettings.addValue("inpoint", timeline.getInPointTimecode());
    projectSettings.addValue("outpoint", timeline.getOutPointTimecode());
    projectSettings.addValue("loop", timeline.getLoopType() == OF_LOOP_NORMAL);
	projectSettings.popTag();// timelineSettings
    
    //UI SETTINGS
    projectSettings.addTag("projectSettings");
    projectSettings.pushTag("projectSettings");
    projectSettings.addValue("useBPM", settings.useBPM);
    projectSettings.addValue("bpm", settings.bpm);
    projectSettings.addValue("snapToBPM", settings.snapToBPM);
    projectSettings.addValue("snapToKeys", settings.snapToKeys);
    projectSettings.addValue("useOSC", settings.useOSC);
    projectSettings.addValue("OSCIP", settings.oscIP);
    projectSettings.addValue("OSCPort", settings.oscPort);

	projectSettings.popTag(); //projectSettings
    projectSettings.saveFile(settings.settingsPath);
}

//--------------------------------------------------------------
void testApp::createHeaderForTrack(ofxTLTrack* track){
    ofxTLUIHeader* headerGui = new ofxTLUIHeader();
    ofxTLTrackHeader* header = timeline.getTrackHeader(track);
    headerGui->setTrackHeader(header);
    headers.push_back(headerGui);
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
