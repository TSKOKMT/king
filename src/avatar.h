#pragma once

#include "ofMain.h"

#include "ofxOsc.h"
#include "ofxSyphon.h"

class avatar {

public:
    ofxOscSender sender;
    bool bConnected = false;
    float lastSignalReceivedTime = 0;
    ofxSyphonClient client;
};
