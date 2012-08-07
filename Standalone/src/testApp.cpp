#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
    //setup OF
    ofSetFrameRate(120);
    ofBackground(.15*255);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    //Set up top GUI
    gui = new ofxUICanvas(0,0,ofGetWidth(), 375);     
    
    //ADD TIMECODE
    string zeroTimecode = "00:00:00:000";
    timeLabel = new ofxUILabel(zeroTimecode, OFX_UI_FONT_LARGE);
    gui->addWidgetDown(timeLabel); 
    
    //ADD PLAY/PAUSE
    playpause = new ofxUIMultiImageToggle(32, 32, false, "GUI/play_.png", "PLAYPAUSE");
    playpause->setLabelVisible(false);
    gui->addWidgetDown(playpause); 
    gui->addWidgetRight(new ofxUIImageButton(32, 32, false, "GUI/stop2.png", "STOP"));  
	
    //ADD TRACKS
    vector<string> trackTypes; 
    trackTypes.push_back("BANGS");    
    trackTypes.push_back("FLAGS");    
    trackTypes.push_back("SWITCHES");    
    trackTypes.push_back("CURVES");    
    
    addTrackDropDown = new ofxUIDropDownList(320, "ADD TRACK", trackTypes, OFX_UI_FONT_SMALL); 
    addTrackDropDown->setAllowMultiple(false);
    addTrackDropDown->setAutoClose(true);

    gui->addWidgetEastOf(addTrackDropDown, zeroTimecode);
    
    //SET UP LISENTERS
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);	
    
    //setup timeline
	timeline.setup();
    timeline.setFrameRate(30);
	timeline.setDurationInSeconds(300);
	timeline.setOffset(ofVec2f(0, 75));
    
    timeline.addKeyframes("keys");
    
    ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);
}


//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& bang){
 	ofLogVerbose() << "Bang Fired from track " << bang.track->getName() << " at time " << bang.currentTime << " with flag " << bang.flag;
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
    else if(name == "ADD TRACK"){
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        if(ddlist->isOpen()){
            cout << "DISABLING" << endl;
            timeline.disable();
        }
        else{
            cout << "ENABLING" << endl;
            timeline.enable();
        }
        vector<ofxUIWidget *> &selected = ddlist->getSelected(); 
        for(int i = 0; i < selected.size(); i++) {
            if(selected[i]->getName() == "BANGS"){
                timeline.addBangs("New Bangs");
            }
            else if(selected[i]->getName() == "FLAGS"){
                timeline.addFlags("New Flags");                
            }
            else if(selected[i]->getName() == "CURVES"){
                timeline.addKeyframes("New Curves");                                
            }
            else if(selected[i]->getName() == "SWITCHES"){
                timeline.addSwitcher("New Switches");                
            }
        }
        ddlist->clearSelected();
    }
}

//--------------------------------------------------------------
void testApp::update(){
	timeLabel->setLabel(timeline.getCurrentTimeString());
}

//--------------------------------------------------------------
void testApp::draw(){
    timeline.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
        timeline.togglePlay();
    }
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