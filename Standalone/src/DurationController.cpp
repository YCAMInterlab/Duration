//
//  DurationController.h
//  Duration
//
//  Duration is an application for time.
//  Made at YCAM InterLab
//
//

#include "DurationController.h"

#define DROP_DOWN_WIDTH 200
#define TEXT_INPUT_WIDTH 100

#define NEW_PROJECT_TEXT "new project..."
#define OPEN_PROJECT_TEXT "open project..."


DurationController::DurationController(){
	//TODO: make variable
	oscRate = (1.0/60.0)*1000; //in millis
	lastOSCBundleSent = 0;

	shouldCreateNewProject = false;
    shouldLoadProject = false;

}

DurationController::~DurationController(){

}

void DurationController::setup(){
    
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

	tooltipFont.loadFont("GUI/NewMedia Fett.ttf", 8);
	
	//setup timeline
	timeline.setup();
    timeline.setFrameRate(30);
	timeline.setDurationInSeconds(30);
	timeline.setOffset(ofVec2f(0, 90));
    timeline.getColors().load("defaultColors.xml");
    timeline.setBPM(120.f);
	timeline.setAutosave(false);
	timeline.moveToThread(); //increases accuracy of bang call backs
    
	//Set up top GUI
    gui = new ofxUICanvas(0,0,ofGetWidth(), 90);
    
    //ADD PROJECT DROP DOWN
    projectDropDown = new ofxUIDropDownList(DROP_DOWN_WIDTH, "PROJECT", projects, OFX_UI_FONT_LARGE);
    projectDropDown->setAutoClose(true);
    gui->addWidgetDown(projectDropDown);
    //ADD TRACKS
    vector<string> trackTypes;
    trackTypes.push_back("BANGS");
    trackTypes.push_back("FLAGS");
    trackTypes.push_back("SWITCHES");
    trackTypes.push_back("CURVES");
    trackTypes.push_back("COLORS");
	//	    trackTypes.push_back("SOUND");
	
    addTrackDropDown = new ofxUIDropDownList(DROP_DOWN_WIDTH, "ADD TRACK", trackTypes, OFX_UI_FONT_MEDIUM);
    addTrackDropDown->setAllowMultiple(false);
    addTrackDropDown->setAutoClose(true);
	//    gui->addWidgetRight(addTrackDropDown);
	gui->addWidgetSouthOf(addTrackDropDown, "PROJECT");
	
    saveButton = new ofxUIMultiImageButton(32, 32, false, "GUI/save_.png", "SAVE");
    saveButton->setLabelVisible(false);
    gui->addWidgetEastOf(saveButton, "PROJECT");
    
	
    //ADD TIMECODE
    string zeroTimecode = "00:00:00:000";
    timeLabel = new ofxUILabel(zeroTimecode, OFX_UI_FONT_LARGE);
    gui->addWidgetRight(timeLabel);
	//durationLabel = new ofxUILabel(" / "+zeroTimecode, OFX_UI_FONT_SMALL);
    durationLabel = new ofxUITextInput("DURATION", zeroTimecode, timeLabel->getRect()->width,0,0,0,OFX_UI_FONT_MEDIUM);
    durationLabel->setAutoClear(false);
    gui->addWidgetSouthOf(durationLabel, zeroTimecode);
    
    //ADD PLAY/PAUSE
    playpauseToggle = new ofxUIMultiImageToggle(32, 32, false, "GUI/play_.png", "PLAYPAUSE");
    playpauseToggle->setLabelVisible(false);
    gui->addWidgetEastOf(playpauseToggle, zeroTimecode);
    stopButton = new ofxUIMultiImageButton(32, 32, false, "GUI/stop_.png", "STOP");
    stopButton->setLabelVisible(false);
    gui->addWidgetRight(stopButton);
	loopToggle = new ofxUIMultiImageToggle(32, 32, false, "GUI/loop_.png", "LOOP");
	loopToggle->setLabelVisible(false);
    gui->addWidgetRight(loopToggle);
    
	
    //SETUP BPM CONTROLS
	useBPMToggle = new ofxUILabelToggle("BPM", false,0,0,0,0);
    gui->addWidgetRight(useBPMToggle);
	bpmDialer = new ofxUINumberDialer(0., 250., 120., 2, "BPM_VALUE", OFX_UI_FONT_MEDIUM);
    gui->addWidgetEastOf(bpmDialer, "BPM");
    //figure out where to put this
//	snapToKeysToggle = new ofxUILabelToggle("Snap to Keys",false,0,0,00,OFX_UI_FONT_MEDIUM);
//	gui->addWidgetSouthOf(snapToKeysToggle, "BPM");
//    snapToBPMToggle = new ofxUILabelToggle("Snap to BPM",false,0,0,0,0,OFX_UI_FONT_SMALL);
//    gui->addWidgetSouthOf(snapToBPM, "BPM");
    
    //SETUP OSC CONTROLS
    enableOSCInToggle = new ofxUILabelToggle("OSC IN",false,0,0,0,0, OFX_UI_FONT_MEDIUM);
    enableOSCOutToggle = new ofxUILabelToggle("OSC OUT",false,0,0,0,0, OFX_UI_FONT_MEDIUM);
    oscOutIPInput = new ofxUITextInput("OSCIP", "127.0.0.1",TEXT_INPUT_WIDTH,0,0,0, OFX_UI_FONT_MEDIUM);
    oscOutIPInput->setAutoClear(false);
	
    oscInPortInput = new ofxUITextInput("OSCINPORT", "12346",TEXT_INPUT_WIDTH,0,0,0, OFX_UI_FONT_MEDIUM);
    oscInPortInput->setAutoClear(false);

    oscOutPortInput = new ofxUITextInput("OSCOUTPORT", "12345",TEXT_INPUT_WIDTH,0,0,0, OFX_UI_FONT_MEDIUM);
    oscOutPortInput->setAutoClear(false);

	gui->addWidgetRight(enableOSCInToggle);
    gui->addWidgetRight(oscInPortInput);
    gui->addWidgetRight(enableOSCOutToggle);
    gui->addWidgetRight(oscOutIPInput);
    gui->addWidgetRight(oscOutPortInput);

	ofAddListener(gui->newGUIEvent, this, &DurationController::guiEvent);
	gui->disableAppEventCallbacks(); //we want to controld raw 
	
	//add events
    ofAddListener(timeline.events().bangFired, this, &DurationController::bangFired);
	ofAddListener(ofEvents().update, this, &DurationController::update);
	ofAddListener(ofEvents().draw, this, &DurationController::draw);
	ofAddListener(ofEvents().keyPressed, this, &DurationController::keyPressed);
	ofAddListener(ofEvents().exit, this, &DurationController::exit);

    //SET UP LISENTERS
    
    ofxXmlSettings defaultSettings;
    if(defaultSettings.loadFile("settings.xml")){
        string lastProjectPath = defaultSettings.getValue("lastProjectPath", "");
        string lastProjectName = defaultSettings.getValue("lastProjectName", "");
        if(lastProjectPath != "" && lastProjectName != "" && ofDirectory(lastProjectPath).exists()){
            loadProject(lastProjectPath, lastProjectName);
        }
        else{
            ofLogError() << "Duration -- Last project was not found, creating a new project";
            loadProject(ofToDataPath(defaultProjectDirectoryPath+"Sample Project"), "Sample Project", true);
        }
    }
    else {
//        cout << "Loading sample project " << defaultProjectDirectoryPath << endl;
        loadProject(ofToDataPath(defaultProjectDirectoryPath+"Sample Project"), "Sample Project", true);
    }


	createTooltips();
	
	startThread();
}

