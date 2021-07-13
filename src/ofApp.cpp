#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetWindowPosition(ofGetScreenWidth() / 18., ofGetScreenHeight() / 18.);
    ofSetWindowShape(ofGetScreenWidth() / 18. * 16, ofGetScreenHeight() / 18. * 16);
    ofBackground(0);
    
    //setup osc
    receiver.setup(50000);
    
    //setup frame
    xml.load("/Users/tskokmt/Documents/PROJECT/FRAME FREE/DATA/frameData.xml");
    frameL = ofRectangle(0, 0, xml.getValue("frameLW", 1340), xml.getValue("frameLH", 460));
    frameC = ofRectangle(frameL.width, 0, xml.getValue("frameCW", 1140), xml.getValue("frameCH", 460));
    frameR = ofRectangle(frameL.width + frameC.width, 0, xml.getValue("frameRW", 1340), xml.getValue("frameRH", 460));
    frame = ofRectangle(0, 0, frameC.width + frameL.width + frameR.width, frameC.height);
    
    //fbo
    fboC.allocate(frameC.width, frameC.height);
    fboL.allocate(frameL.width, frameL.height);
    fboR.allocate(frameR.width, frameR.height);
    fboLR.allocate(frameL.width + frameR.width, frameL.height);
    
    //setup syphon
    poolClient.setup();
    poolClient.set("poolServer", "pool");
    serverC.setName("kingServerC");
    serverLR.setName("kingServerLR");

    //load windowRect
    tool.loadWindowRect();
}

