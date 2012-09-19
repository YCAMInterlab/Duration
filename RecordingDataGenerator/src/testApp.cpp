#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(255*.15);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofEnableSmoothing();

	sendBang = false;
	
	// open an outgoing connection to HOST:PORT
	sender.setup(HOST, PORT);
	
	font.loadFont("NewMedia Fett.ttf", 15);
	
	currentFreeDrawCursor = ofVec2f(.5,.5);
	currentNoiseCursor = ofVec2f(.5,.5);
	currentSineCursor = ofVec2f(.5,.5);
	
	currentSineValue = 0;
	currentNoiseValue = 0;
	currentSinePosition = 0;
	currentNoisePosition = 0;
	
	redColor = ofColor(250, 200, 80);
	blueColor = redColor.getInverted();
	
	bangTime = -10;
	lastTime = 0;
}

//--------------------------------------------------------------
void testApp::update(){
	
	//update rectangles
	sineWave = ofRectangle(1,0, ofGetWidth()/2, ofGetHeight()/2);
	freeDraw = ofRectangle(ofGetWidth()/2, 0, ofGetWidth()/2, ofGetHeight()/2);
	clickBang = ofRectangle(1, ofGetHeight()/2, ofGetWidth()/2, ofGetHeight()/2-1);
	noise = ofRectangle(ofGetWidth()/2, ofGetHeight()/2, ofGetWidth()/2, ofGetHeight()/2-1);

	//update noise and sine
	float dT = ofGetElapsedTimef()-lastTime;
	lastTime = ofGetElapsedTimef();
 	currentSinePosition  += dT * currentSineCursor.x*5;
	currentNoisePosition += dT * currentNoiseCursor.x;

	//update noise
	currentNoiseValue = ofNoise(currentNoisePosition, currentNoiseCursor.y);
	currentSineValue = sin(currentSinePosition)*.5+.5;
	
	freedrawbuf.push_back(currentFreeDrawCursor);
	noisebuf.push_back(currentNoiseValue);
	sinbuf.push_back(currentSineValue);
	while(sinbuf.size() > sineWave.width && sinbuf.size() > 0){
		sinbuf.erase(sinbuf.begin());
		noisebuf.erase(noisebuf.begin());
		freedrawbuf.erase(freedrawbuf.begin());
	}

	//send values
	ofxOscBundle bundle;
	
	ofxOscMessage m;
	m.setAddress("/sine");
	m.addFloatArg(currentSineValue);
	bundle.addMessage(m);
	m.clear();

	m.setAddress("/noise");
	m.addFloatArg(currentNoiseValue);
	bundle.addMessage(m);
	m.clear();
	
	m.setAddress("/freedraw/x");
	m.addFloatArg(currentFreeDrawCursor.x);
	bundle.addMessage(m);
	m.clear();
	
	m.setAddress("/freedraw/y");
	m.addFloatArg(currentFreeDrawCursor.y);
	bundle.addMessage(m);
	m.clear();
	
	if(sendBang){
		m.setAddress("/bang");
		bundle.addMessage(m);
		m.clear();
		sendBang = false;
	}
	
	sender.sendBundle(bundle);
}