void DurationController::threadedFunction(){
	while(isThreadRunning()){
		handleOscIn();
		handleOscOut();

		ofSleepMillis(1);
	}
}

void DurationController::handleOscIn(){
	if(!settings.oscInEnabled){
		return;
	}
	
	long timelineStartTime = timeline.getCurrentTimeMillis();
	
	while(receiver.hasWaitingMessages()){

		ofxOscMessage m;
		receiver.getNextMessage(&m);
		bool handled = false;
		long startTime = recordTimer.getAppTimeMicros();
		vector<ofxTLPage*>& pages = timeline.getPages();
		for(int i = 0; i < pages.size(); i++){
			vector<ofxTLTrack*> tracks = pages[i]->getTracks();
			for(int t = 0; t < tracks.size(); t++){
//				cout << " testing against " << "/"+tracks[t]->getDisplayName() << endl;
				ofxTLTrack* track = tracks[t];
				if(m.getAddress() == ofFilePath::addLeadingSlash(track->getDisplayName()) ){
					if(timeline.getIsPlaying()){ //TODO: check for local record enabled on this track
						if(track->getTrackType() == "Curves" ){
							ofxTLCurves* curves = (ofxTLCurves*)track;
							//curves->addKeyframeAtMillis(m.getArgAsFloat(0), recordTimer.getElapsedMillis()+recordTimeOffset);
//							cout << "adding value " << m.getArgAsFloat(0) << endl;
							if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
								curves->addKeyframeAtMillis(m.getArgAsFloat(0), timelineStartTime);
							}
						}
						else if(track->getTrackType() == "Bangs"){
							ofxTLBangs* bangs = (ofxTLBangs*)track;
							bangs->addKeyframeAtMillis(0,timelineStartTime);
						}
					}

					headers[track->getName()]->lastInputReceivedTime = recordTimer.getAppTimeSeconds();
					handled = true;
				}
			}
		}
		
		long endTime = recordTimer.getAppTimeMicros();
//		cout << "receiving message took " << (endTime - startTime) << " micros " << endl;
		if(handled){
			return;
		}
		
		//check for playback messages
		if(m.getAddress() == "/duration/open"){
			string projectPath = ofToDataPath(m.getArgAsString(0));
			if(ofFilePath::isAbsolute(projectPath)){
				loadProject(projectPath);
			}
			else{
				loadProject(defaultProjectDirectoryPath+m.getArgAsString(0));
			}
		}
		else if(m.getAddress() == "/duration/play"){
			timeline.play();
		}
		else if(m.getAddress() == "/duration/stop"){
			timeline.stop();
			//TODO: stop record mode
		}
		else if(m.getAddress() == "/duration/record"){
			timeline.play();
			//TODO: turn on record mode
		}
		else if(m.getAddress() == "/duration/seektosecond"){
			if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
				timeline.setCurrentTimeSeconds(m.getArgAsFloat(0));
			}
			else{
				ofLogError("Duration:OSC") << " Seek to Second failed: first argument must be a float";
			}
		}
		else if(m.getAddress() == "/duration/seektoposition"){
			if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
				float percent = ofClamp(m.getArgAsFloat(0),0.0,1.0);
				timeline.setCurrentTimeSeconds(percent);
			}
			else{
				ofLogError("Duration:OSC") << " Seek to Position failed: first argument must be a float between 0.0 and 1.0";
			}
		}
		else if(m.getAddress() == "/duration/seektomillis"){
			if(m.getArgType(0) == OFXOSC_TYPE_INT32){
				timeline.setCurrentTimeMillis(m.getArgAsInt32(0));
			}
			else if(m.getArgType(0) == OFXOSC_TYPE_INT64){
				timeline.setCurrentTimeMillis(m.getArgAsInt64(0));
			}
			else{
				ofLogError("Duration:OSC") << " Seek to Millis failed: first argument must be a int 32 or in 64";
			}
		}
		else if(m.getAddress() == "/druation/seektotimecode"){
			if(m.getArgType(0) == OFXOSC_TYPE_STRING){
				long millis = ofxTimecode::millisForTimecode(m.getArgAsString(0));
				if(millis > 0){
					timeline.setCurrentTimeMillis(millis);
				}
				else{
					ofLogError("Duration:OSC") << " Seek to Timecode failed: bad timecode. Please format HH:MM:SS:MMM";
				}
			}
			else{
				ofLogError("Duration:OSC") << " Seek to Timecode failed: first argument must be a string";
			}
		}
	}
}

