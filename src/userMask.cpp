//
//  hydra.cpp
//  JointExample
//
//  Created by default on 8/22/12.
//  Copyright (c) 2012 James Nova. All rights reserved.
//

#include "userMask.h"

ofxUserMask::ofxUserMask() 
{
}

void ofxUserMask::setup()
{
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
}

void ofxUserMask::update(ofPixels mask){
    
    userMaskPixels = mask;
    greyScaleMask.setChannel(0, userMaskPixels.getChannel(0));
    
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


void ofxUserMask::draw(bool contour)
{
    // we could draw the whole contour finder
    if(contour) contourFinder.draw(0, 500);	
    
    // draw contours
    ofPushStyle();
    ofPushMatrix();
    ofTranslate((ofGetWindowWidth() - 640) / 2,(ofGetWindowHeight() - 480));
    
    int x = 0;//(ofGetWindowWidth() - 640) / 2;
    int y = 0;//ofGetWindowHeight() - 480;
    
    ofPath                  path;
    
    ofNoFill();
    ofSetLineWidth(5);
    ofSetHexColor(0x000000);
    
    for (int i = 0; i < contourFinder.blobs.size(); i++){
        ofPolyline              line;

        for (int j = 0; j < contourFinder.blobs[i].nPts; j+=6){

            if(j==0){
                line.addVertex(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
                line.addVertex(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
                path.moveTo(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
                path.curveTo(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
            }
            else if(j==contourFinder.blobs[i].nPts-1){
                line.curveTo(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
                line.curveTo(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
                path.curveTo(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
                path.curveTo(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
            }
            else{
                line.curveTo(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
                path.curveTo(x + contourFinder.blobs[i].pts[j].x, y + contourFinder.blobs[i].pts[j].y);
            }
        }
        line.close();
        //line.simplify(6.66f);
        line.getSmoothed(50,6.6f);
        line.draw();
    }        
     

    

    
        ofFill();
        ofEnableSmoothing();
        ofSetHexColor(0xFFFFFF);
        //path.close();
        path.setPolyWindingMode(OF_POLY_WINDING_POSITIVE);
        path.draw();

        
        ofNoFill();
        ofSetLineWidth(5);
        ofSetHexColor(0x000000);
        vector<ofPolyline> polyLine = path.getOutline();

        for(int l; l < polyLine.size(); l++){
            polyLine[l].draw();
        }
        
    ofPopMatrix();
    ofPopStyle();

}

void ofxUserMask::record(){
    //recording.push_back(line);
}

void ofxUserMask::play(){
    //recording[1].draw();
}