//--------------------------------------------------------------
void testApp::draw(){
	ofPushStyle();
	
	//draw backgrounds
	ofVec2f mousePoint(mouseX,mouseY);
	ofSetColor(blueColor, 75+50 * sin(ofGetElapsedTimef()*10)*.5+.5);
	ofFill();
	if(freeDraw.inside(mousePoint)){
		ofRect(freeDraw);
	}
	if(sineWave.inside(mousePoint)){
		ofRect(sineWave);
	}
	if(noise.inside(mousePoint)){
		ofRect(noise);
	}
	if(clickBang.inside(mousePoint)){
		ofRect(clickBang);
	}
	
	ofPolyline p;
	
	//draw free draw
	float fadeValue = powf(sin(ofGetElapsedTimef()*7)*.5+.5,2);
	ofFill();
	ofVec2f freeDrawPoint = freeDraw.getMin() + currentFreeDrawCursor*ofVec2f(freeDraw.width,freeDraw.height);
	ofRectangle cursorRect;
	cursorRect.setFromCenter(freeDrawPoint, 10, 10);
	ofRectangle cursorHighlight = cursorRect;
	cursorHighlight.scaleFromCenter(1.2 + fadeValue*.5);
	
	ofVec2f freeDrawDims = ofVec2f(freeDraw.width,freeDraw.height);
	for(int i = 1; i < freedrawbuf.size(); i++){
		if(freedrawbuf[i] != freedrawbuf[i-1]){
			float fade = powf(1.0*i/freedrawbuf.size(),2);
			ofSetColor(redColor,fade*255);
			ofLine(freeDraw.getMin()+freedrawbuf[i  ]*freeDrawDims,
				   freeDraw.getMin()+freedrawbuf[i-1]*freeDrawDims);
		}
	}
	
	ofNoFill();
	ofSetColor(redColor,(1-fadeValue)*255);
	ofRect(cursorHighlight);
	ofFill();
	ofSetColor(redColor);
	ofRect(cursorRect);
	//end free draw
	
	//draw noise
	ofVec2f noisePoint = ofVec2f(0,currentNoiseValue*noise.getHeight());
	ofLine(ofVec2f(noise.getMinX(),noise.getMaxY()) - noisePoint,
		   ofVec2f(noise.getMaxX(),noise.getMaxY()) - noisePoint);
	
	p.clear();
	for(int i = 0; i < noisebuf.size(); i++){
		p.addVertex(ofVec2f(noise.x + (noise.width - noisebuf.size()+i),
				 			noise.getMaxY()-noisebuf[i]*noise.height));
	}
	p.draw();
	//end noise draw
	
	
	//draw sine
	ofVec2f sinePoint = ofVec2f(0,currentSineValue*sineWave.getHeight());
	ofLine(ofVec2f(sineWave.getMinX(),sineWave.getMaxY()) - sinePoint,
		   ofVec2f(sineWave.getMaxX(),sineWave.getMaxY()) - sinePoint);
	
	p.clear();
	for(int i = 0; i < sinbuf.size(); i++){
		p.addVertex(ofVec2f(sineWave.x + (sineWave.width - sinbuf.size() + i),
				 			sineWave.getMaxY()-sinbuf[i]*sineWave.height));
	}
	
	p.draw();
	//end sine
	
	//draw bang
	float bangFade = ofMap(ofGetElapsedTimef(), bangTime, bangTime+.33, 1.0, 0, true);
	if(bangFade > 0){
		ofFill();
		ofSetColor(redColor,bangFade*255);
		ofRect(clickBang);
		ofRectangle fadeOut = clickBang;
		ofNoFill();
		ofSetColor(redColor);
		fadeOut.scaleFromCenter(powf(1-bangFade*.4,2));
		ofRect(fadeOut);
	}
	
	//draw borders
	ofNoFill();
	ofSetColor(redColor);
	
	ofRect(sineWave);
	ofRect(freeDraw);
	ofRect(clickBang);
	ofRect(noise);
	
	ofSetColor(255);
	font.drawString("/sine", sineWave.x+10, sineWave.y+20);
	font.drawString("/freedraw/x + /freedraw/y", freeDraw.x+10, freeDraw.y+20);
	font.drawString("/bang", clickBang.x+10, clickBang.y+20);
	font.drawString("/noise", noise.x+10, noise.y+20);

	font.drawString("sending to localhost:12346", clickBang.x+10, clickBang.getMaxY() - 5);
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

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	ofVec2f mousePoint(x,y);
	updateCursorPoint(mousePoint, freeDraw, currentFreeDrawCursor);
	updateCursorPoint(mousePoint, sineWave, currentSineCursor);
	updateCursorPoint(mousePoint, noise, currentNoiseCursor);
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	ofVec2f mousePoint(x,y);
	updateCursorPoint(mousePoint, freeDraw, currentFreeDrawCursor);
	updateCursorPoint(mousePoint, sineWave, currentSineCursor);
	updateCursorPoint(mousePoint, noise, currentNoiseCursor);

	if(clickBang.inside(mousePoint)){
		bangTime = ofGetElapsedTimef();
		sendBang = true;
	}
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

//--------------------------------------------------------------
void testApp::updateCursorPoint(ofVec2f mousePoint, ofRectangle rect, ofVec2f& cursor){
	if(rect.inside(mousePoint)){
		cursor = ofVec2f(mousePoint - rect.getMin()) / (rect.getMax() - rect.getMin());
	}
}
