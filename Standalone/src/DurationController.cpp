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
	oscIsEnabled = true;
	recordingIsEnabled = true;
	audioTrack = NULL;
	oscRate = (1.0/30.0)*1000; //in millis
	lastOSCBundleSent = 0;
}

DurationController::~DurationController(){

}

void DurationController::setup(){
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

	
	//setup timeline
	timeline.setup();
    timeline.setFrameRate(30);
	timeline.setDurationInSeconds(30);
	timeline.setOffset(ofVec2f(0, 75));
    timeline.getColors().load("defaultColors.xml");
    timeline.setBPM(120.f);
	timeline.setAutosave(false);
	timeline.moveToThread(); //increases accuracy of bang call backs
    
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
    trackTypes.push_back("COLORS");
//	    trackTypes.push_back("SOUND");
	
    addTrackDropDown = new ofxUIDropDownList(DROP_DOWN_WIDTH, "ADD TRACK", trackTypes, OFX_UI_FONT_MEDIUM);
    addTrackDropDown->setAllowMultiple(false);
    addTrackDropDown->setAutoClose(true);
    gui->addWidgetRight(addTrackDropDown);
    
    //SETUP BPM CONTROLS
	
//	useBPMToggle = new ofxUILabelToggle(false, "BPM", OFX_UI_FONT_MEDIUM);
	useBPMToggle = new ofxUILabelToggle("BPM", false,0,0,0,0);
    gui->addWidgetEastOf(useBPMToggle, "ADD TRACK");
	bpmDialer = new ofxUINumberDialer(0., 250., 120., 2, "BPM_VALUE", OFX_UI_FONT_MEDIUM);
    gui->addWidgetEastOf(bpmDialer, "BPM");
	
    //figure out where to put these
    snapToBPMToggle = new ofxUILabelToggle("Snap to BPM",false,0,0,0,0,OFX_UI_FONT_SMALL);
	//    gui->addWidgetSouthOf(snapToBPM, "BPM");
    snapToKeysToggle = new ofxUILabelToggle("Snap to Keys",false,0,0,0,0,OFX_UI_FONT_SMALL);
	//    gui->addWidgetRight(snapToKeys);
    
    //SETUP OSC CONTROLS
    useOSCToggle = new ofxUILabelToggle("OSC",false,0,0,0,0);
    oscIPInput = new ofxUITextInput("OSCIP", "127.0.0.1",TEXT_INPUT_WIDTH,0,0,0, OFX_UI_FONT_MEDIUM);
    oscIPInput->setAutoClear(false);
    oscPortInput = new ofxUITextInput("OSCPORT", "12345",TEXT_INPUT_WIDTH,0,0,0, OFX_UI_FONT_MEDIUM);
    oscPortInput->setAutoClear(false);
    
    gui->addWidgetEastOf(useOSCToggle, "BPM_VALUE");
    gui->addWidgetRight(oscIPInput);
    gui->addWidgetRight(oscPortInput);

	ofAddListener(gui->newGUIEvent, this, &DurationController::guiEvent);
	gui->disableAppEventCallbacks();
	
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

	receiver.setup(12346);
	
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
					if(recordingIsEnabled && timeline.getIsPlaying()){
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
			recordingIsEnabled = false;
		}
		else if(m.getAddress() == "/duration/record"){
			timeline.play();
			recordingIsEnabled = false;
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

void DurationController::startRecording(){
	recordingIsEnabled = true;
	recordTimer.setStartTime();
	recordTimeOffset = timeline.getCurrentTimeMillis();
	timeline.play();
}

void DurationController::stopRecording(){
	recordingIsEnabled = false;
}

void DurationController::handleOscOut(){
	
	if(!oscIsEnabled){
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
 	ofLogNotice() << "Bang from " << bang.track->getDisplayName() << " at time " << bang.currentTime << " with flag " << bang.flag;
	
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
//	cout << "main draw" << endl;
	//go through and draw all the overlay backgrounds
	if(recordingIsEnabled){
		map<string, ofPtr<ofxTLUIHeader> >::iterator trackit;
		for(trackit = headers.begin(); trackit != headers.end(); trackit++){
			float timeSinceInput = recordTimer.getAppTimeSeconds() - trackit->second->lastInputReceivedTime;
			if(timeSinceInput > 0 && timeSinceInput < 1.0){
				
				ofSetColor(200,0,0,(1-timeSinceInput)*100);
				ofRect(trackit->second->getTrack()->getDrawRect());
			}
		}
	}
	
	timeline.draw();
	gui->draw();
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
    settings.snapToKeys = true;;
    
    settings.useOSC = true;;
    settings.oscIP = "127.0.0.1";
    settings.oscPort = 12345;
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
			else if(trackType == "Sound"){
				if(audioTrack != NULL){
					ofLogError("DurationController::loadProject") << "Trying to add an additional sound track";
				}
				audioTrack = new ofxTLAudioTrack();
				timeline.addTrack(trackName, audioTrack);
				newTrack = audioTrack;
				string paletteFilePath = projectSettings.getValue("palette", timeline.getDefaultColorPalettePath());
			}
			
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
    projectSettings.addValue("useOSC", settings.useOSC);
    projectSettings.addValue("OSCIP", settings.oscIP);
    projectSettings.addValue("OSCPort", settings.oscPort);
	
	projectSettings.popTag(); //projectSettings
    projectSettings.saveFile(settings.settingsPath);
}

//--------------------------------------------------------------
ofxTLUIHeader* DurationController::createHeaderForTrack(ofxTLTrack* track){
    ofxTLUIHeader* headerGui = new ofxTLUIHeader();
    ofxTLTrackHeader* header = timeline.getTrackHeader(track);
    headerGui->setTrackHeader(header);
    headers[track->getName()] = ofPtr<ofxTLUIHeader>( headerGui );
    return headerGui;
}

void DurationController::exit(ofEventArgs& e){
	ofLogNotice("DurationController") << "waiting for thread on exit";
	waitForThread(true);
}