//TODO: hook up to record button
//and make NO LOOP 
void DurationController::startRecording(){
	recordTimer.setStartTime();
	recordTimeOffset = timeline.getCurrentTimeMillis();
	timeline.play();
}

void DurationController::stopRecording(){

}

void DurationController::handleOscOut(){
	
	if(!settings.oscOutEnabled){
		return;
	}
	
	unsigned long bundleTime = recordTimer.getAppTimeMillis();
	if(lastOSCBundleSent+oscRate > bundleTime){
		return;
	}
	
	unsigned long sampleMillis = timeline.getCurrentTimeMillis();
	int numMessages = 0;
	ofxOscBundle bundle;

	lock();
	vector<ofxTLPage*>& pages = timeline.getPages();
	for(int i = 0; i < pages.size(); i++){
		vector<ofxTLTrack*> tracks = pages[i]->getTracks();
		for(int t = 0; t < tracks.size(); t++){
			if(!headers[tracks[t]->getName()]->isOSCEnabled()){
				continue;
			}
			
			string trackType = tracks[t]->getTrackType();
			if(trackType == "Curves" || trackType == "Switches" || trackType == "Colors"){
				ofxOscMessage m;
				m.setAddress(ofFilePath::addLeadingSlash(tracks[t]->getDisplayName()));
				if(trackType == "Curves"){
					ofxTLCurves* curves = (ofxTLCurves*)tracks[t];
					m.addFloatArg(curves->getValueAtTimeInMillis(sampleMillis));
				}
				else if(trackType == "Switches"){
					ofxTLSwitches* switches = (ofxTLSwitches*)tracks[t];
					m.addIntArg(switches->isOnAtMillis(sampleMillis));
				}
				else if(trackType == "Colors"){
					ofxTLColorTrack* colors = (ofxTLColorTrack*)tracks[t];
					ofColor color = colors->getColorAtMillis(sampleMillis);
					m.addIntArg(color.r);
					m.addIntArg(color.g);
					m.addIntArg(color.b);
				}
				
				bundle.addMessage(m);
				numMessages++;
			}
		}
	}
	unlock();
	
	//any bangs that came our way this frame send them out too
	for(int i = 0; i < bangsReceived.size(); i++){
		bundle.addMessage(bangsReceived[i]);
	}
	numMessages += bangsReceived.size();
	if(numMessages > 0){
		sender.sendBundle(bundle);
	}
	lastOSCBundleSent = bundleTime;
	bangsReceived.clear();
	
}

