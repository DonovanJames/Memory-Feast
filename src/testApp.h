#ifndef _TEST_APP
#define _TEST_APP

#include "ParticleFluid.h"
#include "ofxOpenNI.h"
#include "ofxOpenCv.h"
#include "ofMain.h"
#include "ofxBox2d.h"
#include "hydra.h"
#include "userMask.h"
#include "memory.h"


#define MAX_DEVICES 2


class testApp : public ofBaseApp{

public:
    // OF Core calls
	void setup();
	void update();
	void draw();
    void exit();
    
    
    // OF interaction calls
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
    
    // OpenNI calls
    void userEvent(ofxOpenNIUserEvent & event);
	ofxOpenNI               openNIDevices[MAX_DEVICES];
    ofxOpenNIUser           user;
    ofxOpenNIJoint          rightHand;
    ofxOpenNIJoint          leftHand;
    int                     numDevices;
    bool                    foundUser;

    // Open CV user mask solver
    ofxUserMask             userMask;
    
    ParticleFluid           fluidSystem;
    MSA::Vec2f				pMouse;
    MSA::Vec2f				pLeftHand;
	MSA::Vec2f				pRightHand;
    
    
    
    // Memory Setup
    memory::memory          memoryParticle;
    
    
    // ofxBox2D
    ofxBox2d                box2d;			  //	the box2d world
	vector		<hydra>     heads;			  //	defalut box2d rects
    ofxBox2dCircle          base;
    ofxBox2dRect            floor;
    ofxBox2dPolygon         hydraBody;
    vector <ofxBox2dCircle> bases;
    ofPath                  path;
    
    // debug font
    ofTrueTypeFont          verdana;
};

#endif
