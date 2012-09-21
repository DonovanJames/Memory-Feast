//
//  hydra.cpp
//  JointExample
//
//  Created by default on 8/22/12.
//  Copyright (c) 2012 James Nova. All rights reserved.
//

#include "ParticleFluid.h"

ParticleFluid::ParticleFluid() 
{
}

void ParticleFluid::setup(){
    // MSA Fluid Setup

    // setup fluid stuff
    fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true).setFadeSpeed(0.002).setDeltaT(0.5).setVisc(0.00015).setColorDiffusion(0);
    fluidDrawer.setup( &fluidSolver );
    particleSystem.setFluidSolver( &fluidSolver );

    fluidCellsX			= 150;

    drawFluid			= true;
    drawParticles		= false; // draw random particle stream

    resized(ofGetWidth(), ofGetHeight());		// force this at start (cos I don't think it is called)
    resizeFluid			= true;

    ofEnableAlphaBlending();
    ofSetBackgroundAuto(true);
}

void ParticleFluid::update(){
    // MSA Fluid update methods
    if(resizeFluid) 	{
		fluidSolver.setSize(fluidCellsX, fluidCellsX / getWindowAspectRatio() );
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
	}

    if (randColor){
        fluidSolver.randomizeColor();    
    }
    fluidSolver.update();
    
    randColor = false;
}

void ParticleFluid::draw(){
    if( drawFluid ) {
		glColor3f(1, 1, 1);
		fluidDrawer.draw(0, 0, getWindowWidth(), getWindowHeight());//(640, 500, 640, 480);//
	} else {
		if(getElapsedFrames()%5==0) fadeToColor( 0, 0, 0, 0.1f );
	}
	if( drawParticles )
		particleSystem.updateAndDraw( drawFluid );
}

void ParticleFluid::resized(int w, int h) {
    particleSystem.setWindowSize( Vec2f( w, h ) );
}

//--------------------------------------------------------------
void ParticleFluid::fadeToColor(float r, float g, float b, float speed) {
    glColor4f(r, g, b, speed);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
}

// add force and dye to fluid, and create particles
void ParticleFluid::addToFluid( Vec2f pos, Vec2f vel, bool addColor, bool addForce ) {
    float speed = vel.x * vel.x  + vel.y * vel.y * getWindowAspectRatio() * getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
    if(speed > 0) {
		pos.x = constrain(pos.x, 0.0f, 1.0f);
		pos.y = constrain(pos.y, 0.0f, 1.0f);
		
        const float colorMult = 255;
        const float velocityMult = 150;
		
        int index = fluidSolver.getIndexForPos(pos);
		
		if(addColor) {
			Color drawColor( CM_HSV, ( getElapsedFrames() % 120 ) / 120.0f, 1, 1 );
            //			Color drawColor;
            //			drawColor.setHSV(( getElapsedFrames() % 360 ) / 360.0f, 1, 1 );
			
			fluidSolver.addColorAtIndex(index, drawColor * colorMult);
			
			if( drawParticles )
				particleSystem.addParticles( pos * Vec2f( getWindowSize() ), 5 );
		}
		
		if(addForce)
			fluidSolver.addForceAtIndex(index, vel * velocityMult);
		
    }
}

void ParticleFluid::setVariables(bool fluid, bool particles, bool color){
    drawFluid = fluid;
    drawParticles = particles;
    randColor = color;
}
