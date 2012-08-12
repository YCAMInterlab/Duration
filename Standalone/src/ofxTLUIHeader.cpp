
#include "ofxTLUIHeader.h"

ofxTLUIHeader::ofxTLUIHeader(){
	gui = NULL;
    trackHeader = NULL;
}

ofxTLUIHeader::~ofxTLUIHeader(){
    if(trackHeader != NULL){
        ofRemoveListener(trackHeader->events().viewWasResized, this, &ofxTLUIHeader::viewWasResized);
    }
    if(gui != NULL){
	    ofAddListener(gui->newGUIEvent, this, &ofxTLUIHeader::guiEvent);
    }
}

void ofxTLUIHeader::setTrackHeader(ofxTLTrackHeader* header){
    trackHeader = header;
    
    //create gui
    ofRectangle headerRect = trackHeader->getDrawRect();
    headerRect.width = 300;
    headerRect.x = trackHeader->getTimeline()->getTopRight().x - (headerRect.width + 50);
    headerRect.y -= 20;
	gui = new ofxUICanvas(headerRect);     

    //switch on track type
    string trackType = trackHeader->getTrack()->getTrackType();
    
    gui->addWidgetDown(new ofxUIToggle( "enable", true, 8, 8, 0, 0, OFX_UI_FONT_SMALL));
    
//	if(trackType == "Bangs");
    
    ofAddListener(trackHeader->events().viewWasResized, this, &ofxTLUIHeader::viewWasResized);
}

void ofxTLUIHeader::viewWasResized(ofEventArgs& args){
    //TODO: move gui ...
    gui->getRect()->y = trackHeader->getDrawRect().y - 5;
	gui->getRect()->x = trackHeader->getTimeline()->getTopRight().x - gui->getRect()->width + 50;
}

void ofxTLUIHeader::guiEvent(ofxUIEventArgs &e){
    cout << e.widget->getName() << " hit!" << endl;
}

