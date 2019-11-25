#pragma once

#include "ofMain.h"
#include "Leap.h"
#include "ofxLeapMotionSimpleHand.h"

using namespace Leap;

class ofxLeapMotion : public Listener {
public:

    ofxLeapMotion();
    ~ofxLeapMotion();
    void reset();
    void close();
    void open();
    void setupGestures();
    void updateGestures();
    virtual void onInit(const Controller& controller);
    virtual void onConnect(const Controller& contr);
    virtual void onDisconnect(const Controller& contr);
    virtual void onExit(const Controller& contr);
    virtual void onFrame(const Controller& contr);
    vector<Hand> getLeapHands();
    vector<ofxLeapMotionSimpleHand> getSimpleHands();
    bool isConnected();
    void setReceiveBackgroundFrames(bool bReceiveBg);
    bool isFrameNew();
    void markFrameAsOld();
    int64_t getCurrentFrameID();
    void resetMapping();
    void setMappingX(float minX, float maxX, float outputMinX, float outputMaxX);
    void setMappingY(float minY, float maxY, float outputMinY, float outputMaxY);
    void setMappingZ(float minZ, float maxZ, float outputMinZ, float outputMaxZ);
    ofPoint getMappedofPoint(Vector v);
    ofPoint getofPoint(Vector v);

    // TODO: adding leap gesture support - JRW
    int iGestures;


protected:
    //if you want to use the Leap Controller directly - inhereit ofxLeapMotion and implement this function
    //note: this function is called in a seperate thread - so GL commands here will cause the app to crash.
    //--------------------------------------------------------------
    virtual void onFrameInternal(const Controller& contr);

    int64_t currentFrameID;
    int64_t preFrameId;

    float xOffsetIn, xOffsetOut, xScale;
    float yOffsetIn, yOffsetOut, yScale;
    float zOffsetIn, zOffsetOut, zScale;

    vector<Hand> hands;
    Leap::Controller* ourController;

    // TODO: added for Gesture support - JRW
    Leap::Frame lastFrame;

    std::mutex ourMutex;
};
