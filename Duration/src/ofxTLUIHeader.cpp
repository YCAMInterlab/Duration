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

#include "ofxTLUIHeader.h"
#ifdef TARGET_OSX
#include "ofxTLAudioTrack.h"
#endif

#include <locale>
bool isNumber(const string& s){
	locale loc;
	std::string::const_iterator it = s.begin();
    while (it != s.end() && (std::isdigit(*it, loc))) ++it;
	return !s.empty() && it == s.end();
}

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
	audioNumberOfBins = 256;
	
	bins = NULL;
	minDialer = NULL;
	maxDialer = NULL;
    sendOSCEnable = NULL;
	receiveOSCEnable = NULL;
	modified = false;
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

    trackType = trackHeader->getTrack()->getTrackType();
	if(trackType != "Audio" && trackType != "Video"){
		ofxUILabelButton* playSolo = new ofxUILabelButton(">", false,0,0,0, OFX_UI_FONT_SMALL);
		playSolo->setPadding(0);
		gui->addWidgetRight(playSolo);
	}
	
	if(trackType == "Curves" || trackType == "LFO"){
        //SET THE RANGE
        ofxTLKeyframes* tweenTrack = (ofxTLKeyframes*)trackHeader->getTrack();
        minDialer = new ofxUINumberDialer(-9999., 9999., tweenTrack->getValueRange().min, 2, "min", OFX_UI_FONT_SMALL);
        minDialer->setPadding(0);
        gui->addWidgetRight( minDialer );
        
        maxDialer = new ofxUINumberDialer(-9999., 9999, tweenTrack->getValueRange().max, 2, "max", OFX_UI_FONT_SMALL);
        maxDialer->setPadding(0);
        gui->addWidgetRight( maxDialer );
		
		resetRange = new ofxUILabelButton(translation->translateKey("reset"), false, 0,0,0,0,OFX_UI_FONT_SMALL);
		resetRange->setPadding(0);
		gui->addWidgetRight(resetRange);
    }
	else if(trackType == "Colors"){
		palette = new ofxUILabelButton(translation->translateKey("change palette"), false,0,0,0,0, OFX_UI_FONT_SMALL);
		palette->setPadding(0);
		gui->addWidgetRight(palette);
	}
#ifdef TARGET_OSX
	else if(trackType == "Audio"){
		audioClip = new ofxUILabelButton(translation->translateKey("select audio"), false,0,0,0,0, OFX_UI_FONT_SMALL);
		audioClip->setPadding(0);
		gui->addWidgetRight(audioClip);
		
		ofxUILabel* binLabel = new ofxUILabel(0, 0, "bins", translation->translateKey("bins"), OFX_UI_FONT_SMALL);
		binLabel->setPadding(0);
		gui->addWidgetRight(binLabel);
		
		bins = new ofxUITextInput("bins", "256", 50, 0,0,0, OFX_UI_FONT_SMALL);
		bins->setAutoClear(false);
		bins->setPadding(0);
		gui->addWidgetRight(bins);
	}
#endif
	
	if(trackType == "Bangs" || trackType == "Curves" || trackType == "Notes"){
		receiveOSCEnable = new ofxUIToggle(translation->translateKey("receive osc"), true, 17, 17, 0, 0, OFX_UI_FONT_SMALL);
		receiveOSCEnable->setPadding(1);
		gui->addWidgetRight(receiveOSCEnable);
	}
	
//	if(trackType != "Audio"){ //TODO: audio should send some nice FFT OSC
		sendOSCEnable = new ofxUIToggle(translation->translateKey("send osc"), true, 17, 17, 0, 0, OFX_UI_FONT_SMALL);
		sendOSCEnable->setPadding(1);
		gui->addWidgetRight(sendOSCEnable);
