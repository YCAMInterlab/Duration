#include "testApp.h"
#ifdef TARGET_OSX
#include "ofxRemoveCocoaMenu.h"
#endif

//--------------------------------------------------------------
void testApp::setup(){
	
    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    ofBackground(.15*255);
    ofEnableAlphaBlending();
    ofEnableSmoothing();

	controller.setup();
#ifdef TARGET_OSX	
	removeCocoaMenuFromGlut();
#endif
}

//#include "ofxHotkeys.h"

//--------------------------------------------------------------
void testApp::update(){
	//cout << "CTRL PRESSED" << ofGetModifierControlPressed() << endl;
}

//--------------------------------------------------------------
void testApp::draw(){
	
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
