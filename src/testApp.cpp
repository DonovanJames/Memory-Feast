#include "testApp.h"

using namespace MSA;

//--------------------------------------------------------------
void testApp::setup() {
    // app setup
    ofSetFrameRate(60);
	ofBackground(255, 255, 255);
	ofSetVerticalSync(false);
    
    fluidSystem.setup();
	
	ofEnableAlphaBlending();
	ofSetBackgroundAuto(true);

    // OpenNI Setup
    ofSetLogLevel(OF_LOG_NOTICE);
            
    numDevices = openNIDevices[0].getNumDevices();
    
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        openNIDevices[deviceID].setup();
        openNIDevices[deviceID].addDepthGenerator();
        openNIDevices[deviceID].addImageGenerator();
        openNIDevices[deviceID].addUserGenerator();
        openNIDevices[deviceID].setRegister(true);
        openNIDevices[deviceID].setMirror(true);
		openNIDevices[deviceID].start();
    }
    
    openNIDevices[0].setMaxNumUsers(4); // defualt is 4
    ofAddListener(openNIDevices[0].userEvent, this, &testApp::userEvent);
    
    ofxOpenNIUser user;
    user.setUseMaskTexture(true);
    user.setUsePointCloud(true);
    user.setPointCloudDrawSize(2); // this is the size of the glPoint that will be drawn for the point cloud
    user.setPointCloudResolution(2); // this is the step size between points for the cloud -> eg., this sets it to every second point
    openNIDevices[0].setBaseUserClass(user); // this becomes the base class on which tracked users are created
    
    userMask.setup();
    
    
    // Box2D setup
    box2d.init();
	box2d.setGravity(0, -0.1);
	box2d.setFPS(30.0);
	box2d.registerGrabbing();

    
	int iHeads = 6;
	int distance = 60;
    
    //base.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()/2, 60);
    
    floor.setup(box2d.getWorld(), -ofGetWidth()/2, ofGetHeight(), ofGetWidth()*2, 5);
    
    for (int h = 0; h < iHeads; h++){
        ofxBox2dCircle neck;
        neck.setup(box2d.getWorld(), ((h + 1) * distance) + (ofGetWidth() - (iHeads + 1) * distance)/2, ofGetHeight()*.8, 5); 
        neck.fixture.filter.maskBits = 0x0000;
        // create base circle where the backbone is attached.
        hydra::hydra head;
        head.setup(box2d.getWorld(), neck, 12);
		heads.push_back(head);
        bases.push_back(neck);
	}
    
    //Memory Setup
    memoryParticle.setup();
     
}

