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
	currentIp = "localhost";
	currentPort = 1235;
	setupOSC();
	
	drawCursor = false;

	ui = new ofxUICanvas();
	ui->addWidgetRight( new ofxUISlider("far threshold", 0, 200, &farThreshold, 10, 250) );
	
	ip = ui->addTextInput("IP", "localhost", 200);
	ip->setAutoClear(false);
	port = ui->addTextInput("PORT", "12345", 200);
	port->setAutoClear(false);
	
	ui->loadSettings("GUI/guiSettings.xml");
	ofAddListener(ui->newGUIEvent, this, &testApp::guiEvent);

}

void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
	//	cout << "name is " << name << " kind is " << kind << endl;
    bool updateOSC = false;
	if(name == "IP"){
		string newIp = ip->getTextString();
		if(newIp != currentIp && (ofSplitString(newIp,".").size() == 4 || newIp == "localhost")){
			updateOSC = true;
			currentIp = newIp;
		}
	}
	else if(name == "PORT"){
		int newPort = ofToInt(port->getTextString());
		if(newPort != currentPort && newPort > 0 && newPort < 65535){
			currentPort = newPort;
			updateOSC = true;
		}
	}
	if(updateOSC){
		setupOSC();
	}
}

void testApp::setupOSC(){
   sender.setup(currentIp, currentPort);
}

//--------------------------------------------------------------
void testApp::update() {
	
	float insetPercent = .75;
	ofRectangle screenRect = ofRectangle( 0, 0, ofGetWidth(), ofGetHeight() );
	kinectRect = ofRectangle( 0, 0, 640, 480 );
	kinectRect = screenRect.scaleIntoMe(kinectRect);

	ofRectangle drawableArea = ofRectangle(0,0,640,480);
	drawableArea.scaleFromCenter(insetPercent);
	
	drawingRect = kinectRect;
	drawingRect.scaleFromCenter(insetPercent);

	
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
		contourFinder.findContours(subtracted, 300, (kinect->width*kinect->height)/2, 20, false);
		
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
			
			ofVec2f pointtosend = ofVec2f(ofMap(contourFinder.blobs[maxBlob].centroid.x,drawableArea.getMinX(), drawableArea.getMaxX(), 0.0, 1.0, true),
										  ofMap(contourFinder.blobs[maxBlob].centroid.y,drawableArea.getMinY(), drawableArea.getMaxY(), 0.0, 1.0, true));
			ofxOscMessage mx;
			mx.setAddress("/kinect/x");
			mx.addFloatArg(pointtosend.x);
			
			ofxOscMessage my;
			my.setAddress("/kinect/y");
			my.addFloatArg(pointtosend.y);
			
			ofxOscBundle b;
			b.addMessage(mx);
			b.addMessage(my);
			drawCursor = true;
			sender.sendBundle(b);
			//sentPoints.push_back( kinectRect.getMin() + pointtosend * ofVec2f(kinectRect.width,kinectRect.height) );
			sentPoints.push_back(pointtosend);
			while(sentPoints.size() > 60){
				sentPoints.erase(sentPoints.begin());
			}
		}
		else {
			drawCursor = false;
			if(sentPoints.size() > 0){
				sentPoints.erase(sentPoints.begin());
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackground(100, 100, 100);

	ofPushStyle();

	ofSetColor(255,255,255, 100);
	kinect->drawDepth(kinectRect);

	ofSetColor(255,255,255, 100);
	subtracted.draw(kinectRect);
	
	ofSetColor(255);
//	contourFinder.draw(kinectRect);
	
	ofSetColor(255, 200, 180);
	ofNoFill();
	
	ofRect(drawingRect);
	
	ofSetColor(220,120,40);
	ofMesh m;
	vector<ofVec2f> points;
	vector<ofFloatColor> colors;
	for(int i = 1; i < sentPoints.size(); i++){
		sentPoints[i].y += .01*powf(1.-1.0*i/sentPoints.size(), 3.0); //fall
		ofVec2f screenpoint = drawingRect.getMin() +  sentPoints[i-1]*ofVec2f(drawingRect.width,drawingRect.height);
		ofFloatColor color = ofFloatColor(1.0,1.0,1.0, powf(ofMap(i, 0, sentPoints.size(), 0, 1.0), 2.0));
		ofVec2f direction = (sentPoints[i-1] - sentPoints[i]).getNormalized();
		float width = powf(ofMap(i, 0, sentPoints.size(), 0, 1.0), 3.0)*60;
		colors.push_back(color);
		colors.push_back(color);
		points.push_back(screenpoint + direction.getRotated(90)*width/2);
		points.push_back(screenpoint - direction.getRotated(90)*width/2);
	}
	
	ofEnableAlphaBlending();
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, sizeof(ofFloatColor), &(colors[0].r));
	glVertexPointer(2, GL_FLOAT, sizeof(ofVec2f), &(points[0].x));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, points.size());
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	ofSetCircleResolution(30);
	if(drawCursor){
		ofNoFill();
		ofSetColor(250,120,40);
		ofPoint center = drawingRect.getMin() + sentPoints[sentPoints.size()-1]*ofVec2f(drawingRect.width,drawingRect.height);
		ofCircle(center, powf(1-sin(ofGetElapsedTimef()*7)*.5+.5,2)*+5+31);
		ofFill();
		ofSetColor(250,120,40,120);
		ofCircle(center, 30);
	}

	ofPopStyle();	
}

//--------------------------------------------------------------
void testApp::exit() {
	ui->saveSettings("GUI/guiSettings.xml");
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