//	}
    
    // add range high / low
    if(trackType == "Notes"){
        int testInt = 0;    // ambiguous call avoided!
         minDialer = new ofxUINumberDialer(0, 127, testInt, 0, "min", OFX_UI_FONT_SMALL);
        minDialer->setPadding(0);
        gui->addWidgetRight( minDialer );
        
        maxDialer = new ofxUINumberDialer(0, 127, 11, 0, "max", OFX_UI_FONT_SMALL);
        maxDialer->setPadding(0);
        gui->addWidgetRight( maxDialer );
    }
    
    //DELETE ME???
    vector<string> deleteTrack;
    deleteTrack.push_back(translation->translateKey("sure?"));
    ofxUIDropDownList* dropDown = new ofxUIDropDownList(translation->translateKey("delete"), deleteTrack, 0,0,0, OFX_UI_FONT_SMALL);
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

int ofxTLUIHeader::getNumberOfBins(){
#ifdef TARGET_OSX
	if(getTrackType() == "Audio"){
		return ((ofxTLAudioTrack*)getTrack())->getDefaultBinCount();
	}
#endif
	return 0;
}

void ofxTLUIHeader::setNumberOfbins(int binCount){
#ifdef TARGET_OSX
	if(getTrackType() == "Audio"){
		((ofxTLAudioTrack*)getTrack())->getFFTSpectrum(binCount);
		bins->setTextString(ofToString(binCount));
	}
#endif
}

void ofxTLUIHeader::setValueRange(ofRange range){
	if(getTrackType() == "Curves" || getTrackType() == "LFO"){
		minDialer->setValue(range.min);
		maxDialer->setValue(range.max);
		((ofxTLKeyframes*)getTrack())->setValueRange(range);
	}
	else{
		ofLogError("ofxTLUIHeader::setValueMax") << "Cannot set value range on tracks that aren't curves";
	}	
}

void ofxTLUIHeader::setValueMin(float min){
	if(getTrackType() == "Curves" || getTrackType() == "LFO"){
		minDialer->setValue(min);
		((ofxTLKeyframes*)getTrack())->setValueRangeMin(min);
	}
	else{
		ofLogError("ofxTLUIHeader::setValueMax") << "Cannot set min value on tracks that aren't curves";
	}
}

void ofxTLUIHeader::setValueMax(float max){
	if(getTrackType() == "Curves" || getTrackType() == "LFO"){
		maxDialer->setValue(max);
		((ofxTLKeyframes*)getTrack())->setValueRangeMin(max);
	}
	else{
		ofLogError("ofxTLUIHeader::setValueMax") << "Cannot set max value on tracks that aren't curves";
	}
}

ofxUICanvas* ofxTLUIHeader::getGui(){
	return gui;
}

bool ofxTLUIHeader::sendOSC(){
	return sendOSCEnable != NULL && sendOSCEnable->getValue();
}

void ofxTLUIHeader::setSendOSC(bool enable){
	if(sendOSCEnable != NULL){
		sendOSCEnable->setValue(enable);
	}
}

bool ofxTLUIHeader::receiveOSC(){
	return receiveOSCEnable != NULL && receiveOSCEnable->getValue();
}

void ofxTLUIHeader::setReceiveOSC(bool enable){
	if(receiveOSCEnable != NULL){
		receiveOSCEnable->setValue(enable);
	}
}

void ofxTLUIHeader::setShouldDelete(bool del){
	shouldDelete = del;
	if(shouldDelete){
		ofRemoveListener(trackHeader->events().viewWasResized, this, &ofxTLUIHeader::viewWasResized);
		trackHeader = NULL; //this is needed to circumvent the problem in the destructor
	}
}

bool ofxTLUIHeader::getShouldDelete(){
    return shouldDelete;
}

bool ofxTLUIHeader::getModified(){
	bool b = modified;
	modified = false;
	return b;
}

ofxTLTrack* ofxTLUIHeader::getTrack(){
	return trackHeader->getTrack();
}