//--------------------------------------------------------------
void DurationController::bangFired(ofxTLBangEventArgs& bang){
// 	ofLogNotice() << "Bang from " << bang.track->getDisplayName() << " at time " << bang.currentTime << " with flag " << bang.flag;
	if(settings.oscOutEnabled){
		return;
	}
    string trackType = bang.track->getTrackType();
    if(!headers[bang.track->getName()]->isOSCEnabled()){
        return;
    }
    ofxOscMessage m;
    m.setAddress( ofFilePath::addLeadingSlash(bang.track->getDisplayName()) );
    m.addIntArg(bang.currentMillis);
    if(trackType == "Flags"){
        m.addStringArg(bang.flag);
    }
	bangsReceived.push_back(m);	
}

//--------------------------------------------------------------
void DurationController::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
	//	cout << "name is " << name << " kind is " << kind << endl;
    
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
				lock();
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
                    newTrack = timeline.addCurves(name, xmlFile);
                }
                else if(selectedTrackType == "SWITCHES"){
                	string name = timeline.confirmedUniqueName("Switches");
                    string xmlFile = ofToDataPath(settings.path + "/" + name + "_.xml");
                    newTrack = timeline.addSwitches(name, xmlFile);
                }
                else if(selectedTrackType == "COLORS"){
                	string name = timeline.confirmedUniqueName("Colors");
                    string xmlFile = ofToDataPath(settings.path + "/" + name + "_.xml");
                    newTrack = timeline.addColors(name, xmlFile);
                }
                else if(selectedTrackType == "SOUND"){
                	string name = timeline.confirmedUniqueName("Sound");
                    string xmlFile = ofToDataPath(settings.path + "/" + name + "_.xml");
					
                    newTrack = timeline.addColors(name, xmlFile);
                }
                
                if(newTrack != NULL){
                    createHeaderForTrack(newTrack);
                }
				
				unlock();
				
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
                    loadProject(ofToDataPath(defaultProjectDirectoryPath+selectedProjectName), selectedProjectName);
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
	else if(e.widget == snapToKeysToggle){
		timeline.enableSnapToOtherKeyframes(snapToKeysToggle->getValue());
	}
    //OSC INPUT
    else if(e.widget == enableOSCInToggle){
		settings.oscInEnabled = enableOSCInToggle->getValue();
        if(settings.oscInEnabled){
            //TODO validate address
            receiver.setup(settings.oscInPort);
        }
    }
	//INCOMING PORT
	else if(e.widget == oscInPortInput){
        int newPort = ofToInt(oscInPortInput->getTextString());
        if(newPort != settings.oscInEnabled && newPort > 0 && newPort < 65535){
	        sender.setup(settings.oscIP, newPort);
            settings.oscInPort = newPort;
        }
        else {
            oscInPortInput->setTextString( ofToString(settings.oscInPort) );
        }
    }
	//OUTGOING IP
    else if(e.widget == oscOutIPInput){
        string newIP = ofToLower(oscOutIPInput->getTextString());
        if(newIP == settings.oscIP){
            return;
        }
		
        bool valid = (newIP == "localhost");
		if(!valid){
			vector<string> ipComponents = ofSplitString(newIP, ".");
			if(ipComponents.size() == 4){
				for(int i = 0; i < 4; i++){
					int component = ofToInt(ipComponents[i]);
					if (component < 0 || component > 255){
						valid = false;
						break;
					}
				}
			}
		}
		
		if(valid){
			settings.oscIP = newIP;
			sender.setup(settings.oscIP, settings.oscOutPort);
		}
		oscOutIPInput->setTextString(settings.oscIP);
    }
	//OSC OUTPUT
    else if(e.widget == enableOSCOutToggle){
		settings.oscOutEnabled = enableOSCOutToggle->getValue();
        if(settings.oscOutEnabled){
            //TODO validate address
            sender.setup(settings.oscIP, settings.oscOutPort);
        }
    }

}

