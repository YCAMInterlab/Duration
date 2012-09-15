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
		lifeSpanMultiplier = 1.0;;
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
	float lifeSpanMultiplier;
	bool hero;
	//todo connect them and have them break away
	vector<ColorParticle> nearNeighbors;
};

class ColorParticleController {

  public:
	float lifeSpanMin;
	float lifeSpanMax;
	float sparkleSpan;
	float lifeSpanMultiplier;
	ColorParticleController() {
		currentOffset = 0;
		amplitude = 0;
		density = 0;
		speed = 0;
		lifeSpanMin = 1.0;
		lifeSpanMax = 4;
		sparkleSpan = .05;

	};
	void addParticles(vector<ColorParticle>& newParticles){
		for(int i = particles.size()-1; i >= 0; i--){
			if(ofGetElapsedTimef() - particles[i].birthTime > particles[i].lifeSpanMultiplier*(lifeSpanMax + sparkleSpan)){
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

		float amplitude = .4;
		float density = 300;
		float speed = .018;
		currentOffset += speed;
		mesh.clear();
		heroMesh.clear();
//		for(int i = 0; i < particles.size(); i++){
		for(int i = particles.size()-1; i >= 0; i--){
			ColorParticle& p = particles[i];
			float time = ofGetElapsedTimef() - p.birthTime;
			if(time > 0){
				ofVec3f& pos = p.position;
				p.force =  ofVec3f(ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset)*amplitude,
								   ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset+1000)*amplitude,
								   ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset+2000)*amplitude );
				
				p.velocity += p.force;
				p.position += p.velocity;
				if(time > p.lifeSpanMultiplier*lifeSpanMax){
					p.color.a = 255;
				}
				else{
					p.color.a = ofMap(time, p.lifeSpanMultiplier*lifeSpanMin, p.lifeSpanMultiplier*lifeSpanMax, 255, 0, true);
				}
				//p.color.a = 255;
				//cout << "birth time " << time << " alpha " << int(p.color.a) << endl;
				if(p.hero){
					heroMesh.addColor(p.color);
					heroMesh.addVertex(p.position);
				}
				else{
					mesh.addColor(p.color);
					mesh.addVertex(p.position);
				}
			}

		}
	}
	
	void draw(){
		glPointSize(6.0);
		mesh.drawVertices();
		glPointSize(10.0);
		heroMesh.drawVertices();
	}
	
	ofMesh mesh;
	ofMesh heroMesh;
	vector<ColorParticle> particles;
	float currentOffset;
    float amplitude;
    float density;
    float speed;
};