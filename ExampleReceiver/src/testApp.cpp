#include "testApp.h"
#define CIRCLE_RADIUS 20
//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground(30, 30, 130);
	ofEnableAlphaBlending();
	
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
				  
	fbo.allocate(ofGetWidth(),ofGetHeight(),GL_RGB32F_ARB,4);
	fbo.begin();
	ofClear(30, 30, 130);
	fbo.end();
}

//--------------------------------------------------------------
void testApp::update(){


//	circles[2].setPosition(ofVec2f( (sin(ofGetElapsedTimef())*.5+.5) * ofGetWidth(), ofGetHeight()/2) );
	int numMessages = 0;
	// check for waiting messages
	
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
		numMessages++;
		
		for(int i = 0; i < circles.size(); i++){
			//this is a curve
			if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
				if(ofToLower(m.getAddress()) == "/"+circles[i].getName() + "/x"){
					circles[i].setX(ofMap(m.getArgAsFloat(0), 0, 1.0, CIRCLE_RADIUS, ofGetWidth()-CIRCLE_RADIUS));
					break;
				}
				
				else if(ofToLower(m.getAddress()) == "/"+circles[i].getName() + "/y"){
					circles[i].setY(ofMap(m.getArgAsFloat(0), 0, 1.0, CIRCLE_RADIUS, ofGetHeight()-CIRCLE_RADIUS));
					break;
				}
			}

			//this is a color
			if(m.getArgType(0) == OFXOSC_TYPE_INT32 &&
			   m.getNumArgs() == 3 &&
			   ofToLower(m.getAddress()) == "/"+circles[i].getName() + "/color")
			{
				circles[i].setColor(ofColor(m.getArgAsInt32(0),
											m.getArgAsInt32(1),
											m.getArgAsInt32(2)));
				break;
			}
				
		}
	}
	
//	cout << "num messages " << numMessages << endl;
}


//--------------------------------------------------------------
void testApp::draw(){
	fbo.begin();
	ofEnableSmoothing();
	ofPushStyle();
	ofSetColor(30, 30, 130, 1);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
	ofPopStyle();
	for(int i = 0; i < circles.size(); i++){
		ofPushStyle();
		ofFill();
		ofSetColor( circles[i].getColor());
		ofCircle( circles[i].getPosition(), CIRCLE_RADIUS*2);

		ofNoFill();
		ofSetLineWidth(3);
		ofSetColor(0);
		ofCircle( circles[i].getPosition(), CIRCLE_RADIUS*2);

		ofSetColor(255,255,255);
		font.drawString(circles[i].getName(), circles[i].getPosition().x-CIRCLE_RADIUS*.5, circles[i].getPosition().y+CIRCLE_RADIUS*.5);
		ofPopStyle();
	}
	fbo.end();
	fbo.draw(0,0,ofGetWidth(),ofGetHeight());
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
	fbo.allocate(w,h,GL_RGB32F_ARB);
	fbo.begin();
	ofClear(30, 30, 130);
	fbo.end();
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
