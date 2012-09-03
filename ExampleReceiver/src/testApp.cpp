#include "testApp.h"
#define CIRCLE_RADIUS 20
//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofBackground(30, 30, 130);
	ofSetVerticalSync(true);
	
	ofToggleFullscreen();
	
	ofSetCircleResolution(200);
	
	ofEnableSmoothing();
	
	receiver.setup(PORT);

	font.loadFont("NewMedia Fett.ttf", 30);
	
	ControlCircle a,b,c,d,e,f,g;
	a.setName("a");
	b.setName("b");
	c.setName("c");
	d.setName("d");
	e.setName("e");
	f.setName("f");
	g.setName("g");
	
	circles.push_back(a);
	circles.push_back(b);
	circles.push_back(c);
	circles.push_back(d);
	circles.push_back(e);
	circles.push_back(f);
	circles.push_back(g);
	
	for(int i = 0; i < circles.size(); i++){
		circles[i].setPosition(ofVec2f(ofRandom(CIRCLE_RADIUS, ofGetWidth()-CIRCLE_RADIUS),
									   ofRandom(CIRCLE_RADIUS, ofGetHeight()-CIRCLE_RADIUS)));

	}
				  
}

//--------------------------------------------------------------
void testApp::update(){

	int numMessages = 0;
	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
//		cout << "Received " << m.getAddress() << endl;
		for(int i = 0; i < circles.size(); i++){
			numMessages++;
			if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
				if(ofToLower(m.getAddress()) == "/"+circles[i].getName() + "/x"){
					circles[i].setX(m.getArgAsFloat(0));
					break;
				}
				else if(ofToLower(m.getAddress()) == "/"+circles[i].getName() + "/y"){
					circles[i].setY(m.getArgAsFloat(0));
					break;
				}
			}
		}
	}
	cout << "num messages " << numMessages << endl;
}


//--------------------------------------------------------------
void testApp::draw(){
	
	for(int i = 0; i < circles.size(); i++){
		ofPushStyle();
		ofFill();
		ofSetColor(200, 200, 50);
		ofCircle( circles[i].getPosition(), CIRCLE_RADIUS*2);
		ofNoFill();
		ofSetLineWidth(3);
		ofSetColor(0);
		ofCircle( circles[i].getPosition(), CIRCLE_RADIUS*2);

		ofSetColor(255,255,255);
		font.drawString(circles[i].getName(), circles[i].getPosition().x, circles[i].getPosition().y);
		ofPopStyle();
	}
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
