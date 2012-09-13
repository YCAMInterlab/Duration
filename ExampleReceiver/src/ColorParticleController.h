//
//  ColorParticleController.h
//  ExampleReceiver
//
//  Created by Jim on 9/12/12.
//
//

#pragma once

#include "ofMain.h"

class ColorParticle {
  public:
	ColorParticle(){
//		float birthTime;
//		ofColor color = ofColo4(0,0,0);
//		ofVec3f position = ofVec3f(0,0,0);
//		ofVec3f velocity = ofVec3f(0,0,0);
//		ofVec3f force = ofVec3f(0,0,0);
//		ofVec3f origin = ofVec3f(0,0,0);
	}
	float birthTime;
	ofColor color;
	ofVec3f position;
	ofVec3f velocity;
	ofVec3f force;
	ofVec3f origin;
	//todo connect them and have them break away
	vector<ColorParticle> nearNeighbors;
};

class ColorParticleController {

  public:
	
	ColorParticleController() {
		currentOffset = 0;
		amplitude = 0;
		density = 0;
		speed = 0;
	};
	
	void addParticles(vector<ColorParticle>& newParticles){
		for(int i = particles.size()-1; i >= 0; i--){
			if(ofGetElapsedTimef() - particles[i].birthTime > 5){
				particles.erase(particles.begin()+i);
			}
		}

		for(int i = 0; i < newParticles.size(); i++){
			particles.push_back(newParticles[i]);
		}
	}
	
	void purge(){
	}
	
	void update(){
		float amplitude = .08;
		float density = 600;
		float speed = .007;
		currentOffset += speed;
		mesh.clear();
//		for(int i = 0; i < particles.size(); i++){
		for(int i = particles.size()-1; i >= 0; i--){
			ColorParticle& p = particles[i];
			ofVec3f& pos = p.position;
			p.force =  ofVec3f(ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset)*amplitude,
							   ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset+1000)*amplitude,
							   ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset+2000)*amplitude );
			
			p.velocity += p.force;
			p.position += p.velocity;
			mesh.addVertex(p.position);
			p.color.a = ofMap(p.birthTime, ofGetElapsedTimef()-5, ofGetElapsedTimef()-10, 255, 0);
			mesh.addColor(p.color);
		}
	}
	
	void draw(){
		glPointSize(4.0);
		mesh.drawVertices();
	}
	
	ofMesh mesh;
	vector<ColorParticle> particles;
	float currentOffset;
    float amplitude;
    float density;
    float speed;
};