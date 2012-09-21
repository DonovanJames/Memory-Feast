#include "testApp.h"

using namespace MSA;

char sz[] = "[Rd9?-2XaUP0QY[hO%9QTYQ`-W`QZhcccYQY[`b";

//--------------------------------------------------------------
void testApp::setup() {
    
    // app setup
    ofSetFrameRate(60);
	ofBackground(0, 0, 0);
	ofSetVerticalSync(false);
    
    // MSA Fluid Setup
    for(int i=0; i<strlen(sz); i++) sz[i] += 20;
	
	// setup fluid stuff
	fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true).setFadeSpeed(0.002).setDeltaT(0.5).setVisc(0.00015).setColorDiffusion(0);
	fluidDrawer.setup( &fluidSolver );
	particleSystem.setFluidSolver( &fluidSolver );
	
	fluidCellsX			= 150;
	
	drawFluid			= true;
	drawParticles		= false;
    
    windowResized(ofGetWidth(), ofGetHeight());		// force this at start (cos I don't think it is called)
	pMouse = getWindowCenter();
	resizeFluid			= true;
	
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
    
    openNIDevices[0].setMaxNumUsers(1); // defualt is 4
    ofAddListener(openNIDevices[0].userEvent, this, &testApp::userEvent);
    
    ofxOpenNIUser user;
    user.setUseMaskTexture(true);
    user.setUsePointCloud(true);
    user.setPointCloudDrawSize(2); // this is the size of the glPoint that will be drawn for the point cloud
    user.setPointCloudResolution(2); // this is the step size between points for the cloud -> eg., this sets it to every second point
    openNIDevices[0].setBaseUserClass(user); // this becomes the base class on which tracked users are created
    
    // OpenCV Image Allocation
    colorImg.allocate(640,480);
	grayImage.allocate(640,480);
	grayBg.allocate(640,480);
	grayDiff.allocate(640,480);
    
    // Allocate pixels
    userMaskPixels.allocate(640, 480, OF_IMAGE_COLOR_ALPHA);
    greyScaleMask.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    
    // openCV parameters 
	bLearnBakground = true;
	threshold = 80;
    
    // Font load
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
}

