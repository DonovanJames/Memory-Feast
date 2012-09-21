//
//  hydra.cpp
//  JointExample
//
//  Created by default on 8/22/12.
//  Copyright (c) 2012 James Nova. All rights reserved.
//

#include "hydra.h"

hydra::hydra() 
{
}

void hydra::setup(b2World* world, ofxBox2dCircle base, int sections){
        
    skull.setPhysics(3.0, 0, 0);
    jaw.setPhysics(3.0, 0, 0);
    headBase = base;
    skull.setup(world, base.getPosition().x, base.getPosition().y - 25 * sections , 40, 20);
    jaw.setup(world, base.getPosition().x+10, base.getPosition().y - 25 * sections, 40, 20);    
    
	for (int i = 0; i < sections; i++) {
        
        ofxBox2dRect rect;

        // Set collision parameters
        uint16 categoryBits;
        uint16 maskBits;
        
        maskBits = 0x0000; // sets all bones to not colide

        //rect.fixture.filter.categoryBits = categoryBits;
        rect.fixture.filter.maskBits = maskBits;
		
        rect.setPhysics(.5, 0, 0);
		rect.setup(world, base.getPosition().x+10, base.getPosition().y - (25 * (i + 1) ), 25, 30);
		
        bones.push_back(rect);
		
        b2RevoluteJointDef              revoluteJointDef; // create addressable joint
        b2RevoluteJoint*                m_joint; //create joint pointer
        
        // Add the first bone to the base
        if(i == 0) {
            
			//another revolute joint to connect the chain to the circle
			revoluteJointDef.bodyA = bones[i].body; //the first added link of the chain
			revoluteJointDef.bodyB = base.body;
			revoluteJointDef.localAnchorA.Set(0.75,0);//the regular position for chain link joints, as above
			revoluteJointDef.localAnchorB.Set(0,0);//a little in from the edge of the circle
			revoluteJointDef.lowerAngle = -180 * PI / 180;
			revoluteJointDef.upperAngle = 0 * PI / 180;
            revoluteJointDef.enableLimit = true;
            
            m_joint = (b2RevoluteJoint*)world->CreateJoint( &revoluteJointDef );
		}
        
        // Create the last joint with the head attached to the body
        else if(i == sections - 1) {
            
			revoluteJointDef.localAnchorA.Set( 0.75,0);
			revoluteJointDef.localAnchorB.Set(-0.75,0);
			revoluteJointDef.bodyA = bones[i].body;
			revoluteJointDef.bodyB = bones[i-1].body;
			revoluteJointDef.lowerAngle = -30 * DEGTORAD;
			revoluteJointDef.upperAngle = 30 * DEGTORAD;
			revoluteJointDef.maxMotorTorque = 100;
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.enableMotor = true;
            revoluteJointDef.motorSpeed = 0 * DEGTORAD;
            
            m_joint = (b2RevoluteJoint*)world->CreateJoint( &revoluteJointDef );
            
            
            b2RevoluteJointDef              neckJointDef; // create addressable joint
            neckJointDef.localAnchorA.Set( 1.5, -0.25);
			neckJointDef.localAnchorB.Set(-1, -0.9);
			neckJointDef.bodyA = skull.body;
			neckJointDef.bodyB = bones[i].body;
			neckJointDef.lowerAngle =  -30 * DEGTORAD;
			neckJointDef.upperAngle = 30 * DEGTORAD;
			neckJointDef.enableLimit = true;
            neckJointDef.maxMotorTorque = 100;
            neckJointDef.motorSpeed = 0 * DEGTORAD;
            //neckJointDef.enableMotor = true;
            
            neckJoint = (b2RevoluteJoint*)world->CreateJoint( &neckJointDef );
            
            
            b2RevoluteJointDef              jawJointDef; // create addressable joint
            jawJointDef.bodyA = jaw.body; //the last added link of the chain
            jawJointDef.bodyB = bones[i].body;
            jawJointDef.localAnchorA.Set(1.5, 0.25);
            jawJointDef.localAnchorB.Set(-1, 0.9);
            jawJointDef.lowerAngle = -30 * DEGTORAD;
            jawJointDef.upperAngle = 30 * DEGTORAD;
            jawJointDef.enableLimit = true;
            jawJointDef.maxMotorTorque = 60;
            jawJointDef.motorSpeed = 0 * DEGTORAD;
            //jawJointDef.enableMotor = true;
            
            jawJoint = (b2RevoluteJoint*)world->CreateJoint( &jawJointDef );

		}
        
        // adds all normal bones to the structure 
		else {
			revoluteJointDef.localAnchorA.Set( 0.75,0);
			revoluteJointDef.localAnchorB.Set(-0.75,0);
			revoluteJointDef.bodyA = bones[i].body;
			revoluteJointDef.bodyB = bones[i-1].body;
			revoluteJointDef.lowerAngle =  -30 * DEGTORAD;
			revoluteJointDef.upperAngle = 30 * DEGTORAD;
			revoluteJointDef.maxMotorTorque = 100;
			revoluteJointDef.enableLimit = true;
			//revoluteJointDef.enableMotor = true;
            revoluteJointDef.motorSpeed = 0 * DEGTORAD;
            
            m_joint = (b2RevoluteJoint*)world->CreateJoint( &revoluteJointDef );
		}
        
	joints.push_back(m_joint); 
    //place joints int revJoint vectors for later manipulation
	}  	
}

