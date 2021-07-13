#pragma once

#include "ofMain.h"

#include "ofxOsc.h"
#include "ofxSyphon.h"
#include "avatar.h"
#include "ofxTskokmtTool.h"
#include "ofxTskokmtTranslator.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
	
private:
    //time
    float time;
    float previousTime;
    
    //index
    int index = -1;
    
    //scale
    float scale = 1;
    
    //avatar
    vector<avatar> avatars;
    
    //osc
    ofxOscReceiver receiver;
    ofxOscMessage message;
    float signalTerm = .2;
    
    //frame
    ofRectangle frameC;
    ofRectangle frameL;
    ofRectangle frameR;
    ofRectangle frame;
    
    //fbo
    ofFbo fboC;
    ofFbo fboL;
    ofFbo fboR;
    ofFbo fboLR;
    
    //syphon
    ofxSyphonClient poolClient;
    ofxSyphonServer serverC;
    ofxSyphonServer serverLR;
    
    //xml
    ofxXmlSettings xml;
    
    //tool
    ofxTskokmtTool tool;
    
    //translator
    ofxTskokmtTranslator translator;
};
