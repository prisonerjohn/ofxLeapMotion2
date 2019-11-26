#pragma once
#include "ofMain.h"
#include "Leap.h"
#include "ofxLeapMotionSimpleHand.h"

using namespace Leap;

enum GestureType {
    INVALID = 0,
    SCREEN_TAP,
    KEY_TAP,
    SWIPE_RIGHT,
    SWIPE_LEFT,
    SWIPE_DOWN,
    SWIPE_UP,
    SWIPE_FORWARD,
    SWIPE_BACK,
    CIRCLE_CLOCKWISE,
    CIRCLE_ANTICLOCKWISE
};

class GestureEventArgs : public ofEventArgs {

public:
    GestureType type;
    Leap::Gesture::State state;
};

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

    static FingerType fingerTypes[5];

    ofEvent<GestureEventArgs> gestureEvent;

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

    GestureType gestureType;
    Leap::Frame lastFrame;

    std::mutex ourMutex;
};
