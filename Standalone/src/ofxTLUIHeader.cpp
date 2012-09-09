
#include "ofxTLUIHeader.h"
//#include "ofxTLAudioTrack.h"

ofxTLUIHeader::ofxTLUIHeader(){
	gui = NULL;
    trackHeader = NULL;
    shouldDelete = false;
	lastInputReceivedTime = -1000;

	hasReceivedValue = false;
	hasSentValue = false;
	lastFloatSent = 0;
	lastBoolSent = false;
	lastColorSent = ofColor(0,0,0);
	lastValueReceived = 0;
	
	minDialer = NULL;
	maxDialer = NULL;
    sendOSCEnable = NULL;
	receiveOSCEnable = NULL;


}

ofxTLUIHeader::~ofxTLUIHeader(){
    
    if(trackHeader != NULL){
        ofRemoveListener(trackHeader->events().viewWasResized, this, &ofxTLUIHeader::viewWasResized);
    }
    
    if(gui != NULL){
	    ofRemoveListener(gui->newGUIEvent, this, &ofxTLUIHeader::guiEvent);
        delete gui;
    }
}

void ofxTLUIHeader::setTrackHeader(ofxTLTrackHeader* header){
    trackHeader = header;
    
    //create gui
    ofRectangle headerRect = trackHeader->getDrawRect();
    headerRect.width = 600;
    headerRect.x = trackHeader->getTimeline()->getTopRight().x - (headerRect.width + 50);
    
	gui = new ofxUICanvas(headerRect);     
	//gui->setDrawPadding(true);
    //gui->setDrawWidgetPadding(true);
    
    gui->setWidgetSpacing(1);
	gui->setPadding(0);
    //switch on track type

    string trackType = trackHeader->getTrack()->getTrackType();
	if(trackType == "Curves"){
        //SET THE RANGE
        ofxTLCurves* tweenTrack = (ofxTLCurves*)trackHeader->getTrack();
        minDialer = new ofxUINumberDialer(-9999., 9999., tweenTrack->getValueRange().min, 2, "min", OFX_UI_FONT_SMALL);
        minDialer->setPadding(0);
        gui->addWidgetRight( minDialer );
        
        maxDialer = new ofxUINumberDialer(-9999., 9999, tweenTrack->getValueRange().max, 2, "max", OFX_UI_FONT_SMALL);
        maxDialer->setPadding(0);
        gui->addWidgetRight( maxDialer );
		
    }
	else if(trackType == "Colors"){
		ofxTLColorTrack* colorTrack = (ofxTLColorTrack*)trackHeader->getTrack();
		//TODO: add load/save palette
	}
	/*
	else if(trackType == "Sound"){
		ofxTLAudioTrack* soundTrack = (ofxTLAudioTrack*)trackHeader->getTrack();
		//TODO: add load/save sound
	}
	*/
	
	if(trackType == "Bangs" || trackType == "Curves"){
		receiveOSCEnable = new ofxUIToggle("receive osc", true, 17, 17, 0, 0, OFX_UI_FONT_SMALL);
		receiveOSCEnable->setPadding(1);
		gui->addWidgetRight(receiveOSCEnable);
	}
	
	//Enable??
    sendOSCEnable = new ofxUIToggle("send osc", true, 17, 17, 0, 0, OFX_UI_FONT_SMALL);
    sendOSCEnable->setPadding(1);
    gui->addWidgetRight(sendOSCEnable);
    
    //DELETE ME???
    vector<string> deleteTrack;
    deleteTrack.push_back("sure?");
    ofxUIDropDownList* dropDown = new ofxUIDropDownList("delete", deleteTrack, 0,0,0, OFX_UI_FONT_SMALL);
    dropDown->setAllowMultiple(false);
    dropDown->setAutoClose(true);
    dropDown->setPadding(0); //Tweak to make the drop down small enough
    gui->addWidgetRight(dropDown);
    
    gui->autoSizeToFitWidgets();
    
    gui->getRect()->y = trackHeader->getDrawRect().y; //TWEAK to get on the header
	gui->getRect()->x = trackHeader->getTimeline()->getTopRight().x - (gui->getRect()->width + 50);

    ofAddListener(trackHeader->events().viewWasResized, this, &ofxTLUIHeader::viewWasResized);
    ofAddListener(gui->newGUIEvent, this, &ofxTLUIHeader::guiEvent);
}

void ofxTLUIHeader::viewWasResized(ofEventArgs& args){
    gui->getRect()->y = trackHeader->getDrawRect().y; //TWEAK to get on the header
	gui->getRect()->x = trackHeader->getTimeline()->getTopRight().x - (gui->getRect()->width + 50);
}

//bool ofxTLUIHeader::isOSCEnabled(){
//	return oscEnabledToggle->getValue();
//}
//
//void ofxTLUIHeader::setOSCEnabled(bool enableosc){
//    oscEnabledToggle->setValue(enableosc);
//}

bool ofxTLUIHeader::sendOSC(){
	return sendOSCEnable->getValue();
}

void ofxTLUIHeader::setSendOSC(bool enable){
	sendOSCEnable->setValue(enable);
	
}
bool ofxTLUIHeader::receiveOSC(){
	return receiveOSCEnable != NULL && receiveOSCEnable->getValue();
}

void ofxTLUIHeader::setReceiveOSC(bool enable){
	if(receiveOSCEnable != NULL){
		receiveOSCEnable->setValue(enable);
	}
}

bool ofxTLUIHeader::getShouldDelete(){
    return shouldDelete;
}

ofxTLTrack* ofxTLUIHeader::getTrack(){
	return trackHeader->getTrack();
}

ofxTLTrackHeader* ofxTLUIHeader::getTrackHeader(){
	return trackHeader;
}

void ofxTLUIHeader::guiEvent(ofxUIEventArgs &e){
//    cout << e.widget->getName() << " hit!" << endl;
    
    //TODO min/max
    if(e.widget->getName() == "min"){
        float newMinValue = MIN(minDialer->getValue(), maxDialer->getValue());
        minDialer->setValue(newMinValue);
        ofRange newValueRange = ofRange(newMinValue, maxDialer->getValue());
        ofxTLCurves* track = (ofxTLCurves*)trackHeader->getTrack();
        track->setValueRange(newValueRange);
    }
	else if(e.widget->getName() == "max"){
        float newMaxValue = MAX(minDialer->getValue(), maxDialer->getValue());
        maxDialer->setValue(newMaxValue);
        ofRange newValueRange = ofRange(minDialer->getValue(), newMaxValue);
        ofxTLCurves* track = (ofxTLCurves*)trackHeader->getTrack();
        track->setValueRange(newValueRange);
    }
    else if(e.widget->getName() == "delete"){
        ofxUIDropDownList* deleteDropDown = (ofxUIDropDownList*)e.widget;
        if(deleteDropDown->isOpen()){
            trackHeader->getTrack()->disable();
        }
        else{
            trackHeader->getTrack()->enable();
            if(deleteDropDown->getSelected().size() > 0 &&
               deleteDropDown->getSelected()[0]->getName() == "sure?"){
				//do this because the header gets deleted before our destructor is called
                ofRemoveListener(trackHeader->events().viewWasResized, this, &ofxTLUIHeader::viewWasResized);
                trackHeader = NULL;
                shouldDelete = true;
            }
            deleteDropDown->clearSelected();
            deleteDropDown->close();
        }
    }
    //this is polled from outside
//	else if(e.widget->getName() == "send osc"){
//
//    }
}