//--------------------------------------------------------------
void testApp::update(){
    ofBackground(0, 0, 0);
    
    // MSA Fluid update methods
    if(resizeFluid) 	{
		fluidSolver.setSize(fluidCellsX, fluidCellsX / getWindowAspectRatio() );
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
	}
    fluidSolver.update();
    
    // Set bNewFrame to false at the start of every update
    bool bNewFrame = false;
    
    // Update the data from the Kinect device(s). Then Update Tracked Users.
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        openNIDevices[deviceID].update();
        int numUsers = openNIDevices[0].getNumTrackedUsers();
        for (int nID = 0; nID < numUsers; nID++){
            ofxOpenNIUser & user = openNIDevices[0].getTrackedUser(nID);

            // Set the bNewFrame to true if there is a pixel mask.
            bNewFrame = user.getUseMaskPixels();
            
            // allocate greyscale pixels for openCV analysis.
            userMaskPixels = user.getMaskPixels();
            greyScaleMask.setChannel(0, userMaskPixels.getChannel(0));
            
            // establish hand tracking
            leftHand = user.getJoint(JOINT_LEFT_HAND);
            rightHand = user.getJoint(JOINT_RIGHT_HAND);
        } 
    }
        
    if (bNewFrame) {
        
        // set the pixel mask to an openCV grayscale image
        grayImage.setFromPixels(greyScaleMask);

        if (bLearnBakground == true){
            // create a pure white background image
            bgSet.allocate(640, 480, OF_IMAGE_GRAYSCALE);
            bgSet.set(255);
            grayBg.setFromPixels(bgSet);
            bLearnBakground = false;
        }
    
        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);
    
        contourFinder.findContours(grayDiff, 20, (640*480)/3, 10, true);	// find holes
    }
    
    if (leftHand.isFound()) {
        Vec2f leftHandPos = Vec2f(leftHand.getProjectivePosition().x, leftHand.getProjectivePosition().y);
        Vec2f leftHandNorm = Vec2f( leftHandPos) / Vec2f(640,480);
        Vec2f leftHandVel = Vec2f( leftHandPos - pLeftHand ) / Vec2f(640,480);
        addToFluid( leftHandNorm, leftHandVel, true, true );
        pLeftHand = leftHandPos;
    }
    
    if (rightHand.isFound()) {
        Vec2f rightHandPos = Vec2f(rightHand.getProjectivePosition().x, rightHand.getProjectivePosition().y);
        Vec2f rightHandNorm = Vec2f( rightHandPos) / Vec2f(640,480);
        Vec2f rightHandVel = Vec2f( rightHandPos - pRightHand ) / Vec2f(640,480);
        addToFluid( rightHandNorm, rightHandVel, true, true );
        pRightHand = rightHandPos;
    }
    
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
    
    
    ofPushMatrix();
    
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        ofTranslate(0, deviceID * 480);
        openNIDevices[deviceID].drawDebug();        
    }

    // Draw user piont clouds isolated by user masks
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
    
    	
	// we could draw the whole contour finder
    
	//contourFinder.draw(0, 500);	
    
    if( drawFluid ) {
		glColor3f(1, 1, 1);
		fluidDrawer.draw(640, 500, 640, 480);//(0, 0, getWindowWidth(), getWindowHeight());
	} 
    else {
		if(getElapsedFrames()%5==0) fadeToColor( 0, 0, 0, 0.1f );
	}
	if( drawParticles )
		particleSystem.updateAndDraw( drawFluid );
    
    // draw contours
    ofFill();
    ofEnableSmoothing();
    ofSetHexColor(0xFFFFFF);
    
    int x = (ofGetWindowWidth() - 640) / 2;
    int y = ofGetWindowHeight() - 480;
    for (int i = 0; i < contourFinder.blobs.size(); i++){
        ofBeginShape();
        for (int j = 0; j < contourFinder.blobs[i].nPts; j+=2){
            ofVertex(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
        }
        ofEndShape(true);
    }
    
    /*
     // Write debug info to screen
     ofSetHexColor(0xffffff);
     string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate()) + " BPP: " + ofToString(userMaskPixels.getImageType());
     verdana.drawString(msg, 20, numDevices * 480 + 26);
     */
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {
	//particleSystem.setWindowSize( Vec2f( w, h ) );
}

//--------------------------------------------------------------
void testApp::fadeToColor(float r, float g, float b, float speed) {
    glColor4f(r, g, b, speed);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
}

// add force and dye to fluid, and create particles
void testApp::addToFluid( Vec2f pos, Vec2f vel, bool addColor, bool addForce ) {
    float speed = vel.x * vel.x  + vel.y * vel.y * getWindowAspectRatio() * getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
    if(speed > 0) {
		pos.x = constrain(pos.x, 0.0f, 1.0f);
		pos.y = constrain(pos.y, 0.0f, 1.0f);
		
        const float colorMult = 100;
        const float velocityMult = 30;
		
        int index = fluidSolver.getIndexForPos(pos);
		
		if(addColor) {
			Color drawColor( CM_HSV, ( getElapsedFrames() % 360 ) / 360.0f, 1, 1 );
            //			Color drawColor;
            //			drawColor.setHSV(( getElapsedFrames() % 360 ) / 360.0f, 1, 1 );
			
			fluidSolver.addColorAtIndex(index, drawColor * colorMult);
			
			if( drawParticles )
				particleSystem.addParticles( pos * Vec2f( getWindowSize() ), 10 );
		}
		
		if(addForce)
			fluidSolver.addForceAtIndex(index, vel * velocityMult);
		
    }
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
    int cloudRes = -1;
    switch (key) {
        case '1':
            cloudRes = 1;
            break;
        case '2':
            cloudRes = 2;
            break;
        case '3':
            cloudRes = 3;
            break;
        case '4':
            cloudRes = 4;
            break;
        case '5':
            cloudRes = 5;
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        case 'x':
            for (int deviceID = 0; deviceID < numDevices; deviceID++){
                openNIDevices[deviceID].stop();
            }
            break;
        case 'i':
            for (int deviceID = 0; deviceID < numDevices; deviceID++){
                if (openNIDevices[deviceID].isImageOn()){
                    openNIDevices[deviceID].removeImageGenerator();
                    openNIDevices[deviceID].addInfraGenerator();
                    continue;
                }
                if (openNIDevices[deviceID].isInfraOn()){
                    openNIDevices[deviceID].removeInfraGenerator();
                    openNIDevices[deviceID].addImageGenerator();
                    continue;
                }
            }
            break;
        case 'b':
            for (int deviceID = 0; deviceID < numDevices; deviceID++){
                openNIDevices[deviceID].setUseBackBuffer(!openNIDevices[deviceID].getUseBackBuffer());
            }
            break;
        default:
            break;
    }
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
		openNIDevices[deviceID].setPointCloudResolutionAllUsers(cloudRes);
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	Vec2f eventPos = Vec2f(x, y);
	Vec2f mouseNorm = Vec2f( eventPos) / getWindowSize();
	Vec2f mouseVel = Vec2f( eventPos - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, true, true );
	pMouse = eventPos;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	Vec2f eventPos = Vec2f(x, y);
	Vec2f mouseNorm = Vec2f( eventPos ) / getWindowSize();
	Vec2f mouseVel = Vec2f( eventPos - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, false, true );
	pMouse = eventPos;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}


