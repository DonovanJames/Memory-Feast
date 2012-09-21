//
//  userMask.h
//  KinectFluid
//
//  Created by James Donovan on 8/30/12.
//  Copyright 2012 Independant Contractor. All rights reserved.
//
#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"

class ofxUserMask {
	
public:
    // OpenCV calls
    ofxCvColorImage			colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvContourFinder      contourFinder;
    ofPixels                userMaskPixels;
    ofPixels                greyScaleMask;
    ofPixels                bgSet;
    ofTexture               userMaskTexture;
    int                     threshold;
    bool                    bLearnBakground;
    
    vector <ofPolyline>     recording;

    
	ofxUserMask();
    
	void setup();
    void update(ofPixels mask);
    void draw(bool contour = false);
    void record();
    void play();
    void mask();


};