//--------------------------------------------------------------
void testApp::update(){   
    
    // Box2D update
    box2d.update();
    
    for (int i = 0; i < heads.size(); i++){
        heads[i].update();
	}
    
    hydraBody.destroy();
    path.clear();
    
    path.moveTo(800/4, 600);
    path.curveTo(800/4, 600);
    path.curveTo(800/4, 600);
    hydraBody.addVertex(800/4, 600);
    
    for (int i = 0; i < heads.size(); i++){
        vector <ofPoint> parts = heads[i].line.getVertices();
        hydraBody.curveTo(parts[parts.size()-1]);
        if (i == 0){
            hydraBody.curveTo(parts[parts.size()-1]);
        }
        hydraBody.curveTo(parts[0]);
        if (i == heads.size() - 1){
            hydraBody.curveTo(parts[0]);
        }
        path.curveTo(parts[parts.size()-1]);
        path.curveTo(parts[0]);
	}
    hydraBody.addVertex(800/4 * 3, 600);
    path.curveTo(800/4 * 3, 600);
    path.curveTo(800/4 * 3, 600);
    path.setPolyWindingMode(OF_POLY_WINDING_NONZERO);
    hydraBody.create(box2d.getWorld());
    
    //Memory update
    memoryParticle.update();
    
    //Fluid update
    fluidSystem.update();
    
    // Set bNewFrame to false at the start of every update
    bool bNewFrame = false;
    foundUser = false;
    // Update the data from the Kinect device(s). Then Update Tracked Users.
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        openNIDevices[deviceID].update();
        int numUsers = openNIDevices[0].getNumTrackedUsers();
        for (int nID = 0; nID < numUsers; nID++){
            ofxOpenNIUser & user = openNIDevices[0].getTrackedUser(nID);
            // update userMask with pixel information
            userMask.update(user.getMaskPixels());

            // establish hand tracking
            leftHand = user.getJoint(JOINT_LEFT_HAND);
            rightHand = user.getJoint(JOINT_RIGHT_HAND);
            foundUser = true;
        } 
    }
    
    if (leftHand.isFound()) {
        Vec2f leftHandPos = Vec2f(leftHand.getProjectivePosition().x, leftHand.getProjectivePosition().y);
        Vec2f leftHandNorm = Vec2f( leftHandPos) / Vec2f(640,480);
        Vec2f leftHandVel = Vec2f( leftHandPos - pLeftHand ) / Vec2f(640,480);
        fluidSystem.addToFluid( leftHandNorm, leftHandVel, true, true );
        pLeftHand = leftHandPos;
        memoryParticle.move(leftHandPos.x, leftHandPos.y);

    }
    
    if (rightHand.isFound()) {
        Vec2f rightHandPos = Vec2f(rightHand.getProjectivePosition().x, rightHand.getProjectivePosition().y);
        Vec2f rightHandNorm = Vec2f( rightHandPos) / Vec2f(640,480);
        Vec2f rightHandVel = Vec2f( rightHandPos - pRightHand ) / Vec2f(640,480);
        fluidSystem.addToFluid( rightHandNorm, rightHandVel, true, true );
        pRightHand = rightHandPos;

    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackgroundGradient(ofColor(abs(sin(ofGetElapsedTimef()*0.01)*255),
                                 abs(sin(ofGetElapsedTimef()*0.03)*255),
                                 abs(sin(ofGetElapsedTimef()*0.09)*255)),
                         ofColor(0));
    

    ofPushMatrix();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    int numUsers = openNIDevices[0].getNumTrackedUsers();
    for (int nID = 0; nID < numUsers; nID++){
        ofxOpenNIUser & user = openNIDevices[0].getTrackedUser(nID);
        user.drawMask();
        ofPushMatrix();
        ofTranslate(320, 240, -1000);
        user.drawPointCloud();
        ofPopMatrix();
    }
    ofDisableBlendMode();
    ofPopMatrix();
    
    
    //Draw rainbow water
    fluidSystem.draw();

    //Memory update
    memoryParticle.draw();

    
    // Draw the Hydra
    ofPushStyle();
    ofScale(.7, .7);
    ofTranslate(150, 300);
    ofSetLineWidth(10);
    ofFill();
    ofSetHexColor(0x000000);
    hydraBody.draw();
    for ( int i = 0; i < heads.size(); i++){
		heads[i].draw();
    }
    ofSetHexColor(0xFFFFFF);
    path.draw();
    ofPopStyle();
    

    

    
    if (foundUser){
        userMask.draw();
    }
    //userMask.draw();

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {

}


//--------------------------------------------------------------
void testApp::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void testApp::exit(){
    // this often does not work -> it's a known bug -> but calling it on a key press or anywhere that isnt std::aexit() works
    // press 'x' to shutdown cleanly...
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        openNIDevices[deviceID].stop();
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (key == 't'){
        fluidSystem.setVariables(true, false, true);

    } 
    else if (key == 's' || key == 'S' ){
        ofImage screenGrab;
        screenGrab.grabScreen(0, 0, ofGetWidth(), ofGetHeight()); 
        screenGrab.saveImage("MemoryFeast" + ofGetTimestampString() + ".png", OF_IMAGE_QUALITY_BEST);
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	Vec2f eventPos = Vec2f(x, y);
	Vec2f mouseNorm = Vec2f( eventPos ) / getWindowSize();
	Vec2f mouseVel = Vec2f( eventPos - pMouse ) / getWindowSize();
	fluidSystem.addToFluid( mouseNorm, mouseVel, true, true );
	pMouse = eventPos;
    
    memoryParticle.move(x, y);
    
    for ( int i = 0; i < heads.size(); i++){
		heads[i].setSpeed(x-y);
    }
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    memoryParticle.set(x, y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}