//--------------------------------------------------------------
void DurationController::update(ofEventArgs& args){
	gui->update();
	
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
    
    //check if we deleted an element this frame
    map<string,ofPtr<ofxTLUIHeader> >::iterator it = headers.begin();
    while(it != headers.end()){
		if(it->second->getShouldDelete()){
            timeline.removeTrack(it->first);
            headers.erase(it);
            break;
        }
        it++;
    }
}

//--------------------------------------------------------------
void DurationController::draw(ofEventArgs& args){

	//go through and draw all the overlay backgrounds to indicate 'hot' track sfor recording
	if(settings.oscInEnabled){
		ofPushStyle();
		map<string, ofPtr<ofxTLUIHeader> >::iterator trackit;
		for(trackit = headers.begin(); trackit != headers.end(); trackit++){
			//TODO: check to make sure recording is enabled on this track
			//TODO: find a way to illustrate 'invalid' output sent to this track
			float timeSinceInput = recordTimer.getAppTimeSeconds() - trackit->second->lastInputReceivedTime;
			if(timeSinceInput > 0 && timeSinceInput < 1.0){
				//oscilating red to indicate active
				ofSetColor(200,0,0,(1-timeSinceInput)*(80 + 20*sin(ofGetElapsedTimef()*.5+.5)));
				ofRect(trackit->second->getTrack()->getDrawRect());
			}
		}
		ofPopStyle();
	}
	
	timeline.draw();
	gui->draw();
	
	drawTooltips();
	//drawTooltipDebug();
}