//--------------------------------------------------------------
void ofApp::update(){

    ofSetWindowTitle("king | " + ofToString(round(ofGetFrameRate())) + " | " + ofToString(scale));
    
    //update time
    previousTime = time;
    time = ofGetElapsedTimef();
    
    //send signal
    if (fmod(time, signalTerm) < fmod(previousTime, signalTerm)) {
        for (int i = 0; i < avatars.size(); i++) {
            message.clear();
            message.setAddress("signalK");
            message.addBoolArg(i == index);
            avatars[i].sender.sendMessage(message);
        }
    }
    
    //receive
    while (receiver.hasWaitingMessages()) {
        message.clear();
        receiver.getNextMessage(message);
        
        //receive connect
        if (message.getAddress() == "connect") {
            string name = message.getArgAsString(0);
            int port = message.getArgAsInt(1);
            bool bNew = true;
            for (int i = 0; i < avatars.size(); i++) {
                if (avatars[i].sender.getPort() == port) bNew = false;
            }
            if (bNew) {
                avatars.push_back(avatar());
                avatars.back().sender.setup("localhost", port);
                avatars.back().client.setup();
                avatars.back().client.set(name + "Server", name);
            }
        }
        
        //receive disconnect
        if (message.getAddress() == "disconnect") {
            int port = message.getArgAsInt(0);
            int i = 0;
            while (i < avatars.size()) {
                if (avatars[i].sender.getPort() == port) {
                    avatars.erase(avatars.begin() + i);
                    if (i == index) index = -1;
                    if (i < index) index--;
                }
                else i++;
            }
        }
        
        //receive signal
        if (message.getAddress() == "signal") {
            int port = message.getArgAsInt(0);
            for (int i = 0; i < avatars.size(); i++) {
                if (avatars[i].sender.getPort() == port) {
                    avatars[i].bConnected = true;
                    avatars[i].lastSignalReceivedTime = time;
                }
            }
        }
    }
    
    //update bConnecteds
    for (int i = 0; i < avatars.size(); i++) {
        if (time - avatars[i].lastSignalReceivedTime > signalTerm * 4) avatars[i].bConnected = false;
    }
    
    //update fbo
    fboC.begin();
    ofClear(0);
    ofPushMatrix();
    translator.reset();
    translator.disFit(ofRectangle(0, 0, fboC.getWidth(), fboC.getHeight()), frameC);
    ofSetColor(255);
    if (index == -1) poolClient.draw(0, 0);
    else avatars[index].client.draw(0, 0);
    ofPopMatrix();
    fboC.end();
    
    fboL.begin();
    ofClear(0);
    ofPushMatrix();
    translator.reset();
    translator.disFit(ofRectangle(0, 0, fboL.getWidth(), fboL.getHeight()), frameL);
    ofSetColor(255);
    if (index == -1) poolClient.draw(0, 0);
    else avatars[index].client.draw(0, 0);
    ofPopMatrix();
    fboL.end();
    
    fboR.begin();
    ofClear(0);
    ofPushMatrix();
    translator.reset();
    translator.disFit(ofRectangle(0, 0, fboR.getWidth(), fboR.getHeight()), frameR);
    ofSetColor(255);
    if (index == -1) poolClient.draw(0, 0);
    else avatars[index].client.draw(0, 0);
    ofPopMatrix();
    fboR.end();
    
    fboLR.begin();
    ofClear(0);
    ofPushMatrix();
    translator.reset();
    translator.fit(ofRectangle(0, 0, fboL.getWidth() + fboR.getWidth(), fboL.getHeight()), ofRectangle(0, 0, fboLR.getWidth(), fboLR.getHeight()));
    ofSetColor(255);
    fboL.draw(0, 0);
    fboR.draw(fboL.getWidth(), 0);
    ofPopMatrix();
    fboLR.end();
    
    //publish fbo
    serverC.publishTexture(&fboC.getTexture());
    serverLR.publishTexture(&fboLR.getTexture());
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //create frames
    vector<ofRectangle>frames = tool.separatedRectanglesVertical(ofGetWindowRect(), avatars.size() + 1);
    
    for (int i = 0; i < avatars.size() + 1; i++) {
        ofPushMatrix();
        translator.reset();
        translator.smartFit(frame, frames[i]);
        
        //draw clients
        ofSetColor(255);
        if (i == 0) poolClient.draw(0, 0);
        else avatars[i - 1].client.draw(0, 0);
        
        //draw frame
        ofPushStyle();
        ofNoFill();
        ofSetColor(255);
        ofDrawRectangle(frameC);
        ofDrawRectangle(frameL);
        ofDrawRectangle(frameR);
        ofDrawRectangle(frame);
        ofPopStyle();
        
        ofPopMatrix();
        
        //draw name & port
        ofSetColor(255);
        if (i == 0) ofDrawBitmapString("pool", 10, 20);
        else {
            ofDrawBitmapString(avatars[i - 1].client.getApplicationName(), frames[i].getLeft() + 10, frames[i].getTop() + 20);
            ofDrawBitmapString(ofToString(avatars[i - 1].sender.getPort()), frames[i].getLeft() + 10, frames[i].getTop() + 40);
        }

        //draw bConnecteds
        if (i >= 1) {
            float radius = sqrt(ofGetWindowRect().getArea()) / 90.;
            if (avatars[i - 1].bConnected) ofSetColor(0, 255, 0);
            else ofSetColor(255, 0, 0);
            ofDrawCircle(frames[i].getRight() - radius * 5, frames[i].getTop() + radius * 2, radius);
        }
    }
    
    //draw index
    float radius = sqrt(ofGetWindowRect().getArea()) / 90.;
    ofSetColor(255);
    ofDrawCircle(frames[index + 1].getRight() - radius * 2, frames[index + 1].y + radius * 2, radius);
}

//--------------------------------------------------------------
void ofApp::exit(){

    //save windowRect
    tool.saveWindowRect();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    //index
    if (key == ' ') {
        index++;
        if (index >= avatars.size()) index = -1;
    }
    
    //scale
    if (key == OF_KEY_UP) {
        scale += .25;
        scale = ofClamp(scale, .25, 1);
        fboLR.allocate((frameL.width + frameR.width) * scale, frameL.height * scale);
    }
    if (key == OF_KEY_DOWN) {
        scale -= .25;
        scale = ofClamp(scale, .25, 1);
        fboLR.allocate((frameL.width + frameR.width) * scale, frameL.height * scale);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
