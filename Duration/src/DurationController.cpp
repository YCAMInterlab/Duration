/**
 * Duration
 * Standalone timeline for Creative Code
 *
 * Copyright (c) 2012 James George
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "DurationController.h"
#include "ofxHotKeys.h"

#define DROP_DOWN_WIDTH 250
#define TEXT_INPUT_WIDTH 100

DurationController::DurationController(){
	lastOSCBundleSent = 0;
	shouldStartPlayback = false;
	receivedAddTrack = false;
	receivedPaletteToLoad = false;
	
	enabled = false;
	shouldCreateNewProject = false;
    shouldLoadProject = false;

#ifdef TARGET_OSX
	audioTrack = NULL;
#endif
}

DurationController::~DurationController(){

}

void DurationController::enableInterface(){
	if(!enabled){
		enabled = true;
		ofAddListener(ofEvents().update, this, &DurationController::update);
		ofAddListener(ofEvents().draw, this, &DurationController::draw);
		ofAddListener(ofEvents().keyPressed, this, &DurationController::keyPressed);
		gui->enable();
		gui->disableAppEventCallbacks();
		timeline.enable();
		map<string,ofPtr<ofxTLUIHeader> >::iterator it = headers.begin();
		while(it != headers.end()){
			it->second->getGui()->enable();
			it++;
		}
	}
}

void DurationController::disableInterface(){
	if(enabled){
		enabled = false;
		ofRemoveListener(ofEvents().update, this, &DurationController::update);
		ofRemoveListener(ofEvents().draw, this, &DurationController::draw);
		ofRemoveListener(ofEvents().keyPressed, this, &DurationController::keyPressed);
		gui->disable();
		timeline.disable();
		map<string,ofPtr<ofxTLUIHeader> >::iterator it = headers.begin();
		while(it != headers.end()){
			it->second->getGui()->disable();
			it++;
		}
	}
}

bool DurationController::isInterfaceEnabled(){
	return enabled;
}

void DurationController::setup(){
    
	if(!translation.load("languageFile.csv")){
		ofLogError("DurationController::setup") << "error setting up translation, unpredictable stuff will happen" << endl;
	}
	
	ofxXmlSettings defaultSettings;
	bool settingsLoaded = defaultSettings.loadFile("settings.xml");	;
	translation.setCurrentLanguage(defaultSettings.getValue("language", "english"));
	
	if(!settingsLoaded){
		defaultSettings.setValue("language", "english");
		defaultSettings.saveFile();
	}
	
    //populate projects
    vector<string> projects;
    projects.push_back(translation.translateKey("new project..."));
    projects.push_back(translation.translateKey("open project..."));
   
#ifdef TARGET_WIN32
    defaultProjectDirectoryPath = ofToDataPath(ofFilePath::getUserHomeDir() + "\\Documents\\Duration\\");
#else
    defaultProjectDirectoryPath = ofToDataPath(ofFilePath::getUserHomeDir() + "/Documents/Duration/");
#endif
	ofDirectory projectDirectory = ofDirectory(defaultProjectDirectoryPath);
	
    if(!projectDirectory.exists()){
        projectDirectory.create(true);
    }
    
    projectDirectory.listDir();
    for(int i = 0; i < projectDirectory.size(); i++){
        if(projectDirectory.getFile(i).isDirectory()){
            ofDirectory subDir = ofDirectory(projectDirectory.getPath(i));
			//cout << "checking path " << projectDirectory.getPath(i) << endl;
            subDir.allowExt("durationproj");
            subDir.setShowHidden(true);
            subDir.listDir();
            if(subDir.size() > 0){
                projects.push_back(projectDirectory.getName(i));
            }
        }
    }
	
	tooltipFont.loadFont("GUI/mplus-1c-regular.ttf", 5);
	timeline.setupFont("GUI/mplus-1c-regular.ttf", 6);
	
	//setup timeline
	timeline.setup();
//	timeline.curvesUseBinary = true; //ELOI SWITCH THIS HERE
//	timeline.enableUndo(false);
    timeline.setSpacebarTogglePlay(false);
    timeline.setFrameRate(30);
	timeline.setDurationInSeconds(30);
	timeline.setOffset(ofVec2f(0, 90));
    timeline.getColors().load("defaultColors.xml");
    timeline.setBPM(120.f);
	timeline.setAutosave(false);
	timeline.setEditableHeaders(true);
	timeline.moveToThread(); //increases accuracy of bang call backs
    
	//Set up top GUI
    gui = new ofxUICanvas(0,0,ofGetWidth(), 90);
    
    //ADD PROJECT DROP DOWN
    projectDropDown = new ofxUIDropDownList(DROP_DOWN_WIDTH, "PROJECT", projects, OFX_UI_FONT_LARGE);
    projectDropDown->setAutoClose(true);
    gui->addWidgetDown(projectDropDown);
    //ADD TRACKS
    vector<string> trackTypes;
    trackTypes.push_back(translation.translateKey("bangs"));
    trackTypes.push_back(translation.translateKey("flags"));
    trackTypes.push_back(translation.translateKey("switches"));
    trackTypes.push_back(translation.translateKey("curves"));
    trackTypes.push_back(translation.translateKey("colors"));
	trackTypes.push_back(translation.translateKey("lfo"));
#ifdef TARGET_OSX
	trackTypes.push_back(translation.translateKey("audio"));
#endif
	
    addTrackDropDown = new ofxUIDropDownList(DROP_DOWN_WIDTH, translation.translateKey("ADD TRACK"), trackTypes, OFX_UI_FONT_MEDIUM);
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
	useBPMToggle = new ofxUILabelToggle(translation.translateKey("BPM"), false);
    gui->addWidgetRight(useBPMToggle);
	bpmDialer = new ofxUINumberDialer(0., 250., 120., 2, "BPM_VALUE", OFX_UI_FONT_MEDIUM);
    gui->addWidgetEastOf(bpmDialer, translation.translateKey("BPM"));
    //figure out where to put this
//	snapToKeysToggle = new ofxUILabelToggle("Snap to Keys",false,0,0,00,OFX_UI_FONT_MEDIUM);
//	gui->addWidgetSouthOf(snapToKeysToggle, "BPM");
//    snapToBPMToggle = new ofxUILabelToggle("Snap to BPM",false,0,0,0,0,OFX_UI_FONT_SMALL);
//    gui->addWidgetSouthOf(snapToBPM, "BPM");
    
    //SETUP OSC CONTROLS
    enableOSCInToggle = new ofxUILabelToggle(translation.translateKey("OSC IN"),false,0,0,0,0, OFX_UI_FONT_MEDIUM);
    enableOSCOutToggle = new ofxUILabelToggle(translation.translateKey("OSC OUT"),false,0,0,0,0, OFX_UI_FONT_MEDIUM);
    oscOutIPInput = new ofxUITextInput("OSCIP", "127.0.0.1",TEXT_INPUT_WIDTH*1.5,0,0,0, OFX_UI_FONT_MEDIUM);
    oscOutIPInput->setAutoClear(false);
	
    oscInPortInput = new ofxUITextInput("OSCINPORT", "12346",TEXT_INPUT_WIDTH*.8,0,0,0, OFX_UI_FONT_MEDIUM);
    oscInPortInput->setAutoClear(false);

    oscOutPortInput = new ofxUITextInput("OSCOUTPORT", "12345",TEXT_INPUT_WIDTH*.8,0,0,0, OFX_UI_FONT_MEDIUM);
    oscOutPortInput->setAutoClear(false);

	gui->addWidgetRight(enableOSCInToggle);
    gui->addWidgetRight(oscInPortInput);
    gui->addWidgetRight(enableOSCOutToggle);
    gui->addWidgetRight(oscOutIPInput);
    gui->addWidgetRight(oscOutPortInput);

	ofAddListener(gui->newGUIEvent, this, &DurationController::guiEvent);
	
	//add events
    ofAddListener(timeline.events().bangFired, this, &DurationController::bangFired);
	ofAddListener(ofEvents().exit, this, &DurationController::exit);

    //SET UP LISENTERS
	enableInterface();
	
    if(settingsLoaded){
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
		lock();
		oscLock.lock();
		handleOscIn();
		handleOscOut();
		oscLock.unlock();
		unlock();
		
		ofSleepMillis(1);
	}
}

void DurationController::handleOscIn(){
	if(!settings.oscInEnabled){
		return;
	}
	
	//TODO: move parsing and receing to separate different threads?
	long timelineStartTime = timeline.getCurrentTimeMillis();
	while(receiver.hasWaitingMessages()){

		ofxOscMessage m;
		receiver.getNextMessage(&m);
		bool handled = false;
		long startTime = recordTimer.getAppTimeMicros();
		vector<ofxTLPage*>& pages = timeline.getPages();
		for(int i = 0; i < pages.size(); i++){
			vector<ofxTLTrack*>& tracks = pages[i]->getTracks();
			for(int t = 0; t < tracks.size(); t++){
//				cout << " testing against " << "/"+tracks[t]->getDisplayName() << endl;
				ofxTLTrack* track = tracks[t];
				ofPtr<ofxTLUIHeader> header = headers[track->getName()];				
				if(header->receiveOSC() && m.getAddress() == ofFilePath::addLeadingSlash(track->getDisplayName()) ){
					
					if(timeline.getIsPlaying() ){ //TODO: change to isPlaying() && isRecording()
						if(track->getTrackType() == "Curves"){
							ofxTLCurves* curves = (ofxTLCurves*)track;
//							cout << "adding value " << m.getArgAsFloat(0) << endl;
							if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
								float value = m.getArgAsFloat(0);
								if(value != header->lastValueReceived || !header->hasReceivedValue){
									curves->addKeyframeAtMillis(value, timelineStartTime);
									header->lastValueReceived = value;
									header->hasReceivedValue = true;
								}
							}
						}
						else if(track->getTrackType() == "Bangs"){
							ofxTLBangs* bangs = (ofxTLBangs*)track;
							bangs->addKeyframeAtMillis(0,timelineStartTime);
						}
					}

					header->lastInputReceivedTime = recordTimer.getAppTimeSeconds();
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
			if(m.getNumArgs() == 1 && m.getArgType(0) == OFXOSC_TYPE_STRING){
				string projectPath = m.getArgAsString(0);
				shouldLoadProject = true;
				if(ofFilePath::isAbsolute(projectPath)){
					projectToLoad = projectPath;
				}
				else{
					projectToLoad = defaultProjectDirectoryPath+projectPath;
				}
			}
			else{
				ofLogError("Duration:OSC") << " Open Project Failed - must have on string argument specifying project name or absolute path";
			}
		}
		else if(m.getAddress() == "/duration/new"){
			if(m.getNumArgs() == 1 && m.getArgType(0) == OFXOSC_TYPE_STRING){
				string path = m.getArgAsString(0);
				shouldCreateNewProject = true;
				if(ofFilePath::isAbsolute(path)){
					newProjectPath = path;
				}
				else{
					newProjectPath = defaultProjectDirectoryPath+path;
				}
				cout << "creating new project at path " << newProjectPath << endl;
			}
			else{
				ofLogError("Duration:OSC") << " New Project Failed - must have on string argument specifying the new project path";
			}
		}
		else if(m.getAddress() == "/duration/save"){
			saveProject();
		}
		else if(m.getAddress() == "/duration/setduration"){
			if(m.getNumArgs() == 1){
				//seconds
				if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
					timeline.setDurationInSeconds(m.getArgAsFloat(0));
					durationLabel->setTextString(timeline.getDurationInTimecode());					
				}
				//timecode
				else if(m.getArgType(0) == OFXOSC_TYPE_STRING){
					timeline.setDurationInTimecode(m.getArgAsString(0));
					durationLabel->setTextString(timeline.getDurationInTimecode());
				}
				//millis
				else if(m.getArgType(0) == OFXOSC_TYPE_INT32){
					timeline.setDurationInMillis(m.getArgAsInt32(0));
					durationLabel->setTextString(timeline.getDurationInTimecode());					
				}
				else if(m.getArgType(0) == OFXOSC_TYPE_INT64){
					timeline.setDurationInMillis(m.getArgAsInt64(0));
					durationLabel->setTextString(timeline.getDurationInTimecode());
				}
			}
			else {
				ofLogError("Duration:OSC") << " Set Duration failed - must have one argument. seconds as float, timecode string HH:MM:SS:MILS, or integer as milliseconds";
			}
		}
		else if(m.getAddress() == "/duration/play"){
			if(m.getNumArgs() == 0){
				if(!timeline.getIsPlaying()){
					shouldStartPlayback = true;
				}
			}
			else {
				for(int i = 0; i < m.getNumArgs(); i++){
					if(m.getArgType(i) == OFXOSC_TYPE_STRING){
						ofPtr<ofxTLUIHeader> header = getHeaderWithDisplayName(m.getArgAsString(i));
						if(header != NULL){
							header->getTrack()->play();
						}
					}
				}
			}
		}
		else if(m.getAddress() == "/duration/stop"){
			if(m.getNumArgs() == 0){
				if(timeline.getIsPlaying()){
					timeline.stop();
				}
				else{
					timeline.setCurrentTimeMillis(0);
				}
			}
			else{
				for(int i = 0; i < m.getNumArgs(); i++){
					if(m.getArgType(i) == OFXOSC_TYPE_STRING){
						ofPtr<ofxTLUIHeader> header = getHeaderWithDisplayName(m.getArgAsString(i));
						if(header != NULL){
							header->getTrack()->stop();
						}
					}
				}				
			}
		}
		else if(m.getAddress() == "/duration/record"){
			//TODO: turn on record mode
			shouldStartPlayback = true;
			//startPlayback();
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
				timeline.setPercentComplete(percent);
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
		else if(m.getAddress() == "/duration/seektotimecode"){
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
		//enable and disable OSC
		else if(m.getAddress() == "/duration/enableoscout"){
			//system wide
			if(m.getNumArgs() == 1 && m.getArgType(0) == OFXOSC_TYPE_INT32){
				settings.oscOutEnabled = m.getArgAsInt32(0) != 0;
				enableOSCOutToggle->setValue(settings.oscOutEnabled);
			}
			//per track
			else if(m.getNumArgs() == 2 &&
					m.getArgType(0) == OFXOSC_TYPE_STRING &&
					m.getArgType(1) == OFXOSC_TYPE_INT32)
			{
				string trackName = m.getArgAsString(0);
				ofPtr<ofxTLUIHeader> header = getHeaderWithDisplayName(trackName);
				if(header != NULL){
					header->setSendOSC(m.getArgAsInt32(1) != 0);
				}
				else {
					ofLogError("Duration:OSC") << " Enable OSC out failed. track not found " << trackName;
				}
			}
			else{
				ofLogError("Duration:OSC") << " Enable OSC out incorrectly formatted arguments. usage: /duration/enableoscout enable:int32 == (1 or 0), or /duration/enableoscout trackname:string enable:int32 (1 or 0)";
			}
		}
		else if(m.getAddress() == "/duration/oscrate"){
			if(m.getNumArgs() == 1){
				if(m.getArgType(0) == OFXOSC_TYPE_INT32){
					settings.oscRate = m.getArgAsInt32(0);
					oscFrequency = 1000*1/settings.oscRate;
				}
				else if(m.getArgType(0) == OFXOSC_TYPE_INT64){
					settings.oscRate = m.getArgAsInt64(0);
					oscFrequency = 1000*1/settings.oscRate;
				}
				else if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
					settings.oscRate = m.getArgAsFloat(0);
					oscFrequency = 1000*1/settings.oscRate;
				}
				else {
					ofLogError("Duration:OSC") << " Set OSC rate failed. must specify an int or a float as the first parameter";
				}
			}
		}
		else if(m.getAddress() == "/duration/enableoscin"){
			//system wide -- don't quite know what to do as this will turn off all osc
			if(m.getNumArgs() == 1 && m.getArgType(0) == OFXOSC_TYPE_INT32){
				settings.oscInEnabled = m.getArgAsInt32(0) != 0;
				enableOSCInToggle->setValue(settings.oscInEnabled);
			}
			//per track
			else if(m.getNumArgs() == 2 && m.getArgType(0) == OFXOSC_TYPE_STRING && m.getArgType(1) == OFXOSC_TYPE_INT32){
				string trackName = m.getArgAsString(0);
				ofPtr<ofxTLUIHeader> header = getHeaderWithDisplayName(trackName);
				if(header != NULL){
					header->setReceiveOSC(m.getArgAsInt32(1) == 1);
				}
				else {
					ofLogError("Duration:OSC") << " Enable in out failed. track not found " << trackName;
				}
			}
			else{
				ofLogError("Duration:OSC") << "Enable OSC in incorrectly formatted arguments. usage: /duration/enableoscout enable:int32 == (1 or 0), or /duration/enableoscout trackname:string enable:int32 (1 or 0)";
			}
		}		
		//adding and removing tracks
		else if(m.getAddress() == "/duration/addtrack"){
			//type,
			receivedAddTrack = false;
			oscTrackTypeReceived = "";
			oscTrackNameReceived = "";
			oscTrackFilePathReceived = "";
			//type
			if(m.getNumArgs() > 0 && m.getArgType(0) == OFXOSC_TYPE_STRING) {
				oscTrackTypeReceived = m.getArgAsString(0);
				receivedAddTrack = true;
			}
			//type, name
			if(m.getNumArgs() > 1 &&m.getArgType(1) == OFXOSC_TYPE_STRING) {
				oscTrackNameReceived = m.getArgAsString(1);
				receivedAddTrack = true;
			}
			//type, name, file path
			if(m.getNumArgs() > 2 && m.getArgType(2) == OFXOSC_TYPE_STRING)
			{
				oscTrackFilePathReceived = m.getArgAsString(2);
				receivedAddTrack = true;
			}
			if(!receivedAddTrack){
				ofLogError("Duration:OSC") << "Add track failed, incorrectly formatted arguments. \n usage: /duration/addtrack type:string [optional name:string ] [optional filepath:string ]";
			}
		}
		else if(m.getAddress() == "/duration/removetrack"){
			if(m.getNumArgs() == 1 && m.getArgType(0) == OFXOSC_TYPE_STRING){
				string trackName = m.getArgAsString(0);
				ofPtr<ofxTLUIHeader> header = getHeaderWithDisplayName(trackName);
				if(header != NULL){
					header->setShouldDelete(true);
				}
				else{
					ofLogError("Duration:OSC") << "Remove track failed, could not find track " << trackName;
				}
			}
			else {
				ofLogError("Duration:OSC") << "Remove track failed, incorrectly formatted arguments. \n usage: /duration/removetrack name:string";
			}
		}
		else if(m.getAddress() == "/duration/trackname"){
			if(m.getNumArgs() == 2 &&
			   m.getArgType(0) == OFXOSC_TYPE_STRING &&
			   m.getArgType(1) == OFXOSC_TYPE_STRING)
			{
				string trackName = m.getArgAsString(0);
				ofPtr<ofxTLUIHeader> header = getHeaderWithDisplayName(trackName);
				if(header != NULL){
					header->getTrack()->setDisplayName(m.getArgAsString(1));
				}
				else{
					ofLogError("Duration:OSC") << "Set Track Name failed, could not find track " << trackName;
				}
			}
			else{
				ofLogError("Duration:OSC") << "Set Track Name failed, incorrectly formatted arguments. \n usage: /duration/trackname oldname:string newname:string";
			}
		}
		else if(m.getAddress() == "/duration/valuerange"){
			if(m.getNumArgs() == 3 &&
			   m.getArgType(0) == OFXOSC_TYPE_STRING && //track name
			   m.getArgType(1) == OFXOSC_TYPE_FLOAT && //min
			   m.getArgType(2) == OFXOSC_TYPE_FLOAT) //max
			{
				string trackName = m.getArgAsString(0);
				ofPtr<ofxTLUIHeader> header = getHeaderWithDisplayName(trackName);
				if(header != NULL){
					if(header->getTrackType() == "Curves" || header->getTrackType() == "LFO"){
						header->setValueRange(ofRange(m.getArgAsFloat(1),m.getArgAsFloat(2)));
					}
					else {
						ofLogError("Duration:OSC") << "Set value range failed, track is not a Curves track " << trackName;
					}
				}
				else{
					ofLogError("Duration:OSC") << "Set value range failed, could not find track " << trackName;
				}
			}
			else {
				ofLogError("Duration:OSC") << "Set value range failed, incorrectly formatted message. \n usage: /duration/valuerange trackname:string min:float max:float";
			}
		}
		else if(m.getAddress() == "/duration/valuerange/min"){
			if(m.getNumArgs() == 2 &&
			   m.getArgType(0) == OFXOSC_TYPE_STRING && //track name
			   m.getArgType(1) == OFXOSC_TYPE_FLOAT) //min
			{
				string trackName = m.getArgAsString(0);
				ofPtr<ofxTLUIHeader> header = getHeaderWithDisplayName(trackName);
				if(header != NULL){
					if(header->getTrackType() == "Curves" || header->getTrackType() == "LFO"){
						header->setValueMin(m.getArgAsFloat(1));
					}
					else{
						ofLogError("Duration:OSC") << "Set value range min failed, track is not a Curves track " << trackName;
					}
				}
				else{
					ofLogError("Duration:OSC") << "Set value range min failed, could not find track " << trackName;
				}
			}
			else{
				ofLogError("Duration:OSC") << "Set value range min failed. Incorrectly formatted arguments \n usage: /duration/valuerange/min trackname:string";
			}
		}
		else if(m.getAddress() == "/duration/valuerange/max"){
			if(m.getNumArgs() == 2 &&
			   m.getArgType(0) == OFXOSC_TYPE_STRING && //track name
			   m.getArgType(1) == OFXOSC_TYPE_FLOAT) //max
			{
				string trackName = m.getArgAsString(0);
				ofPtr<ofxTLUIHeader> header = getHeaderWithDisplayName(trackName);
				if(header != NULL){
					if(header->getTrackType() == "Curves" || header->getTrackType() == "LFO"){
						header->setValueMax(m.getArgAsFloat(1));
					}
					else{
						ofLogError("Duration:OSC") << "Set value range max failed, track is not a Curves track " << trackName;
					}
				}
				else{
					ofLogError("Duration:OSC") << "Set value range min failed, could not find track " << trackName;
				}
			}
			else{
				ofLogError("Duration:OSC") << "Set value range min failed. Incorrectly formatted arguments \n usage: /duration/valuerange/min trackname:string";
			}
		}
		else if(m.getAddress() == "/duration/colorpalette"){
			if(m.getNumArgs() == 2 &&
			   m.getArgType(0) == OFXOSC_TYPE_STRING && //track name
			   m.getArgType(1) == OFXOSC_TYPE_STRING) //file path
			{
				string trackName = m.getArgAsString(0);
				ofPtr<ofxTLUIHeader> header = getHeaderWithDisplayName(trackName);
				if(header != NULL){
					if(header->getTrackType() == "Colors"){
						receivedPaletteToLoad = true;
						paletteTrack = (ofxTLColorTrack*)header->getTrack();
						palettePath  = m.getArgAsString(1);
					}
				}
				else {
					ofLogError("Duration:OSC") << "Set color palette failed, could not find track " << trackName;
				}
			}
			else{
				ofLogError("Duration:OSC") << "Set color palette failed, incorrectly formatted arguments \n usage: /duration/colorpalette trackname:string imagefilepath:string";
			}
		}
#ifdef TARGET_OSX
		else if(m.getAddress() == "/duration/audioclip"){
			if(m.getNumArgs() == 1 && m.getArgType(0) == OFXOSC_TYPE_STRING){
				if(audioTrack != NULL){
					if(!audioTrack->loadSoundfile(m.getArgAsString(0))){
						ofLogError("Duration:OSC") << "Set audio clip failed, clip failed to load. " << m.getArgAsString(0);
					}
				}
				else {
					ofLogError("Duration:OSC") << "Set audio clip failed, first add an audio track to the composition.";
				}
			}
			else{
				ofLogError("Duration:OSC") << "Set audio clip failed, incorrectly formatted arguments. \n usage /duration/audioclip filepath:string ";
			}
		}
#endif
	}
}

void DurationController::handleOscOut(){
	
	if(!settings.oscOutEnabled){
		return;
	}
	
	unsigned long bundleTime = recordTimer.getAppTimeMillis();
	if(lastOSCBundleSent+oscFrequency > bundleTime){
		return;
	}
	//cout << "OSC RATE IS " << settings.oscRate << " osc FREQUENCY is " << oscFrequency << " sending num at record timer " << recordTimer.getAppTimeMillis() << endl;
	
	unsigned long timelineSampleTime = timeline.getCurrentTimeMillis();
	int numMessages = 0;
	ofxOscBundle bundle;
	
	//lock();
	vector<ofxTLPage*>& pages = timeline.getPages();
	for(int i = 0; i < pages.size(); i++){
		vector<ofxTLTrack*>& tracks = pages[i]->getTracks();
		for(int t = 0; t < tracks.size(); t++){
			ofPtr<ofxTLUIHeader> header = headers[tracks[t]->getName()];
			if(!header->sendOSC()){
				continue;
			}
			unsigned long trackSampleTime = tracks[t]->getIsPlaying() ? tracks[t]->currentTrackTime() : timelineSampleTime;
			string trackType = tracks[t]->getTrackType();
			if(trackType == "Curves" || trackType == "Switches" || trackType == "Colors" || trackType == "Audio" || trackType == "LFO"){
				bool messageValid = false;
				ofxOscMessage m;
				if(trackType == "Curves" || trackType == "LFO"){
					ofxTLKeyframes* curves = (ofxTLKeyframes*)tracks[t];
					float value = curves->getValueAtTimeInMillis(trackSampleTime);
					if(value != header->lastFloatSent || !header->hasSentValue || refreshAllOscOut){
						m.addFloatArg(value);
						header->lastFloatSent = value;
						header->hasSentValue = true;
						messageValid = true;
					}
				}
				else if(trackType == "Switches"){
					ofxTLSwitches* switches = (ofxTLSwitches*)tracks[t];
					bool on = switches->isOnAtMillis(trackSampleTime);
					if(on != header->lastBoolSent || !header->hasSentValue || refreshAllOscOut){
						m.addIntArg(on ? 1 : 0);
						header->lastBoolSent = on;
						header->hasSentValue = true;
						messageValid = true;
					}
				}
				else if(trackType == "Colors"){
					ofxTLColorTrack* colors = (ofxTLColorTrack*)tracks[t];
					ofColor color = colors->getColorAtMillis(trackSampleTime);
					if(color != header->lastColorSent || !header->hasSentValue || refreshAllOscOut){
						m.addIntArg(color.r);
						m.addIntArg(color.g);
						m.addIntArg(color.b);
						header->lastColorSent = color;
						header->hasSentValue = true;
						messageValid = true;
					}
				}
#ifdef TARGET_OSX
				else if(trackType == "Audio"){
					ofxTLAudioTrack* audio = (ofxTLAudioTrack*)tracks[t];
					if(audio->getIsPlaying() || timeline.getIsPlaying()){
						vector<float>& bins = audio->getFFTSpectrum(header->getNumberOfBins());
						for(int b = 0; b < bins.size(); b++){
							m.addFloatArg(bins[b]);
						}
						messageValid = true;
					}
				}
#endif
				if(messageValid){
					m.setAddress(ofFilePath::addLeadingSlash(tracks[t]->getDisplayName()));
					bundle.addMessage(m);
					numMessages++;
				}
			}
		}
	}
	//unlock();
	
	//any bangs that came our way this frame send them out too
	for(int i = 0; i < bangsReceived.size(); i++){
//		cout << "FOUND BANGS!" << endl;
		bundle.addMessage(bangsReceived[i]);
	}
	numMessages += bangsReceived.size();
	if(numMessages > 0){
		sender.sendBundle(bundle);
		refreshAllOscOut = false;

	}
	lastOSCBundleSent = bundleTime;
	bangsReceived.clear();	
}

//TODO: hook up to record button
//and make NO LOOP 
void DurationController::startRecording(){
	recordTimer.setStartTime();
	recordTimeOffset = timeline.getCurrentTimeMillis();
	//timeline.play();
	startPlayback();
}

void DurationController::stopRecording(){

}

//--------------------------------------------------------------
void DurationController::bangFired(ofxTLBangEventArgs& bang){
// 	ofLogNotice() << "Bang from " << bang.track->getDisplayName() << " at time " << bang.currentTime << " with flag " << bang.flag;
	if(!settings.oscOutEnabled){
		return;
	}
	
    string trackType = bang.track->getTrackType();
    if(!headers[bang.track->getName()]->sendOSC()){
        return;
    }
    ofxOscMessage m;
    m.setAddress( ofFilePath::addLeadingSlash(bang.track->getDisplayName()) );

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
    
	if(e.widget == stopButton && stopButton->getValue()){
		if(timeline.getIsPlaying()){
	        timeline.stop();
		}
		else{
	        timeline.setCurrentTimeMillis(0);
		}
    }
    else if(name == "PLAYPAUSE"){
		if(!timeline.getIsPlaying()){
			startPlayback();
		}
		else{
			timeline.stop();
		}
    }
    else if(name == "DURATION"){
		if(!gui->hasKeyboardFocus()){
			string newDuration = durationLabel->getTextString();
			timeline.setDurationInTimecode(newDuration);
			durationLabel->setTextString(timeline.getDurationInTimecode());
			needsSave = true;
		}
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
				addTrack(translation.keyForTranslation(selectedTrackType));
				unlock();
				
                addTrackDropDown->clearSelected();
            }
        }
    }
    else if(e.widget == projectDropDown){
        if(projectDropDown->isOpen()){
            timeline.disable();
			addTrackDropDown->setVisible(false);
        }
		else {
			addTrackDropDown->setVisible(true);
			addTrackDropDown->close();
            timeline.enable();
            if(projectDropDown->getSelected().size() > 0){
                string selectedProjectName = projectDropDown->getSelected()[0]->getName();
                if(selectedProjectName == translation.translateKey("new project...")){
                    shouldCreateNewProject = true;
                }
                else if(selectedProjectName == translation.translateKey("open project...")){
                    shouldLoadProject = true;
					projectToLoad = "";
                }
                else {
					shouldLoadProject = true;
					projectToLoad = ofToDataPath(defaultProjectDirectoryPath+selectedProjectName);
                }
                projectDropDown->clearSelected();
            }
        }
    }
    else if(e.widget == saveButton && saveButton->getValue()){
        saveProject();
    }
    //LOOP
    else if(e.widget == loopToggle){
        timeline.setLoopType(loopToggle->getValue() ? OF_LOOP_NORMAL : OF_LOOP_NONE);
		needsSave = true;
    }
    //BPM
	else if(e.widget == bpmDialer){
		if(settings.bpm != bpmDialer->getValue()){
	    	timeline.setBPM(settings.bpm = bpmDialer->getValue());
			needsSave = true;
		}
	}
    else if(e.widget == useBPMToggle){
        settings.useBPM = useBPMToggle->getValue();
        timeline.setShowBPMGrid(settings.useBPM);
        timeline.enableSnapToBPM(settings.useBPM);
		needsSave = true;
    }
	else if(e.widget == snapToKeysToggle){
		timeline.enableSnapToOtherKeyframes(snapToKeysToggle->getValue());
	}
    //OSC INPUT
    else if(e.widget == enableOSCInToggle){
		settings.oscInEnabled = enableOSCInToggle->getValue();
        if(settings.oscInEnabled){
			oscLock.lock();
            receiver.setup(settings.oscInPort);
			oscLock.unlock();
        }
		needsSave = true;		
    }
	//INCOMING PORT
	else if(e.widget == oscInPortInput){
		if(!gui->hasKeyboardFocus()){
			int newPort = ofToInt(oscInPortInput->getTextString());
			if(newPort > 0 && newPort < 65535 &&
			   newPort != settings.oscInPort &&
			   //don't send messages to ourself
			   (newPort != settings.oscOutPort || (settings.oscIP != "localhost" && settings.oscIP != "127.0.0.1"))){
				settings.oscInPort = newPort;
				oscLock.lock();
				receiver.setup(settings.oscInPort);
				oscLock.unlock();
				needsSave = true;
			}
			else {
				oscInPortInput->setTextString( ofToString(settings.oscInPort) );
			}
		}
    }
	
	//OSC OUTPUT
    else if(e.widget == enableOSCOutToggle){
		settings.oscOutEnabled = enableOSCOutToggle->getValue();
        if(settings.oscOutEnabled){
			oscLock.lock();
            sender.setup(settings.oscIP, settings.oscOutPort);
			oscLock.unlock();
			needsSave = true;			
        }
    }

	//OUTGOING IP
    else if(e.widget == oscOutIPInput && !gui->hasKeyboardFocus()){
        string newIP = ofToLower(oscOutIPInput->getTextString());
        if(newIP == settings.oscIP){
            return;
        }
		
        bool valid = (newIP == "localhost");
		if(!valid){
			vector<string> ipComponents = ofSplitString(newIP, ".");
			if(ipComponents.size() == 4){
				valid = true;
				for(int i = 0; i < 4; i++){
					int component = ofToInt(ipComponents[i]);
					if (component < 0 || component > 255){
						valid = false;
						break;
					}
				}
			}
		}

		if((newIP == "127.0.0.1" || newIP == "localhost") && settings.oscInPort == settings.oscOutPort){
			//don't allow us to send messages to ourself
			valid = false;
		}
		
		if(valid){
			settings.oscIP = newIP;
			oscLock.lock();
			sender.setup(settings.oscIP, settings.oscOutPort);
			oscLock.unlock();
			needsSave = true;			
		}
		oscOutIPInput->setTextString(settings.oscIP);
    }
	//OUTGOING PORT
	else if(e.widget == oscOutPortInput && !gui->hasKeyboardFocus()){
        int newPort = ofToInt(oscOutPortInput->getTextString());
        if(newPort > 0 && newPort < 65535 &&
		   newPort != settings.oscOutPort &&
		   //don't send messages to ourself
		   (newPort != settings.oscInPort || (settings.oscIP != "localhost" && settings.oscIP != "127.0.0.1"))){
            settings.oscOutPort = newPort;
			oscLock.lock();
			sender.setup(settings.oscIP, settings.oscOutPort);
			oscLock.unlock();
			needsSave = true;			
        }
        else {
            oscOutPortInput->setTextString( ofToString(settings.oscOutPort) );
        }
    }
}

//--------------------------------------------------------------
ofxTLTrack* DurationController::addTrack(string trackType, string trackName, string xmlFileName){
	ofxTLTrack* newTrack = NULL;
	
	trackType = ofToLower(trackType);
	if(trackName == ""){
		trackName = trackType;
	}

	if(xmlFileName == ""){
		string uniqueName = timeline.confirmedUniqueName(trackName);
		xmlFileName = ofToDataPath(settings.path + "/" + uniqueName + "_.xml");
	}
	
	if(trackType == translation.translateKey("bangs") || trackType == "bangs"){
		newTrack = timeline.addBangs(trackName, xmlFileName);
	}
	else if(trackType == translation.translateKey("flags") || trackType == "flags"){
		newTrack = timeline.addFlags(trackName, xmlFileName);
	}
	else if(trackType == translation.translateKey("curves") || trackType == "curves"){
		newTrack = timeline.addCurves(trackName, xmlFileName);
	}
	else if(trackType == translation.translateKey("switches")|| trackType == "switches"){
		newTrack = timeline.addSwitches(trackName, xmlFileName);
	}
	else if(trackType == translation.translateKey("colors") || trackType == "colors"){
		newTrack = timeline.addColors(trackName, xmlFileName);
	}
	else if(trackType == translation.translateKey("lfo") || trackType == "lfo"){
		newTrack = timeline.addLFO(trackName, xmlFileName);
	}
#ifdef TARGET_OSX
	else if(trackType == translation.translateKey("audio") || trackType == "audio"){
		if(audioTrack != NULL){
			ofLogError("DurationController::addTrack") << "Trying to add an additional audio track";
		}
		else{
			audioTrack = new ofxTLAudioTrack();
			timeline.addTrack(trackName, audioTrack);
			timeline.bringTrackToTop(audioTrack);
			newTrack = audioTrack;
		}
	}
#endif
	else {
		ofLogError("DurationController::addTrack") << "Unsupported track type: " << trackType;
	}
	
	if(newTrack != NULL){
		createHeaderForTrack(newTrack);
		needsSave = true;
	}
	return newTrack;
}

//--------------------------------------------------------------
void DurationController::update(ofEventArgs& args){
	gui->update();
	
	if(shouldStartPlayback){
		shouldStartPlayback = false;
		startPlayback();
	}
	timeLabel->setLabel(timeline.getCurrentTimecode());
	playpauseToggle->setValue(timeline.getIsPlaying());

#ifdef TARGET_OSX
	if(audioTrack != NULL && audioTrack->isSoundLoaded()){
		
		if(timeline.getTimecontrolTrack() != audioTrack){
			timeline.setTimecontrolTrack(audioTrack);
		}

		if(audioTrack->getDuration() != timeline.getDurationInSeconds()){
			timeline.setDurationInSeconds(audioTrack->getDuration());
		}
		
		if(durationLabel->getTextString() != timeline.getDurationInTimecode()){
			durationLabel->setTextString(timeline.getDurationInTimecode());
		}
	}
#endif
	
	if(ofGetHeight() < timeline.getDrawRect().getMaxY()){
		ofSetWindowShape(ofGetWidth(), timeline.getDrawRect().getMaxY()+30);
	}
    if(shouldLoadProject){
        shouldLoadProject = false;
		if(projectToLoad != ""){
			loadProject(projectToLoad);
			projectToLoad = "";
		}
		else{
			ofFileDialogResult r = ofSystemLoadDialog("Load Project", true);
			if(r.bSuccess){
				loadProject(r.getPath(), r.getName());
			}
		}
    }
    
    if(shouldCreateNewProject){
        shouldCreateNewProject = false;
		if(newProjectPath != ""){
			newProject(newProjectPath);
			newProjectPath = "";
		}
		else{
			ofFileDialogResult r = ofSystemSaveDialog("New Project", "NewDuration");
			if(r.bSuccess){
				newProject(r.getPath(), r.getName());
			}
		}
    }

	if(receivedPaletteToLoad){
		receivedPaletteToLoad = false;
		if(!paletteTrack->loadColorPalette(palettePath)){
			ofLogError("Duration:OSC") << "Set color palette failed, file not found";
		}
	}

	if(receivedAddTrack){
		lock();
		receivedAddTrack = false;
		addTrack(oscTrackTypeReceived, oscTrackNameReceived, oscTrackFilePathReceived);
		unlock();
	}
    
    //check if we deleted an element this frame
    map<string,ofPtr<ofxTLUIHeader> >::iterator it = headers.begin();
    while(it != headers.end()){
		
		needsSave |= it->second->getModified();
		
		if(timeline.isModal() && it->second->getGui()->isEnabled()){
			it->second->getGui()->disable();
		}
		else if(!timeline.isModal() && !it->second->getGui()->isEnabled()){
			it->second->getGui()->enable();
		}
		
		if(it->second->getShouldDelete()){
			lock();
            timeline.removeTrack(it->first);
			timeline.setTimecontrolTrack(NULL);
#ifdef TARGET_OSX
			if(it->second->getTrackType() == "Audio"){
				if(audioTrack == NULL){
					ofLogError("Audio track inconsistency");
				}
				else{
					delete audioTrack;
					audioTrack = NULL;
				}
			}
#endif
            headers.erase(it);
			unlock();
			needsSave = true;
            break;
        }
        it++;
    }
}

//--------------------------------------------------------------
ofPtr<ofxTLUIHeader> DurationController::getHeaderWithDisplayName(string name){
	map<string, ofPtr<ofxTLUIHeader> >::iterator trackit;
	for(trackit = headers.begin(); trackit != headers.end(); trackit++){
		if(trackit->second->getTrack()->getDisplayName() == name){
			return trackit->second;
		}
	}
	//same as null
	return ofPtr<ofxTLUIHeader>();
}

//--------------------------------------------------------------
void DurationController::draw(ofEventArgs& args){

	//go through and draw all the overlay backgrounds to indicate 'hot' track sfor recording
	ofPushStyle();
	map<string, ofPtr<ofxTLUIHeader> >::iterator trackit;
	for(trackit = headers.begin(); trackit != headers.end(); trackit++){
		//TODO: check to make sure recording is enabled on this track
		//TODO: find a way to illustrate 'invalid' output sent to this track
		float timeSinceInput = recordTimer.getAppTimeSeconds() - trackit->second->lastInputReceivedTime;
		if(timeSinceInput > 0 && timeSinceInput < 1.0){
			//oscilating red to indicate active
			ofSetColor(200,20,0,(1-timeSinceInput)*(80 + (20*sin(ofGetElapsedTimef()*8)*.5+.5)));
			ofRect(trackit->second->getTrack()->getDrawRect());
		}
	}
	ofPopStyle();
	
	timeline.draw();
	gui->draw();

	if(needsSave || timeline.hasUnsavedChanges()){
		ofPushStyle();
		ofSetColor(200,20,0, 40);
		ofFill();
		ofRect(*saveButton->getRect());
		ofPopStyle();
	}
	drawTooltips();
	//drawTooltipDebug();
}

//--------------------------------------------------------------
void DurationController::keyPressed(ofKeyEventArgs& keyArgs){
    if(timeline.isModal()){
        return;
    }
	
	if(gui->hasKeyboardFocus()){
		return;
	}
	
    int key = keyArgs.key;
	if(key == ' '){
		if(ofGetModifierShiftPressed()){
			timeline.togglePlaySelectedTrack();
		}
		else{
			if(!timeline.getIsPlaying()){
				startPlayback();
			}
			else{
				timeline.stop();
			}
		}
    }
    
    if(key == 'i'){
		if(ofGetModifierAltPressed()){
			timeline.setInPointAtMillis(0);
		}
		else{
	        timeline.setInPointAtPlayhead();
		}
    }
    
    if(key == 'o'){
		if(ofGetModifierAltPressed()){
			timeline.setOutPointAtPercent(1.0);
		}
		else{
	        timeline.setOutPointAtPlayhead();
		}
    }
	
	
	if(ofGetModifierShortcutKeyPressed() && (key == 's' || key=='s'-96) ){
		saveProject();
	}
	
}

//--------------------------------------------------------------
void DurationController::startPlayback(){
	if(!timeline.getIsPlaying()){
		sendInfoMessage();
		timeline.play();
	}
}

//--------------------------------------------------------------
void DurationController::sendInfoMessage(){
	if(settings.oscOutEnabled){
		ofxOscMessage m;
		m.setAddress("/duration/info");		
		vector<ofxTLPage*>& pages = timeline.getPages();
		for(int i = 0; i < pages.size(); i++){
			vector<ofxTLTrack*>& tracks = pages[i]->getTracks();
			for (int t = 0; t < tracks.size(); t++) {
				m.addStringArg(tracks[t]->getTrackType());
				m.addStringArg(ofFilePath::addLeadingSlash( tracks[t]->getDisplayName() ));
				if(tracks[t]->getTrackType() == "Curves" || tracks[t]->getTrackType() == "LFO"){
					ofxTLCurves* curves = (ofxTLCurves*)tracks[t];
					m.addFloatArg(curves->getValueRange().min);
					m.addFloatArg(curves->getValueRange().max);
				}
			}
		}
		oscLock.lock();
		sender.sendMessage(m);
		refreshAllOscOut = true;
		oscLock.unlock();
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
    
	settings.oscRate = 30;
    settings.oscOutEnabled = true;
	settings.oscInEnabled = true;
    settings.oscInPort = 12346;
    settings.oscIP = "localhost";
    settings.oscOutPort = 12345;
    return settings;
}

//--------------------------------------------------------------
void DurationController::newProject(string projectPath){
	//scrape off the last component of the filename for the project name
	projectPath = ofFilePath::removeTrailingSlash(projectPath);
#ifdef TARGET_WIN32
	vector<string> pathComponents = ofSplitString(projectPath, "\\");
#else
	vector<string> pathComponents = ofSplitString(projectPath, "/");
#endif
	newProject(projectPath, pathComponents[pathComponents.size()-1]);
}

//--------------------------------------------------------------
void DurationController::newProject(string newProjectPath, string newProjectName){
    DurationProjectSettings newProjectSettings = defaultProjectSettings();
    newProjectSettings.name = newProjectName;
    newProjectSettings.path = ofToDataPath(newProjectPath);
    newProjectSettings.settingsPath = ofToDataPath(newProjectSettings.path + "/.durationproj");
#ifdef TARGET_WIN32
	ofStringReplace(newProjectSettings.path,"/", "\\");
#endif
    ofDirectory newProjectDirectory(newProjectSettings.path);
    if(newProjectDirectory.exists()){
    	ofSystemAlertDialog(translation.translateKey("Error creating new project. The folder already exists.")+" " + newProjectSettings.path);
        return;
    }
    if(!newProjectDirectory.create(true)){
    	ofSystemAlertDialog(translation.translateKey("Error creating new project. The folder could not be created.")+" " + newProjectSettings.path);
        return;
    }
    
    //TODO: prompt to save existing project
    settings = newProjectSettings;
	lock();
    headers.clear(); //smart pointers will call destructor
    timeline.reset();
	unlock();
	
    //saves file with default settings to new directory
    saveProject();
    
    loadProject(settings.path, settings.name);
    
    projectDropDown->addToggle(newProjectName);
}

//--------------------------------------------------------------
void DurationController::loadProject(string projectPath, bool forceCreate){
	//scrape off the last component of the filename for the project name
	projectPath = ofFilePath::removeTrailingSlash(projectPath);
#ifdef TARGET_WIN32
	vector<string> pathComponents = ofSplitString(projectPath, "\\");
#else
	vector<string> pathComponents = ofSplitString(projectPath, "/");
#endif	
	loadProject(projectPath, pathComponents[pathComponents.size()-1], forceCreate);
}

//--------------------------------------------------------------
void DurationController::loadProject(string projectPath, string projectName, bool forceCreate){    
    ofxXmlSettings projectSettings;
	string projectDataPath = ofToDataPath(projectPath+"/.durationproj");
	if(!projectSettings.loadFile(projectDataPath)){
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
    timeline.reset();
#ifdef TARGET_OSX
	if(audioTrack != NULL){
		delete audioTrack;
		audioTrack = NULL;
	}
#endif
    timeline.setWorkingFolder(projectPath);
	
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
            string trackType = ofToLower(projectSettings.getValue("type", ""));
            string xmlFileName = projectSettings.getValue("xmlFileName", "");
            string trackName = projectSettings.getValue("trackName","");
            string trackFilePath = ofToDataPath(projectPath + "/" + xmlFileName);
			
			//add the track
            ofxTLTrack* newTrack = addTrack(trackType, trackName, trackFilePath);

			//custom setup
			if(newTrack != NULL){
				ofPtr<ofxTLUIHeader> headerTrack = headers[newTrack->getName()];
				if(newTrack->getTrackType() == "Curves" || newTrack->getTrackType() == "LFO"){
					headerTrack->setValueRange(ofRange(projectSettings.getValue("min", 0.0),
													   projectSettings.getValue("max", 1.0)));
				}
				else if(newTrack->getTrackType() == "Colors"){
					ofxTLColorTrack* colors = (ofxTLColorTrack*)newTrack;
					colors->loadColorPalette(projectSettings.getValue("palette", timeline.getDefaultColorPalettePath()));
				}
				#ifdef TARGET_OSX
				else if(newTrack->getTrackType() == "Audio"){
					string clipPath = projectSettings.getValue("clip", "");
					if(clipPath != ""){
						audioTrack->loadSoundfile(clipPath);
					}
					int numbins = projectSettings.getValue("bins", 256);
					headerTrack->setNumberOfbins(numbins);
					cout << "set " << numbins << " after load " << headerTrack->getNumberOfBins() << endl;
					//audioTrack->getFFTSpectrum(projectSettings.getValue("bins", 256));
				}
				#endif

				string displayName = projectSettings.getValue("displayName","");
				if(displayName != ""){
					newTrack->setDisplayName(displayName);
				}

				headerTrack->setSendOSC(projectSettings.getValue("sendOSC", true));
				headerTrack->setReceiveOSC(projectSettings.getValue("receiveOSC", true));
			}
            projectSettings.popTag(); //track
        }
        projectSettings.popTag(); //page
    }
	unlock();
	
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
	newSettings.oscRate = projectSettings.getValue("oscRate", 30.0);
	oscFrequency = 1000 * 1/newSettings.oscRate; //frequence in millis
	
    projectSettings.popTag(); //project settings;
	
    newSettings.path = projectPath;
    newSettings.name = projectName;
    newSettings.settingsPath = ofToDataPath(newSettings.path + "/.durationproj");
    settings = newSettings;
	
    projectDropDown->setLabelText(projectName);
    timeline.setShowBPMGrid(newSettings.useBPM);
    timeline.enableSnapToBPM(newSettings.useBPM);
	timeline.setBPM(newSettings.bpm);
    
	oscLock.lock();
	if(settings.oscInEnabled){
		receiver.setup(settings.oscInPort);
	}
	if(settings.oscOutEnabled){
        sender.setup(settings.oscIP, settings.oscOutPort);
    }
	oscLock.unlock();
	
    ofxXmlSettings defaultSettings;
    defaultSettings.loadFile("settings.xml");
    defaultSettings.setValue("lastProjectPath", settings.path);
    defaultSettings.setValue("lastProjectName", settings.name);
    defaultSettings.saveFile();
	
	needsSave = false;
	sendInfoMessage();
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
        projectSettings.pushTag("page", i);
        projectSettings.addValue("name", pages[i]->getName());
        vector<ofxTLTrack*>& tracks = pages[i]->getTracks();
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
            projectSettings.addValue("sendOSC", headers[trackName]->sendOSC());
			projectSettings.addValue("receiveOSC", headers[trackName]->receiveOSC());
            if(trackType == "Curves" || trackType == "LFO"){
                ofxTLKeyframes* curves = (ofxTLKeyframes*)tracks[t];
                projectSettings.addValue("min", curves->getValueRange().min);
                projectSettings.addValue("max", curves->getValueRange().max);
				
            }
			else if(trackType == "Colors"){
				ofxTLColorTrack* colors = (ofxTLColorTrack*)tracks[t];
				projectSettings.addValue("palette", colors->getPalettePath());
			}
#ifdef TARGET_OSX
			else if(trackType == "Audio"){
				projectSettings.addValue("clip", audioTrack->getSoundfilePath());
				int numbins = audioTrack->getFFTBinCount();
				projectSettings.addValue("bins", audioTrack->getFFTBinCount());
			}
#endif
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
    projectSettings.addValue("oscOutPort", settings.oscOutPort);
	projectSettings.addValue("oscRate", settings.oscRate);
	
//	projectSettings.addValue("zoomViewMin",timeline.getZoomer()->getSelectedRange().min);
//	projectSettings.addValue("zoomViewMax",timeline.getZoomer()->getSelectedRange().max);

	projectSettings.popTag(); //projectSettings
    projectSettings.saveFile(settings.settingsPath);
	
	needsSave = false;
}

//--------------------------------------------------------------
ofxTLUIHeader* DurationController::createHeaderForTrack(ofxTLTrack* track){
    ofxTLUIHeader* headerGui = new ofxTLUIHeader();
	headerGui->translation = &translation;
    ofxTLTrackHeader* header = timeline.getTrackHeader(track);
    headerGui->setTrackHeader(header);
    headers[track->getName()] = ofPtr<ofxTLUIHeader>( headerGui );
    return headerGui;
}

void DurationController::createTooltips(){

	//switch project
	Tooltip projectTip;
	projectTip.text = translation.translateKey("switch project");
	projectTip.sourceRect = *projectDropDown->getRect();
	projectTip.displayPoint = ofVec2f(projectTip.sourceRect.x, 55);
	tooltips.push_back(projectTip);
	
	//save
	Tooltip saveTip;
	saveTip.text = translation.translateKey("save");
	saveTip.sourceRect = *saveButton->getRect();
	saveTip.displayPoint = ofVec2f(saveTip.sourceRect.x, 55);
	tooltips.push_back(saveTip);
		
	//play/pause
	Tooltip playpauseTip;
	playpauseTip.text = translation.translateKey("play")+"/"+translation.translateKey("pause"); //TODO: switch dynamically
	playpauseTip.sourceRect = *playpauseToggle->getRect();
	playpauseTip.displayPoint = ofVec2f(playpauseTip.sourceRect.x, 55);
	tooltips.push_back(playpauseTip);

	ofVec2f zone2 = playpauseTip.displayPoint;
	
	//edit duration
	Tooltip editDurationTip;
	editDurationTip.text = translation.translateKey("edit duration");
	editDurationTip.displayPoint = zone2;
	editDurationTip.sourceRect = *durationLabel->getRect();
	tooltips.push_back(editDurationTip);

	//current time
	Tooltip currentTimeTip;
	currentTimeTip.text = translation.translateKey("current time");
	currentTimeTip.displayPoint = zone2;
	currentTimeTip.sourceRect = *timeLabel->getRect();
	tooltips.push_back(currentTimeTip);
	
	//stop
	Tooltip stopTip;
	stopTip.text = translation.translateKey("stop");
	stopTip.sourceRect = *stopButton->getRect();
	stopTip.displayPoint = ofVec2f(stopTip.sourceRect.x, 55);
	tooltips.push_back(stopTip);
	
	//loop
	Tooltip loopTip;
	loopTip.text = translation.translateKey("toggle loop");
	loopTip.sourceRect = *loopToggle->getRect();
	loopTip.displayPoint = ofVec2f(loopTip.sourceRect.x, 55);
	tooltips.push_back(loopTip);

	//enable Snap to BPM
	Tooltip bpmTip;
	bpmTip.text = translation.translateKey("snap to measures");
	bpmTip.sourceRect = *useBPMToggle->getRect();
	bpmTip.displayPoint = ofVec2f(bpmTip.sourceRect.x, 55);
	tooltips.push_back(bpmTip);
	
	//set beats per minute
	Tooltip setBpmTip;
	setBpmTip.text = translation.translateKey("set beats per minute");
	setBpmTip.sourceRect = *bpmDialer->getRect();
	setBpmTip.displayPoint = ofVec2f(setBpmTip.sourceRect.x, 55);
	tooltips.push_back(setBpmTip);
	
	//enable OSC
	Tooltip oscInTip;
	oscInTip.text = translation.translateKey("enable incoming OSC");
	oscInTip.sourceRect = *enableOSCInToggle->getRect();
	oscInTip.displayPoint = ofVec2f(oscInTip.sourceRect.x, 55);
	tooltips.push_back(oscInTip);

	//osc In Port
	Tooltip oscInPortTip;
	oscInPortTip.text = translation.translateKey("incoming OSC port");
	oscInPortTip.sourceRect = *oscInPortInput->getRect();
	oscInPortTip.displayPoint = ofVec2f(oscInPortTip.sourceRect.x, 55);
	tooltips.push_back(oscInPortTip);

	//osc Out
	Tooltip oscOutTip;
	oscOutTip.text = translation.translateKey("enable outgoing OSC");
	oscOutTip.sourceRect = *enableOSCOutToggle->getRect();
	oscOutTip.displayPoint = ofVec2f(oscOutTip.sourceRect.x, 55);
	tooltips.push_back(oscOutTip);

	//osc Out IP
	Tooltip oscOutIPTip;
	oscOutIPTip.text = translation.translateKey("remote IP");
	oscOutIPTip.sourceRect = *oscOutIPInput->getRect();
	oscOutIPTip.displayPoint = ofVec2f(oscOutIPTip.sourceRect.x, 55);
	tooltips.push_back(oscOutIPTip);

	//osc Out IP
	Tooltip oscOutPortTip;
	oscOutPortTip.text = translation.translateKey("remote port");
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
	lock();
	timeline.removeFromThread();
	headers.clear();
	timeline.reset();
	unlock();
	
	ofLogNotice("DurationController") << "waiting for thread on exit";
	waitForThread(true);
}
