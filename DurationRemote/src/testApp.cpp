#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofBackground(255*.15);
	
	gui = new ofxUICanvas(0,0, ofGetWidth(), ofGetHeight());
	gui->setWidgetFontSize(OFX_UI_FONT_LARGE);
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	gui->addTextInput("IP", "localhost", 300);
	gui->addTextInput("PORT", "12346", 150);
	
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	commandInput = gui->addTextInput("COMMAND", "/duration/command", 300);
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	arg1 = gui->addTextInput("ARG1", "arg1", 150);
	arg2 = gui->addTextInput("ARG2", "arg2", 150);
	arg3 = gui->addTextInput("ARG3", "arg3", 150);
	sendButton = gui->addLabelButton("send >>", false);
	commandInput->setAutoClear(false);
	arg1->setAutoClear(false);
	arg2->setAutoClear(false);
	arg3->setAutoClear(false);
	
	//play controls
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	playButton = gui->addLabelButton("play", false);
	
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	stopButton = gui->addLabelButton("stop", false);


	//TODO add duration setting
	//ADD TRACKS
    vector<string> trackTypes;
    trackTypes.push_back("bangs");
    trackTypes.push_back("flags");
    trackTypes.push_back("switches");
    trackTypes.push_back("curves");
    trackTypes.push_back("colors");
	trackTypes.push_back("audio");
	
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    addTrackDropdown = gui->addDropDownList("track type", trackTypes);
	addTrackDropdown->setAutoClose(true);
	addTrackDropdown->setAllowMultiple(false);
	
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	addTrackNameInput = gui->addTextInput("TRACK NAME", "track name", 200);
	addTrackNameInput->setAutoClear(false);
	filePathInput = gui->addTextInput("TRACK FILEPATH", "track filepath [optional]", 200);
	filePathInput->setAutoClear(false);
	addTrackButton = gui->addLabelButton("add track >>", false);
	
	//REMOVE track
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);	
	removeTrackNameInput = gui->addTextInput("REMOVE TRACK", "remove track name", 200);
	removeTrackNameInput->setAutoClear(false);
	
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	removeTrackButton = gui->addLabelButton("remove track >>", false);

	seekRect = ofRectangle(10, 250, ofGetWidth()-20, 50);
	
	ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
	
	gui->enable();
	
	redColor = ofColor(250, 200, 80);
	blueColor = redColor.getInverted();
	seekPercent = 0;
	hoveringSeek = false;
	sender.setup("localhost", 12346);
}

//number test from http://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
bool isNumber(const string& s){
	std::string::const_iterator it = s.begin();
    while (it != s.end() && (std::isdigit(*it) || *it == '.')) ++it;
    return !s.empty() && it == s.end();
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
	if(e.widget == sendButton && sendButton->getValue()){
		sendCurrentCommand();
	}
	else if(e.widget == playButton && playButton->getValue()){
		clearArgs();
		commandInput->setTextString("/duration/play");
		sendCurrentCommand();
	}
	else if(e.widget == stopButton && stopButton->getValue()){
		clearArgs();
		commandInput->setTextString("/duration/stop");
		sendCurrentCommand();
	}
	else if(e.widget == addTrackDropdown && !addTrackDropdown->isOpen() && addTrackDropdown->getSelected().size() == 1){
		addTrackDropdown->setLabelText(addTrackDropdown->getSelected()[0]->getName());
	}
	else if(e.widget == addTrackButton &&
			addTrackButton->getValue() &&
			addTrackDropdown->getSelected().size() == 1 &&
			addTrackNameInput->getTextString() != "")
	{
		clearArgs();
		commandInput->setTextString("/duration/addtrack");
		arg1->setTextString(addTrackDropdown->getSelected()[0]->getName());
		arg2->setTextString(addTrackNameInput->getTextString());
		if(filePathInput->getTextString().find(".xml") != string::npos){
			arg3->setTextString(filePathInput->getTextString());
		}
		sendCurrentCommand();
	}
	else if(e.widget == removeTrackButton && removeTrackButton->getValue()){
		clearArgs();
		commandInput->setTextString("/duration/removetrack");
		arg1->setTextString(removeTrackNameInput->getTextString());
		sendCurrentCommand();
	}
}

//--------------------------------------------------------------
void testApp::sendCurrentCommand(){
	string command = commandInput->getTextString();
	if(command != ""){
		cout << "sending command " << command << endl;
		ofxOscMessage m;
		m.setAddress(command);
		vector<string> args;
		args.push_back( arg1->getTextString() );
		args.push_back( arg2->getTextString() );
		args.push_back( arg3->getTextString() );
		for(int i = 0; i < args.size(); i++){
			
			if(args[i] == "" || args[i] == " "){
				break;
			}
			
			 if( isNumber(args[i]) ){
				if(args[i].find(".") != string::npos){
					cout << " added " << args[i] << " as float " << endl;
					m.addFloatArg(ofToFloat(args[i]));
				}
				else{
					m.addIntArg(ofToInt(args[i]));
					cout << " added " << args[i] << " as int " << endl;
				}
			}
			else{
				m.addStringArg(args[i]);
				cout << " added " << args[i] << " as string " << endl;
			}
		}
		sender.sendMessage(m);
	}
}

//--------------------------------------------------------------
void testApp::clearArgs(){
	arg1->setTextString(" ");
	arg2->setTextString(" ");
	arg3->setTextString(" ");
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofPushStyle();
	
	if(hoveringSeek){
		ofFill();
		ofSetColor(blueColor, 75+50 * sin(ofGetElapsedTimef()*10)*.5+.5);
		ofRect(seekRect);
	}
	
	ofNoFill();
	ofSetColor(redColor);
	ofRect(seekRect);
	
	ofSetLineWidth(3);
	ofLine(seekRect.x + seekPercent*seekRect.width, seekRect.getMinY(),
		   seekRect.x + seekPercent*seekRect.width, seekRect.getMaxY());
	
	ofPopStyle();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	hoveringSeek = seekRect.inside(x, y);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if(draggingSeek){
		seekPercent = ofMap(x, seekRect.getMinX(), seekRect.getMaxX(), 0, 1.0, true);
		clearArgs();
		commandInput->setTextString("/duration/seektoposition");
		arg1->setTextString(ofToString(seekPercent, 4));
		sendCurrentCommand();
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	draggingSeek = seekRect.inside(x, y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	draggingSeek = false;
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