void hydra::update(){
    
    for (int i = 0; i < joints.size(); i++) {
        joints[i]->SetMotorSpeed(cosf(ofGetElapsedTimef()) * 50 * i);
	} 
    
    
    path.clear();
    line.clear();
    for ( int i = 0; i < bones.size(); i++){
        if(bones[i].body == NULL) {
            return;	
        }
		ofPolyline shape = bones[i].getRectangleShape();
        if(i==0){
            path.moveTo(shape[0]);
            path.curveTo(shape[0]);
            line.addVertex(shape[0]);
            line.addVertex(shape[0]);
        }
        else{
            path.curveTo(shape[0]);
            line.curveTo(shape[0]);
        }
    }
    
    ofPolyline skullShape = skull.getRectangleShape();
    ofPolyline jawShape = jaw.getRectangleShape();
    
    path.curveTo(skullShape[1]);
    path.curveTo(skullShape[0]);
    path.curveTo(skullShape[3]);
    path.curveTo(skullShape[2]);
    path.curveTo(jawShape[1]);
    path.curveTo(jawShape[0]);
    path.curveTo(jawShape[3]);
    path.curveTo(jawShape[2]);
    
    line.curveTo(skullShape[1]);
    line.curveTo(skullShape[0]);
    line.curveTo(skullShape[3]);
    line.curveTo(skullShape[2]);
    line.curveTo(jawShape[1]);
    line.curveTo(jawShape[0]);
    line.curveTo(jawShape[3]);
    line.curveTo(jawShape[2]);
    
    for ( int i = 0; i < bones.size(); i++){
        int j;
        j = bones.size() - (i +1);
        if(bones[j].body == NULL) {
            return;	
        }
		ofPolyline shape = bones[j].getRectangleShape();
        path.curveTo(shape[3]);
        line.curveTo(shape[3]);
        if (j == 0) {
            line.curveTo(shape[3]);
            path.curveTo(shape[3]);
        }
        if (i == 0) {
            line.curveTo(shape[3]);
            path.curveTo(shape[3]);
        }
    }

}

void hydra::draw(){
    ofPushStyle();
    
    ofSetHexColor(0x000000);
    ofSetLineWidth(10);
    line.getSmoothed(10,10.0f);
    line.draw();
    
    ofFill();
    ofEnableSmoothing();
    ofSetHexColor(0xFFFFFF);
    path.close();
    path.setPolyWindingMode(OF_POLY_WINDING_NONZERO);
    path.draw();

    ofPopStyle();
}


void hydra::setSpeed(float32 speed){
    for (int i = 0; i < joints.size(); i++) {
        joints[i]->SetMotorSpeed(speed * DEGTORAD);
    } 
}

void hydra::addAttraction(ofxBox2dCircle attractor){
	bones[(bones.size()-1)].addImpulseForce(attractor.getPosition(), ofVec2f( 10, 10));
}


