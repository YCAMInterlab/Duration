//
//  ColorParticleController.h
//  ExampleReceiver
//
//  Created by Jim on 9/12/12.
//
//

#pragma once

#include "ofMain.h"
#include "ColorParticle.h"

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
	
	ColorParticleController {
		currentOffset = 0;
		amplitude = 0;
		density = 0;
		speed = 0;
	};
	
	vector<ColorParticle> particles;
	void update(){
		
		for(int i = 0; i < particles.size(); i++){
			ColorParticle& p = particles[p];
            p.force =  ofVec3f(ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset)*amplitude,
							   ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset+1000)*amplitude,
                               ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset+2000)*amplitude );
			
			p.velocity += p.force;
			p.position += p.velocity;
		}
	}
	
	void draw(){
		mesh.drawVertices();
	}
	
	ofMesh mesh;
	float currentOffset;
    float amplitude;
    float density;
    float speed;
};