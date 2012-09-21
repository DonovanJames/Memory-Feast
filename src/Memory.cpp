//
//  directory.cpp
//  QuaternionArcballExample
//
//  Created by default on 6/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "Memory.h"

//--------------------------------------------------------------
void memory::setup(){
	
    partNum = 6;
    
	//this slows down the rotate a little bit
	dampen = .4;
	
    ofPushStyle();
    
    ofEnableSmoothing();
    
//    //glEnable(GL_CULL_FACE); //causes visual oclusions with open cylynder
//    glCullFace(GL_FRONT);    
//    glEnable(GL_DEPTH_TEST); //enable depth comparisons and update the depth buffer
    ofDisableArbTex(); //needed for textures to work with gluSphere	
    
    // Light Setup
    light.enable();
    light.setDirectional();
    light.setPosition(0,0,150);
    ofFloatColor ambient_color(1.0,1.0,1.0,1.0);
    ofFloatColor diffuse_color(1.0,1.0,1.0);
    ofFloatColor specular_color(0.0,1.0,0.0);
    light.setAmbientColor(ambient_color);
    light.setDiffuseColor(diffuse_color);
    light.setSpecularColor(specular_color);

    
    noise.setup();
    
	//prepare quadric for sphere and ring
    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_FLAT);

    ofPopStyle();
    
    rotation = (ofRandomuf()-0.5) * 0.05;


    // Setup counters
    lastSecond = ofGetSeconds();
	currentImage = 0;
    
    FBImages.loadImg("memories");

    for(int i=0; i < partNum; i++){
        px.push_back(ofRandom(ofGetWidth()));
        py.push_back(ofRandom(ofGetHeight()));
        vx.push_back(0);
        vy.push_back(0);
        prevx.push_back(0);
        prevy.push_back(0);
        photo.push_back(i);
    }

}

//--------------------------------------------------------------
void memory::update(){
    
    for(int i = 0; i < partNum; i++){
    	// (1) we increase px and py by adding vx and vy
        
        float nX = px[i] * 0.005f;
        float nY = py[i] * 0.005f;
        float nZ = ofGetElapsedTimef() * 0.1f;
        ofVec3f v( nX, nY, nZ );
        float FBM = noise.get( v.x, v.y, v.z );

        px[i] += vx[i];
        py[i] += vy[i];
        
        if (vx[i] > 1) {
            px[i] += FBM * 10;
        }
        
        if (vy[i] > 1) {
            py[i] += FBM * 10;

        }
    
	
	// (2) check for collision, and trigger sounds:
	// horizontal collisions:
        if (px[i] < 0){
            px[i] = 0;
            vx[i] *= -1;
            photo[i] += partNum;
            photo[i] = photo[i] % (FBImages.images.size() - 1 );
        } else if (px[i] > ofGetWidth() - 30){
            px[i] = ofGetWidth()-30;
            vx[i] *= -1;
            photo[i] += partNum;
            photo[i] = photo[i] % (FBImages.images.size() - 1 );
        }
	// vertical collisions:
        if (py[i] < 30 ){
            py[i] = 30;
            vy[i] *= -1;
            photo[i] += partNum;
            photo[i] = photo[i] % (FBImages.images.size() - 1 );
        } else if (py[i] > ofGetHeight() - 30){
            py[i] = ofGetHeight()-30;
            vy[i] *= -1;
            photo[i] += partNum;
            photo[i] = photo[i] % (FBImages.images.size() - 1 );
        }
	// (3) slow down velocity:
        vx[i] 	*= 0.998;
        vy[i] 	*= 0.998;
    }
	
	if ((ofGetSeconds() % 8) == 0 && lastSecond != ofGetSeconds()){
		lastSecond = ofGetSeconds();
		currentImage++;
		currentImage %= FBImages.images.size();
	}
	
}



//--------------------------------------------------------------
void memory::draw(){

	//translate so that 0,0 is the center of the screen
    ofPushMatrix();
    
    ofRotateX(-90);
    for (int i = 0; i < partNum; i++) {
        ofPushMatrix();
        FBImages.images[photo[i]].getTextureReference().bind();
        ofTranslate(px[i], 0, py[i]);
        gluSphere(quadric, 32, 36, 36);
        FBImages.images[photo[i]].getTextureReference().unbind();
        ofPopMatrix();
    }
    ofPopMatrix();

	
}

//--------------------------------------------------------------
void memory::move(int x, int y){
    
    // add into vx and vy a small amount of the change in mouse:
	for(int i = 0; i < partNum; i++){
        
        
        float nX = px[i] * 0.005f;
        float nY = py[i] * 0.005f;
        float nZ = ofGetElapsedTimef() * 0.1f;
        ofVec3f v( nX, nY, nZ );
        float FBM = noise.get( v.x, v.y, v.z );  
        
        if (abs(vx[i]) < 15) {
            vx[i] += (x - prevx[i]) / 10.0f * FBM;
        }
        if (abs(vy[i]) < 15) {
            vy[i] += (y - prevy[i]) / 10.0f * FBM;
        }
        
        // store the previous mouse position:
        prevx[i] = x;
        prevy[i] = y; 
    }
}

//--------------------------------------------------------------
void memory::move(int x, int y, int ID){
	
    // add into vx and vy a small amount of the change in mouse:
	
    vx[ID] += (x - prevx[ID]) / 20.0f;
	vy[ID] += (y - prevy[ID]) / 20.0f;
	// store the previous mouse position:
	prevx[ID] = x;
	prevy[ID] = y; 
}

//--------------------------------------------------------------
void memory::set(int x, int y){
	for(int i = 0; i < partNum; i++){
        prevx[i] = x;
        prevy[i] = y;
    }
}

//--------------------------------------------------------------
void memory::set(int x, int y, int ID){
    prevx[ID] = x;
    prevy[ID] = y;
}

