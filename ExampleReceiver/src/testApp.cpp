#include "testApp.h"
#define CIRCLE_RADIUS 20

//TODO:
//make the circles show up on-demand
//add particle trail system
//add commands
	// in/out of screen
	// bang sends particle explosion
	// flag makes speech bubble

// consider more fun background effects (water ripples)

//--------------------------------------------------------------
void testApp::setup(){
	
//	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground(30, 30, 130);
	ofEnableAlphaBlending();
	
	ofToggleFullscreen();
	
	ofSetCircleResolution(200);
	
	ofEnableSmoothing();
	
	receiver.setup(PORT);

	font.loadFont("NewMedia Fett.ttf", 30);
	
	//gui...?
	doGraph = false;
	maxDistance = 80;
	minDistance = .2;
	maxWidth = 50;
	minWidth = 2;
	
				  
	fbo.allocate(ofGetWidth(),ofGetHeight(),GL_RGB,4);
	fbo.begin();
	ofClear(30, 30, 130);
	fbo.end();
	
	startThread();
}

//--------------------------------------------------------------
void testApp::update(){
}

void testApp::threadedFunction(){
	while(isThreadRunning()){

		int numMessages = 0;
		// check for waiting messages
		
		while(receiver.hasWaitingMessages()){
			// get the next message
			ofxOscMessage m;
			receiver.getNextMessage(&m);
			numMessages++;
			
			//get message components:
			vector<string> components = ofSplitString(ofToLower(m.getAddress()),"/", true,true);
			if(components.size() == 1){
				//check for flag quotes
				if(m.getNumArgs() == 1 && m.getArgType(0) == OFXOSC_TYPE_STRING && hasCircleWithName(components[0])){
					circleWithName(components[0]).setQuote(m.getArgAsString(0));
				}
				//check for global events
			}
			//check for commands
			else if(components.size() == 2){

				if(m.getNumArgs() == 0){
					//bang!
					ControlCircle& circle = circleWithName(components[0]);
					circle.bang();
				}
				else {
					if(components[1] == "x" ||
					   components[1] == "sidetoside")
					{
						if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
							ControlCircle& circle = circleWithName(components[0]);
							float x = m.getArgAsFloat(0);
							circle.setX(ofMap(x, 0, 1.0, CIRCLE_RADIUS, ofGetWidth()-CIRCLE_RADIUS));
						}
					}
					else if(components[1] == "y" ||
							components[1] == "upanddown")
					{
						if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
							ControlCircle& circle = circleWithName(components[0]);
							float y = m.getArgAsFloat(0);
							circle.setY(ofMap(y, 0, 1.0, CIRCLE_RADIUS, ofGetHeight()-CIRCLE_RADIUS));
						}
					}
					else if(components[1] == "z" ||
							components[1] == "inandout")
					{
						if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){
							ControlCircle& circle = circleWithName(components[0]);
							float z = m.getArgAsFloat(0);
							circle.setZ(ofMap(z, 0, 1.0, 0, 500));
						}
					}
					else if(components[1] == "color"){
						if(m.getNumArgs() >= 3 &&
						   m.getArgType(0) == OFXOSC_TYPE_INT32 &&
						   m.getArgType(1) == OFXOSC_TYPE_INT32 &&
						   m.getArgType(2) == OFXOSC_TYPE_INT32)
						{
							ControlCircle& circle = circleWithName(components[0]);
							circle.setColor(ofColor(m.getArgAsInt32(0),m.getArgAsInt32(1),m.getArgAsInt32(2)));
						}						
					}
				}
			}
			else {
				//too many components!
			}
		}
		
		if(numMessages > 0){
			//push updates
			map<string, ControlCircle>::iterator it;
			for(it = circles.begin(); it != circles.end(); it++){
				ControlCircle& circle = it->second;
				ofVec3f lastPosition = circle.getLastPosition();
				ofVec3f position = circle.getPosition();
				if(lastPosition != position){

					ofColor color = circle.getColor();
					float dist = lastPosition.distance(position);
					
					ControlCircleTrail trail;
					trail.color = color;
					
					float width =  ofMap(dist, maxDistance, minDistance, minWidth, maxWidth, true);
					ofVec3f direction = position - lastPosition;
					ofVec3f left = direction.getCrossed(ofVec3f(0,0,1)).normalized();
					ofVec3f right = left.getRotated(180, direction);
					
					trail.left = position + left*width/2;
					trail.right = position + right*width/2;
					
					trail.birthTime = ofGetElapsedTimef();
					circle.trail.push_back(trail);
					//trails.push_back(trail);
				}

				//moves position to lastPosition
				circle.update();
			}
		}
		
		ofSleepMillis(1);
	}