ofxTLTrackHeader* ofxTLUIHeader::getTrackHeader(){
	return trackHeader;
}

string ofxTLUIHeader::getTrackType(){
	return trackType;
}

void ofxTLUIHeader::guiEvent(ofxUIEventArgs &e){
//    cout << e.widget->getName() << " hit!" << endl;
    
	if(e.widget->getName() == ">" && ((ofxUILabelButton*)e.widget)->getValue()){
		getTrack()->togglePlay();
	}
    else if(e.widget->getName() == "min"){
        float newMinValue = MIN(minDialer->getValue(), maxDialer->getValue());
        minDialer->setValue(newMinValue);
        ofRange newValueRange = ofRange(newMinValue, maxDialer->getValue());
        ofxTLCurves* track = (ofxTLCurves*)trackHeader->getTrack();
        track->setValueRange(newValueRange);
		modified = true;
    }
	else if(e.widget->getName() == "max"){
        float newMaxValue = MAX(minDialer->getValue(), maxDialer->getValue());
        maxDialer->setValue(newMaxValue);
        ofRange newValueRange = ofRange(minDialer->getValue(), newMaxValue);
        ofxTLKeyframes* track = (ofxTLKeyframes*)trackHeader->getTrack();
        track->setValueRange(newValueRange);
		modified = true;
    }
    else if(e.widget->getName() == translation->translateKey("delete")){
        ofxUIDropDownList* deleteDropDown = (ofxUIDropDownList*)e.widget;
        if(deleteDropDown->isOpen()){
            trackHeader->getTrack()->disable();
        }
        else{
            trackHeader->getTrack()->enable();
            if(deleteDropDown->getSelected().size() > 0 &&
               deleteDropDown->getSelected()[0]->getName() == translation->translateKey("sure?")){
				//do this because the header gets deleted before our destructor is called
				setShouldDelete(true);
                //shouldDelete = true;
            }
            deleteDropDown->clearSelected();
            deleteDropDown->close();
        }
    }
	else if(e.widget == palette && palette->getValue()){
		ofFileDialogResult r = ofSystemLoadDialog();
		if(r.bSuccess){
			ofxTLColorTrack* colorTrack = (ofxTLColorTrack*)trackHeader->getTrack();
			colorTrack->loadColorPalette(r.getPath());
			modified = true;
		}		
	}
	else if(e.widget == resetRange && resetRange->getValue()){
		minDialer->setValue(0);
        maxDialer->setValue(1.0);
        ofRange newValueRange = ofRange(0, 1.0);
        ofxTLKeyframes* track = (ofxTLKeyframes*)trackHeader->getTrack();
        track->setValueRange(newValueRange);
		modified = true;		
	}
#ifdef TARGET_OSX
	else if(e.widget == audioClip && audioClip->getValue()){
		ofFileDialogResult r = ofSystemLoadDialog();
		if(r.bSuccess){
			ofxTLAudioTrack* audioTrack = (ofxTLAudioTrack*)trackHeader->getTrack();
			audioTrack->loadSoundfile(r.getPath());
			modified = true;
		}
	}
	else if(e.widget == bins){
		if(!isNumber(bins->getTextString())){
			bins->setTextString(ofToString(audioNumberOfBins));
		}
	
		int newBinNumber = ofToInt(bins->getTextString());
		if(newBinNumber != audioNumberOfBins){
			audioNumberOfBins = newBinNumber;
			ofxTLAudioTrack* audioTrack = (ofxTLAudioTrack*)trackHeader->getTrack();
			audioTrack->getFFTSpectrum(audioNumberOfBins);
//			cout << "new bin number is " << audioTrack->getDefaultBinCount() << " after setting to " << audioNumberOfBins << endl;
		}
	}
#endif
    //this is polled from outside
	else if(e.widget == sendOSCEnable){
		modified = true;
    }
	else if(e.widget == receiveOSCEnable){
		modified = true;
    }
}

