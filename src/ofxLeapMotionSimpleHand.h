#pragma once

#include <map>

#include "Leap.h"

#include "ofPoint.h"
#include "ofVectorMath.h"

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

    std::map<FingerType, simpleFinger> fingers;
    
    ofPoint handPos;
    ofPoint handNormal;
    ofPoint direction;
    ofPoint wristPos;
    float pitch, yaw, roll;
    bool isLeft;
    
};

