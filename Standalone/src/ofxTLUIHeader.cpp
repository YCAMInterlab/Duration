
#include "ofxTLUIHeader.h"

ofxTLUIHeader::ofxTLUIHeader(){
	gui = NULL;
    trackHeader = NULL;
    shouldDelete = false;

}

ofxTLUIHeader::~ofxTLUIHeader(){
    
    if(trackHeader != NULL){
        ofRemoveListener(trackHeader->events().viewWasResized, this, &ofxTLUIHeader::viewWasResized);
    }
    
    if(gui != NULL){
	    ofRemoveListener(gui->newGUIEvent, this, &ofxTLUIHeader::guiEvent);
        delete gui;
    }
    cout << "header destructor" << endl;
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
	if(trackType == "Tweens"){
        //SET THE RANGE
        ofxTLTweener* tweenTrack = (ofxTLTweener*)trackHeader->getTrack();
        minDialer = new ofxUINumberDialer(-9999., 9999., tweenTrack->getValueRange().min, 2, "min", OFX_UI_FONT_SMALL);
        minDialer->setPadding(0);
        gui->addWidgetRight( minDialer );
        
        maxDialer = new ofxUINumberDialer(-9999., 9999, tweenTrack->getValueRange().max, 2, "max", OFX_UI_FONT_SMALL);
        maxDialer->setPadding(0);
        gui->addWidgetRight( maxDialer );
    }
	    
	//Enable??
    oscEnabledToggle = new ofxUIToggle("send osc", true, 17, 17, 0, 0, OFX_UI_FONT_SMALL);
    oscEnabledToggle->setPadding(1);
    gui->addWidgetRight(oscEnabledToggle);
    
    //DELETE ME???
    vector<string> deleteTrack;
    deleteTrack.push_back("sure?");
    ofxUIDropDownList* dropDown = new ofxUIDropDownList("delete", deleteTrack, OFX_UI_FONT_SMALL);
    dropDown->setAllowMultiple(false);
    dropDown->setAutoClose(true);
    dropDown->setPadding(0); //Tweak to make the drop down small enough
    gui->addWidgetRight(dropDown);
    
    gui->autoSizeToFitWidgets();
    
    ofAddListener(trackHeader->events().viewWasResized, this, &ofxTLUIHeader::viewWasResized);
    ofAddListener(gui->newGUIEvent, this, &ofxTLUIHeader::guiEvent);
}

void ofxTLUIHeader::viewWasResized(ofEventArgs& args){
    gui->getRect()->y = trackHeader->getDrawRect().y; //TWEAK to get on the header
	gui->getRect()->x = trackHeader->getTimeline()->getTopRight().x - (gui->getRect()->width + 50);
}

bool ofxTLUIHeader::isOSCEnabled(){
	return oscEnabledToggle->getValue();
}

void ofxTLUIHeader::setOSCEnabled(bool enableosc){
    oscEnabledToggle->setValue(enableosc);
}

bool ofxTLUIHeader::getShouldDelete(){
    return shouldDelete;
}

void ofxTLUIHeader::guiEvent(ofxUIEventArgs &e){
    cout << e.widget->getName() << " hit!" << endl;
    
    //TODO min/max
    if(e.widget->getName() == "min"){
        float newMinValue = MIN(minDialer->getValue(), maxDialer->getValue());
        minDialer->setValue(newMinValue);
        ofRange newValueRange = ofRange(newMinValue, maxDialer->getValue());
        ofxTLTweener* track = (ofxTLTweener*)trackHeader->getTrack();
        track->setValueRange(newValueRange);
    }
	else if(e.widget->getName() == "max"){
        float newMaxValue = MAX(minDialer->getValue(), maxDialer->getValue());
        maxDialer->setValue(newMaxValue);
        ofRange newValueRange = ofRange(minDialer->getValue(), newMaxValue);
        ofxTLTweener* track = (ofxTLTweener*)trackHeader->getTrack();
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
//	else if(e.widget->getName() == "send osc"){
//
//    }
}

