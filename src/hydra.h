//
//  hydra.h
//  JointExample
//
//  Created by default on 8/22/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"
#include "Box2D.h"
#include "ofxBox2dUtils.h"

#define DEGTORAD 0.0174532925f
#define RADTODEG 57.2957795
#define PIDEF 3.14159265359


class hydra {
	
public:	
    hydra();
	void setup(b2World* world, ofxBox2dCircle base, int sections=9);
    void update();
    void draw();
    void setSpeed(float32 speed);
	void addAttraction(ofxBox2dCircle attractor);
//    ofPolyline getLine();
//    ofPath getPath();

    vector      <b2RevoluteJoint*>  joints;             //  pointers to joints
	vector		<ofxBox2dRect>		bones;              //	defalut box2d rects
    
    b2RevoluteJoint*                jawJoint;           //  create joint pointer
    b2RevoluteJoint*                neckJoint;          //  create joint pointer
    ofxBox2dCircle                  headBase;
    
    ofxBox2dCircle					head;               //	fixed anchor
    ofxBox2dRect                    skull;
    ofxBox2dRect                    jaw;
    
    ofPath                          path;
    ofPolyline                      line;

};