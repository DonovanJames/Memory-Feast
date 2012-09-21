//
//  hydra.h
//  MSA Fluid Solver
//
//

#pragma once
#include "ofMain.h"
#include "MSAFluid.h"
#include "MSATimer.h"
#include "ParticleSystem.h"

using namespace MSA;

class ParticleFluid {
	
public:	
    
    ParticleFluid();
	void setup();
    void update();
    void draw();
    void resized(int w, int h);  

    // MSA Fluid calls
    void fadeToColor(float r, float g, float b, float speed);
    void addToFluid(Vec2f pos, Vec2f vel, bool addColor, bool addForce);
    void setVariables(bool fluid, bool particles, bool color);
    
    MSA::FluidSolver        fluidSolver;
    MSA::FluidDrawerGl      fluidDrawer;	
    ParticleSystem          particleSystem;
    
    //constants
    int                     fluidCellsX;
    bool                    resizeFluid;
    bool                    drawFluid;
    bool                    drawParticles;
    bool                    randColor;

    
};