//	cout << "num messages " << numMessages << endl;
}

//--------------------------------------------------------------
bool testApp::hasCircleWithName(string name){
	return circles.find(name) != circles.end();
}

//--------------------------------------------------------------
ControlCircle& testApp::circleWithName(string name){
	if(!hasCircleWithName(name)){
		cout << "Creating circle with name" << name << endl;
		circles[name] = ControlCircle();
		circles[name].setName(name);
		circles[name].setPosition(ofVec2f(.5,.5));
	}
	return circles[name];
}

//--------------------------------------------------------------
void testApp::draw(){
	fbo.begin();
	ofClear(30, 30, 130);
	ofEnableSmoothing();
	ofEnableAlphaBlending();
	ofPushStyle();
//	ofSetColor(30, 30, 130, 1);
//	ofRect(0, 0, ofGetWidth(), ofGetHeight());
//	ofPopStyle();
	
	ofMesh m;

//	vector<ofVec3f>& vertices = m.getVertices();
//	vector<ofFloatColor>& colors = m.getColors();
//	glBegin(GL_TRIANGLE_STRIP);
//	for(int i = 0; i < trails.size(); i++){
//		glColor4f(trails[i].color.r/255.0, trails[i].color.g/255.0, trails[i].color.b/255.0,
//				  ofMap(trails[i].birthTime, ofGetElapsedTimef()-20, ofGetElapsedTimef()-60, 1.0, 0, true));
//		glVertex3fv(&trails[i].left.x);
//		glVertex3fv(&trails[i].right.x);
//		
//	}
//	glEnd();
	
//	cout << "trail size is " << m.getVertices().size() << endl;
	map<string, ControlCircle>::iterator it;
	for(it = circles.begin(); it != circles.end(); it++){
		ControlCircle& circle = it->second;
		

		
		vector<ofVec3f> vertices;
		vector<ofFloatColor> colors;
		for(int i = 1; i < circle.trail.size(); i++){
			ofFloatColor c = ofFloatColor(circle.trail[i].color.r/255.0,
							 			  circle.trail[i].color.g/255.0,
										  circle.trail[i].color.b/255.0,
										  ofMap(circle.trail[i].birthTime, ofGetElapsedTimef(), ofGetElapsedTimef()-20, 1.0, 0, true));
			colors.push_back(c);
			colors.push_back(c);
			vertices.push_back(circle.trail[i].left);
			vertices.push_back(circle.trail[i].right);
		}
		
		ofEnableAlphaBlending();
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, sizeof(ofFloatColor), &(colors[0].r));
		glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &(vertices[0].x));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		
		ofPushStyle();
		ofFill();
		ofSetColor( circle.getColor());
		ofCircle( circle.getPosition(), CIRCLE_RADIUS*2);
		
		ofNoFill();
		ofSetLineWidth(3);
		ofSetColor(0);
		ofCircle( circle.getPosition(), CIRCLE_RADIUS*2);
		
		ofSetColor(255,255,255);
		ofPushMatrix();
		ofTranslate(0, 0, circle.getPosition().z);
		font.drawString(circle.getName(),
						circle.getPosition().x-CIRCLE_RADIUS*.5,
						circle.getPosition().y+CIRCLE_RADIUS*.5);
		ofPopMatrix();
		ofPopStyle();

	}
	
	if(doGraph){
		font.drawString(ofToString(ofGetFrameRate()), 30, 30);
		ofPolyline p;
		for(int i = 0; i < MIN(xs.size(), ofGetWidth()); i++){
			p.addVertex(i,xs[xs.size()-i-1]*ofGetHeight()/2);
		}
		
		p.draw();
		p.clear();
		for(int i = 0; i < MIN(ys.size(), ofGetWidth()); i++){
			p.addVertex(i,ys[ys.size()-i-1]*ofGetHeight()/2+ofGetHeight()/2);
		}
		p.draw();
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
	fbo.allocate(w,h,GL_RGB, 4);
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

//--------------------------------------------------------------
void testApp::exit(){
	waitForThread(true);
}
