//
//  directory.h
//  directory image helper
//
//  Created by default on 6/27/12.
//  Copyright (c) 2012 James Donovan. All rights reserved.
//

#pragma once
#include "ofMain.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/Net/FTPStreamFactory.h"
#include "Poco/Net/FTPClientSession.h"
#include "Poco/File.h"
#include "Poco/Path.h"

#include <fstream>
#include <ios>
#include <iostream>

using namespace std;

#define MAX_IMAGES 200

// Create a vector object of images

class directory : public ofDirectory{
public:
    void loadImg( string path );
    void loadFTP( string remote, string local, bool is_binary );
    vector<ofImage>images;
    
protected:
    ofDirectory dir;
    Poco::Net::FTPClientSession* ftp;
    
    
};