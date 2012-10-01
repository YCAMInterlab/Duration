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

#pragma once
#include "ofMain.h"

#include "ofxTimeline.h"
#include "ofxUI.h"
#include "ofxLocalization.h"

class ofxTLUIHeader {
  public:
    ofxTLUIHeader();
    virtual ~ofxTLUIHeader();
    
    void setTrackHeader(ofxTLTrackHeader* trackHeader);
	virtual void viewWasResized(ofEventArgs& args); 
    virtual void guiEvent(ofxUIEventArgs &e);
    
	virtual bool getShouldDelete();
	virtual void setShouldDelete(bool del);
	
    virtual bool sendOSC();
    virtual void setSendOSC(bool enable);
    virtual bool receiveOSC();
    virtual void setReceiveOSC(bool enable);

//	string getPalettePath();
	
	//just for drawing the red receiver background
	float lastInputReceivedTime;
	
	//remove duplicate sending and receiving
	bool hasReceivedValue;
	bool hasSentValue;
	
	//different value types for tracking last values sent over OSC
	float lastFloatSent;
	bool lastBoolSent;
	ofColor lastColorSent;
	
	//only receiving floats for now
	float lastValueReceived;
	
	ofxTLTrack* getTrack();
	ofxTLTrackHeader* getTrackHeader();
	string getTrackType();
	ofxLocalization* translation;
	ofxUICanvas* getGui();
	bool getModified();
	
	int setNumberOfbins(int bins);
	int getNumberOfBins();
	void setValueRange(ofRange range);
	void setValueMin(float min);
	void setValueMax(float max);
	
  protected:
    
    ofxUICanvas* gui;
    ofxTLTrackHeader* trackHeader;
	ofxUINumberDialer* minDialer;
	ofxUINumberDialer* maxDialer;
	ofxUITextInput* bins;
	ofxUILabelButton* palette;
	ofxUILabelButton* audioClip;
	ofxUILabelButton* resetRange;
	//Delay dialer?
	
    ofxUIToggle* sendOSCEnable;
	ofxUIToggle* receiveOSCEnable;
	bool resizeEventsEnabled;
	int audioNumberOfBins;
	
	string trackType;
    bool shouldDelete;
	bool modified;
};