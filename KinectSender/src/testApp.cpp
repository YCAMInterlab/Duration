#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	
	ofSetFrameRate(60);
	ofEnableAlphaBlending();
	
	kinect = new ofxKinect();
	// enable depth->video image calibration
	kinect->setRegistration(false);
    
	kinect->init();
	//kinect->init(true); // shows infrared instead of RGB video image
	kinect->init(false, false); // disable video image (faster fps)
	kinect->open(); // opens first available kinect
	
	colorImg.allocate(kinect->width, kinect->height);
	grayImage.allocate(kinect->width, kinect->height);
	background.allocate(kinect->width, kinect->height);
	subtracted.allocate(kinect->width, kinect->height);
	
//	grayThreshFar.allocate(kinect->width, kinect->height);
	shouldCaptureBackground = true;
	nearThreshold = 230;
	farThreshold = 70;
	
	// zero the tilt on startup
	angle = 0;
	//kinect->setCameraTiltAngle(angle);
	sender.setup("localhost", 12345);

	ui = new ofxUICanvas();
	ui->addWidgetRight( new ofxUISlider("far threshold", 0, 200, &farThreshold, 10, 250) );
}

//--------------------------------------------------------------
void testApp::update() {
	
	ofBackground(100, 100, 100);
	
	kinect->update();
	
	// there is a new frame and we are connected
	if(kinect->isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect->getDepthPixels(), kinect->width, kinect->height);
		grayImage.threshold(farThreshold);
		
		if(shouldCaptureBackground){
			background = grayImage;
			shouldCaptureBackground = false;
		}
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		cvSub(grayImage.getCvImage(), background.getCvImage(), subtracted.getCvImage());

		// update the cv images
//		grayImage.flagImageChanged();
		subtracted.flagImageChanged();
		
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(subtracted, 100, (kinect->width*kinect->height)/2, 20, false);
		
		//use the biggest blob to send XY
		if(contourFinder.blobs.size() > 0){
			float maxBlobSize = contourFinder.blobs[0].area;
			int maxBlob = 0;
			for(int i = 1; i < contourFinder.blobs.size(); i++){
				if(contourFinder.blobs[i].area > maxBlobSize){
					maxBlobSize = contourFinder.blobs[i].area;
					maxBlob = i;
				}
			}
			
			ofVec2f pointtosend = contourFinder.blobs[maxBlob].centroid / ofVec2f(kinect->width,kinect->height);
			ofxOscMessage mx;
			mx.setAddress("/kinect/x");
			mx.addFloatArg(pointtosend.x);
			
			ofxOscMessage my;
			my.setAddress("/kinect/y");
			my.addFloatArg(pointtosend.y);
			
			ofxOscBundle b;
			b.addMessage(mx);
			b.addMessage(my);
						
			sender.sendBundle(b);
			sentPoints.push_back(pointtosend);
			
		}
	}
}

//--------------------------------------------------------------
void testApp::draw() {
	
	// draw from the live kinect	
	ofRectangle screenRect = ofRectangle( 0, 0, ofGetWidth(), ofGetHeight() );
	ofRectangle kinectRect = ofRectangle( 0, 0, 640, 480 );
	kinectRect = screenRect.scaleIntoMe(kinectRect);

	ofPushStyle();

	ofSetColor(255,255,255, 100);
	kinect->drawDepth(kinectRect);

	ofSetColor(255,255,255, 100);
	subtracted.draw(kinectRect);
	
	ofSetColor(255);
	contourFinder.draw(kinectRect);
	
	ofSetColor(255, 200, 180);
	ofNoFill();
	
	ofRect(kinectRect);
	
	ofSetColor(255,0,0);
	for(int i = 1; i < sentPoints.size(); i++){
		ofLine(kinectRect.getMin() + sentPoints[i-1]*ofVec2f(kinectRect.width,kinectRect.height),
			   kinectRect.getMin() + sentPoints[i  ]*ofVec2f(kinectRect.width,kinectRect.height));
	}
	
	ofPopStyle();
	
//	// draw instructions
//	ofSetColor(255, 255, 255);
//	stringstream reportStream;
//	reportStream << "accel is: " << ofToString(kinect->getMksAccel().x, 2) << " / "
//	<< ofToString(kinect->getMksAccel().y, 2) << " / "
//	<< ofToString(kinect->getMksAccel().z, 2) << endl
//	<< "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
//	<< "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
//	<< "set near threshold " << nearThreshold << " (press: + -)" << endl
//	<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
//	<< ", fps: " << ofGetFrameRate() << endl
//	<< "press c to close the connection and o to open it again, connection is: " << kinect->isConnected() << endl
//	<< "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
//	<< "press 1-5 & 0 to change the led mode (mac/linux only)" << endl;
//	ofDrawBitmapString(reportStream.str(),20,652);
}


//--------------------------------------------------------------
void testApp::exit() {
	kinect->setCameraTiltAngle(0); // zero the tilt on exit
	kinect->close();	
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	if(key == ' '){
		shouldCaptureBackground = true;
	}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}
