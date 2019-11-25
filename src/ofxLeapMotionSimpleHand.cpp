
#include "ofxLeapMotionSimpleHand.h"

    void ofxLeapMotionSimpleHand::debugDraw()
    {
        ofPushStyle();
        
        ofSetColor(190);
        ofSetLineWidth(2);
        
        ofEnableLighting();
        ofPushMatrix();
        ofTranslate(handPos);
        //rotate the hand by the downwards normal
        ofQuaternion q;
        q.makeRotate(ofPoint(0, -1, 0), handNormal);
        ofMatrix4x4 m;
        q.get(m);
        glMultMatrixf(m.getPtr());
        
        
        //scale it to make it not a box
        ofScale(1, 0.35, 1.0);
#if (OF_VERSION_MAJOR == 0) && (OF_VERSION_MINOR < 8)
        ofBox(0, 0, 0, 60);
#else
        ofDrawBox(0, 0, 0, 60);
#endif
        ofPopMatrix();

        ofDrawArrow(handPos, fingers[THUMB].pos, 10);
        ofDrawArrow(handPos, fingers[INDEX].pos, 10);
        ofDrawArrow(handPos, fingers[MIDDLE].pos, 10);
        ofDrawArrow(handPos, fingers[RING].pos, 10);
        ofDrawArrow(handPos, fingers[PINKY].pos, 10);
        
        ofSetColor(220, 220, 0);
        ofDrawArrow(fingers[THUMB].pos + fingers[THUMB].vel/20, fingers[THUMB].pos + fingers[THUMB].vel/10, 10);
        ofDrawArrow(fingers[INDEX].pos + fingers[INDEX].vel/20, fingers[INDEX].pos + fingers[INDEX].vel/10, 10);
        ofDrawArrow(fingers[MIDDLE].pos + fingers[MIDDLE].vel/20, fingers[MIDDLE].pos + fingers[MIDDLE].vel/10, 10);
        ofDrawArrow(fingers[RING].pos + fingers[RING].vel/20, fingers[RING].pos + fingers[RING].vel/10, 10);
        ofDrawArrow(fingers[PINKY].pos + fingers[PINKY].vel/20, fingers[PINKY].pos + fingers[PINKY].vel/10, 10);
        
        ofDisableLighting();
        
        ofPopStyle();
    }