//--------------------------------------------------------------
void DurationController::keyPressed(ofKeyEventArgs& keyArgs){
    if(timeline.isModal()){
        return;
    }
	
    int key = keyArgs.key;
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

//--------------------------------------------------------------
DurationProjectSettings DurationController::defaultProjectSettings(){
    DurationProjectSettings settings;
    
    settings.name = "newProject";
    settings.path = defaultProjectDirectoryPath + settings.name;
    
    settings.useBPM = false;
    settings.bpm = 120.0f;
    settings.snapToBPM = false;
    settings.snapToKeys = true;
    
    settings.oscOutEnabled = true;
	settings.oscInEnabled = true;
    settings.oscInPort = 12346;
    settings.oscIP = "localhost";
    settings.oscOutPort = 12345;
    return settings;
}

//--------------------------------------------------------------
void DurationController::newProject(string newProjectPath, string newProjectName){
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
	
    headers.clear(); //smart pointers will call destructor
    timeline.reset();
	
    //saves file with default settings to new directory
    saveProject();
    
    loadProject(settings.path, settings.name);
    
    projectDropDown->addToggle(newProjectName);
}

//--------------------------------------------------------------
void DurationController::loadProject(string projectPath, bool forceCreate){
	//scrape off the last component of the filename for the project name
	projectPath = ofFilePath::removeTrailingSlash(projectPath);
	vector<string> pathComponents = ofSplitString(projectPath, "/");
	loadProject(projectPath, pathComponents[pathComponents.size()-1], forceCreate);
}

//--------------------------------------------------------------
void DurationController::loadProject(string projectPath, string projectName, bool forceCreate){
    
	
    ofxXmlSettings projectSettings;
    if(!projectSettings.loadFile(ofToDataPath(projectPath+"/.durationproj"))){
        if(forceCreate){
            newProject(projectPath, projectName);
        }
        else{
            ofLogError() << " failed to load project " << ofToDataPath(projectPath+"/.durationproj") << endl;
        }
        return;
    }
	
	lock();
	
    headers.clear(); //smart pointers will call destructor
    
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
            else if(trackType == "Curves"){
                ofxTLCurves* curves = timeline.addCurves(trackName, trackFilePath);
                curves->setValueRange(ofRange(projectSettings.getValue("min", 0.0),
                                              projectSettings.getValue("max", 1.0)));
                newTrack = curves;
            }
            else if(trackType == "Switches"){
                newTrack = timeline.addSwitches(trackName, trackFilePath);
            }
            else if(trackType == "Colors"){
				string paletteFilePath  = projectSettings.getValue("palette", timeline.getDefaultColorPalettePath());
                newTrack = timeline.addColorsWithPalette(trackName,paletteFilePath);
            }
			/*
			else if(trackType == "Sound"){
				if(audioTrack != NULL){
					ofLogError("DurationController::loadProject") << "Trying to add an additional sound track";
				}
				audioTrack = new ofxTLAudioTrack();
				timeline.addTrack(trackName, audioTrack);
				newTrack = audioTrack;
				string paletteFilePath = projectSettings.getValue("palette", timeline.getDefaultColorPalettePath());
			}
			*/
			
            if(newTrack != NULL){
                string displayName = projectSettings.getValue("displayName","");
                if(displayName != ""){
	                newTrack->setDisplayName(displayName);
                }
				ofxTLUIHeader* headerTrack = createHeaderForTrack(newTrack);
            	headerTrack->setOSCEnabled(projectSettings.getValue("sendOSC", true));
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
//    snapToBPMToggle->setValue( newSettings.snapToBPM = projectSettings.getValue("snapToBPM", true) );
//    snapToKeysToggle->setValue( newSettings.snapToKeys = projectSettings.getValue("snapToKeys", true) );
    enableOSCInToggle->setValue( newSettings.oscInEnabled = projectSettings.getValue("oscInEnabled", true) );
	enableOSCOutToggle->setValue( newSettings.oscOutEnabled = projectSettings.getValue("oscOutEnabled", true) );
    oscInPortInput->setTextString( ofToString(newSettings.oscInPort = projectSettings.getValue("oscInPort", 12346)) );
    oscOutIPInput->setTextString( newSettings.oscIP = projectSettings.getValue("oscIP", "localhost") );
    oscOutPortInput->setTextString( ofToString(newSettings.oscOutPort = projectSettings.getValue("oscOutPort", 12345)) );
	
    projectSettings.popTag(); //project settings;
	
    newSettings.path = projectPath;
    newSettings.name = projectName;
    newSettings.settingsPath = ofToDataPath(newSettings.path + "/.durationproj");
    settings = newSettings;
	
    projectDropDown->setLabelText(projectName);
    timeline.setShowBPMGrid(newSettings.useBPM);
    timeline.enableSnapToBPM(newSettings.useBPM);
	timeline.setBPM(newSettings.bpm);
    
	if(settings.oscInEnabled){
		receiver.setup(settings.oscInPort);
	}
	if(settings.oscOutEnabled){
        sender.setup(settings.oscIP, settings.oscOutPort);
    }
    ofxXmlSettings defaultSettings;
    defaultSettings.loadFile("settings.xml");
    defaultSettings.setValue("lastProjectPath", settings.path);
    defaultSettings.setValue("lastProjectName", settings.name);
    defaultSettings.saveFile();
	
	unlock();
}

//--------------------------------------------------------------
void DurationController::saveProject(){
	
	timeline.save();
	
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
            //save track properties
            string trackType = tracks[t]->getTrackType();
            string trackName = tracks[t]->getName();
            projectSettings.addValue("type", trackType);
            projectSettings.addValue("xmlFileName", tracks[t]->getXMLFileName());
            projectSettings.addValue("trackName",tracks[t]->getName());
            projectSettings.addValue("displayName",tracks[t]->getDisplayName());
            //save custom gui props
            projectSettings.addValue("sendOSC", headers[trackName]->isOSCEnabled());
            if(trackType == "Curves"){
                ofxTLCurves* tweens = (ofxTLCurves*)tracks[t];
                projectSettings.addValue("min", tweens->getValueRange().min);
                projectSettings.addValue("max", tweens->getValueRange().max);
            }
			else if(trackType == "Colors"){
				ofxTLColorTrack* colors = (ofxTLColorTrack*)tracks[t];
				projectSettings.addValue("palette", colors->getPalettePath());
			}
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
	
    projectSettings.addValue("oscInEnabled", settings.oscInEnabled);
    projectSettings.addValue("oscOutEnabled", settings.oscOutEnabled);
    projectSettings.addValue("oscInPort", settings.oscInPort);
    projectSettings.addValue("oscIP", settings.oscIP);
    projectSettings.addValue("oscInPort", settings.oscOutPort);
	
	projectSettings.popTag(); //projectSettings
    projectSettings.saveFile(settings.settingsPath);
}

//--------------------------------------------------------------
ofxTLUIHeader* DurationController::createHeaderForTrack(ofxTLTrack* track){
    ofxTLUIHeader* headerGui = new ofxTLUIHeader();
    ofxTLTrackHeader* header = timeline.getTrackHeader(track);
    headerGui->setTrackHeader(header);
//	lock();
    headers[track->getName()] = ofPtr<ofxTLUIHeader>( headerGui );
//	unlock();
    return headerGui;
}

void DurationController::createTooltips(){
	

	ofVec2f zone2 = ofVec2f(420, 55);
	//switch project
	Tooltip projectTip;
	projectTip.text = "switch project";
	projectTip.sourceRect = *projectDropDown->getRect();
	projectTip.displayPoint = ofVec2f(projectTip.sourceRect.x, 55);
	tooltips.push_back(projectTip);
	
	//save
	Tooltip saveTip;
	saveTip.text = "save";
	saveTip.sourceRect = *saveButton->getRect();
	saveTip.displayPoint = ofVec2f(saveTip.sourceRect.x, 55);
	tooltips.push_back(saveTip);
	
	//edit duration
	Tooltip editDurationTip;
	editDurationTip.text = "edit duration";
	editDurationTip.displayPoint = zone2;
	editDurationTip.sourceRect = *durationLabel->getRect();
	tooltips.push_back(editDurationTip);
	
	//current time
	Tooltip currentTimeTip;
	currentTimeTip.text = "current time";
	currentTimeTip.displayPoint = zone2;
	currentTimeTip.sourceRect = *timeLabel->getRect();
	tooltips.push_back(currentTimeTip);
	
	//play/pause
	Tooltip playpauseTip;
	playpauseTip.text = "play/pause"; //TODO: switch dynamically
	playpauseTip.displayPoint = zone2;
	playpauseTip.sourceRect = *playpauseToggle->getRect();
	tooltips.push_back(playpauseTip);
	
	//stop
	Tooltip stopTip;
	stopTip.text = "stop";
	stopTip.displayPoint = zone2;
	stopTip.sourceRect = *stopButton->getRect();
	tooltips.push_back(stopTip);
	
	//loop
	Tooltip loopTip;
	loopTip.text = "toggle loop";
	loopTip.displayPoint = zone2;
	loopTip.sourceRect = *loopToggle->getRect();
	tooltips.push_back(loopTip);

	//enable Snap to BPM
	Tooltip bpmTip;
	bpmTip.text = "snap to measures";
	bpmTip.sourceRect = *useBPMToggle->getRect();
	bpmTip.displayPoint = ofVec2f(bpmTip.sourceRect.x, 55);
	tooltips.push_back(bpmTip);
	
	//set beats per minute
	Tooltip setBpmTip;
	setBpmTip.text = "set beats per minute";
	setBpmTip.sourceRect = *bpmDialer->getRect();
	setBpmTip.displayPoint = ofVec2f(setBpmTip.sourceRect.x, 55);
	tooltips.push_back(setBpmTip);
	
	//enable OSC
	Tooltip oscInTip;
	oscInTip.text = "enable incoming OSC";
	oscInTip.sourceRect = *enableOSCInToggle->getRect();
	oscInTip.displayPoint = ofVec2f(oscInTip.sourceRect.x, 55);
	tooltips.push_back(oscInTip);

	//osc In Port
	Tooltip oscInPortTip;
	oscInPortTip.text = "incoming OSC port";
	oscInPortTip.sourceRect = *oscInPortInput->getRect();
	oscInPortTip.displayPoint = ofVec2f(oscInPortTip.sourceRect.x, 55);
	tooltips.push_back(oscInPortTip);

	//osc Out
	Tooltip oscOutTip;
	oscOutTip.text = "enable outgoing OSC";
	oscOutTip.sourceRect = *enableOSCOutToggle->getRect();
	oscOutTip.displayPoint = ofVec2f(oscOutTip.sourceRect.x, 55);
	tooltips.push_back(oscOutTip);

	//osc Out IP
	Tooltip oscOutIPTip;
	oscOutIPTip.text = "remote IP";
	oscOutIPTip.sourceRect = *oscOutIPInput->getRect();
	oscOutIPTip.displayPoint = ofVec2f(oscOutIPTip.sourceRect.x, 55);
	tooltips.push_back(oscOutIPTip);

	//osc Out IP
	Tooltip oscOutPortTip;
	oscOutPortTip.text = "remote port";
	oscOutPortTip.sourceRect = *oscOutPortInput->getRect();
	oscOutPortTip.displayPoint = ofVec2f(oscOutPortTip.sourceRect.x, 55);
	tooltips.push_back(oscOutPortTip);

	
	for(int i = 0; i < tooltips.size(); i++){
		tooltips[i].debugColor = ofColor::fromHsb(ofRandom(255), ofRandom(255,200), ofRandom(255,200));		
	}	
}

void DurationController::drawTooltips(){

	ofVec2f mousepoint(ofGetMouseX(), ofGetMouseY());
	for(int i = 0; i < tooltips.size(); i++){
		if(tooltips[i].sourceRect.inside(mousepoint)){
			tooltipFont.drawString(tooltips[i].text,
								   tooltips[i].displayPoint.x,
								   tooltips[i].displayPoint.y);
		}
	}
}

void DurationController::drawTooltipDebug(){
	//draw tool tip position finder
	tooltipFont.drawString("("+ofToString(ofGetMouseX())+","+ofToString(ofGetMouseY())+")", ofGetMouseX(), ofGetMouseY());
	//draw tooltip debug balloons
	ofPushStyle();
	for(int i = 0; i < tooltips.size(); i++){
		ofNoFill();
		ofSetColor(tooltips[i].debugColor, 200);
		ofRect(tooltips[i].sourceRect);
		ofLine(tooltips[i].sourceRect.getMax(), tooltips[i].displayPoint);
		ofFill();
		ofSetColor(tooltips[i].debugColor, 50);
		ofRect(tooltips[i].sourceRect);
		ofSetColor(255);
		tooltipFont.drawString(tooltips[i].text, tooltips[i].sourceRect.x+5,tooltips[i].sourceRect.y+10);
	}
	ofPopStyle();
}

void DurationController::exit(ofEventArgs& e){
	ofLogNotice("DurationController") << "waiting for thread on exit";
	waitForThread(true);
}
