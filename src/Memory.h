/*
 * Memory Feast - An interactive installation
 * by James Donovan
 *
 * Memory Particle
 *
 *
 * Thanks to:
 * James George 
 * Quaternion Examples
 *
 */

#pragma once
//#include <iostream>
//#include <string>
//#include <sstream>
//#include <algorithm>
//#include <iterator>


#include "ofMain.h"
#include "directory.h"

#include "MSAPerlin.h"


#define RESOLUTION_RATIO 32.0f


class memory {
    
public:
	void setup();
	void update();
	void draw();
	void move(int x, int y);
    void move(int x, int y, int ID);
	void set(int x, int y);
    void set(int x, int y, int ID);


    int partNum;
    
    
	//current state of the rotation  
    ofQuaternion curRot;  
	
    //a place to store the mouse position so we can measure incremental change  
    ofVec2f lastMouse;
	
	//slows down the rotation 1 = 1 degree per pixel
	float dampen;
    
    ofLight light;
    ofEasyCam cam;
    
	ofImage img;
	GLUquadricObj *quadric;
    GLUquadricObj *quadric2;
	
    // counters
	int currentQuadric;
	int lastSecond;  
	int currentImage; 
        
	directory FBImages;
    
    float rotation;
    
    vector <float> px, py, vx, vy;
    vector <float> prevx, prevy;
    vector <int> photo;
    MSA::Perlin noise;

};

