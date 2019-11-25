#pragma once

#include "ofMain.h"
#include "Leap.h"

using namespace Leap;

enum FingerType { THUMB, INDEX, MIDDLE, RING, PINKY };

class ofxLeapMotionSimpleHand {
public:

    typedef struct{
        ofPoint pos;
        ofPoint vel;
        ofPoint dip, mcp, pip, tip;
        int64_t id;
    } simpleFinger;
    

    void debugDraw();

    map<FingerType, simpleFinger> fingers;
    
    ofPoint handPos;
    ofPoint handNormal;
    ofPoint direction;
    ofPoint wristPos;
    float pitch, yaw, roll;
    bool isLeft;
    
};

