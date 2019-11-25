#pragma once

#include "ofMain.h"
#include "Leap.h"


enum fingerType { THUMB, INDEX, MIDDLE, RING, PINKY };


using namespace Leap;

class ofxLeapMotionSimpleHand {
public:

    typedef struct{
        ofPoint pos;
        ofPoint vel;
        ofPoint dip, mcp, pip, tip;
        int64_t id;
    } simpleFinger;
    

    void debugDraw();

    map<fingerType, simpleFinger> fingers;
    
    ofPoint handPos;
    ofPoint handNormal;
    ofPoint direction;
    ofPoint wristPos;
    float pitch, yaw, roll;
    bool isLeft;
    
};

