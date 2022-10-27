/*
 *  ofxLeapMotion2.cpp
 *  ofxLeapMotion2
 *
 *  Created by gene on 5/29/14.
 *  based on ofxLeapMotion by theo
 *
 */

#include "ofxLeapMotion2.h"
#include "ofxLeapMotionSimpleHand.h"

#include "ofLog.h"

FingerType ofxLeapMotion::fingerTypes[5] = { THUMB, INDEX, MIDDLE, RING, PINKY };

//--------------------------------------------------------------
ofxLeapMotion::ofxLeapMotion()
{
    reset();
    resetMapping();
#ifdef TARGET_LINUX
   string count = ofSystem("ps -A | grep -cw leapd");
   if(stoi(count,nullptr) == 0) {
    ofLogNotice() << "Leap daemon not running, run 'sudo leapd' to start it.";
   }
#endif
    ourController = new Leap::Controller();
}

//--------------------------------------------------------------
ofxLeapMotion::~ofxLeapMotion()
{
    //note we don't delete the controller as it causes a crash / mutex exception.
    /// close(); /// JRW - we do not need this...
    /// JRW - seems fine in this demo but, when I add
    /// JRW - threaded objects the Leap controller crashes on exit.
}

//--------------------------------------------------------------
void ofxLeapMotion::reset()
{
    currentFrameID = 0;
    preFrameId = -1;
}

//--------------------------------------------------------------
void ofxLeapMotion::close()
{
    if (ourController) {
        ourController->removeListener(*this);
    }

    /// JRW - let's delete our Leap controller
    /// call close() on app exit
    delete ourController;
}

//--------------------------------------------------------------
void ofxLeapMotion::open()
{
    reset();
    ourController->addListener(*this);
}

//--------------------------------------------------------------
void ofxLeapMotion::setupGestures()
{
    // enables screen tap gesture (forward poke / tap)
    ourController->enableGesture(Gesture::TYPE_SCREEN_TAP);

    // enables key tap gesture (down tap)
    ourController->enableGesture(Gesture::TYPE_KEY_TAP);

    // enables swipe gesture
    ourController->enableGesture(Gesture::TYPE_SWIPE);
    ourController->config().setFloat("Gesture.Swipe.MinLength", 150.0);
    ourController->config().setFloat("Gesture.Swipe.MinVelocity", 600);
    ourController->config().save();

    // enables circle gesture
    ourController->enableGesture(Gesture::TYPE_CIRCLE);
}

//--------------------------------------------------------------
void ofxLeapMotion::updateGestures()
{
    Leap::Frame frame = ourController->frame();
    if (lastFrame == frame) {
        return;
    }

    Leap::GestureList gestures = frame.gestures();

    gestureType = INVALID;
    lastFrame = frame;

    int numGestures = gestures.count();

    for (int i = 0; i < numGestures; i++) {

        // screen tap gesture (forward poke / tap)
        if (gestures[i].type() == Leap::Gesture::TYPE_SCREEN_TAP) {
            Leap::ScreenTapGesture tap = gestures[i];
            ofVec3f tapLoc = getMappedofPoint(tap.position());
            gestureType = SCREEN_TAP;
        }

        // key tap gesture (down tap)
        else if (gestures[i].type() == Leap::Gesture::TYPE_KEY_TAP) {
            Leap::KeyTapGesture tap = gestures[i];
            gestureType = KEY_TAP;
        }

        // swipe gesture
        else if (gestures[i].type() == Leap::Gesture::TYPE_SWIPE) {
            Leap::SwipeGesture swipe = gestures[i];
            Leap::Vector diff = 0.04f * (swipe.position() - swipe.startPosition());
            ofVec3f curSwipe(diff.x, -diff.y, diff.z);

            // swipe right
            if (curSwipe.x > 3 && curSwipe.x < 20) {
                gestureType = SWIPE_RIGHT;
            }
            // swipe left
            else if (curSwipe.x < -3 && curSwipe.x > -20) {
                gestureType = SWIPE_LEFT;
            }
            // swipe down
            if (curSwipe.y > 3 && curSwipe.y < 20) {
                gestureType = SWIPE_DOWN;
            }
            // swipe up
            else if (curSwipe.y < -3 && curSwipe.y > -20) {
                gestureType = SWIPE_UP;
            }
            // swipe forward
            if (curSwipe.z < -5) {
                gestureType = SWIPE_FORWARD;
            }
            // swipe back
            else if (curSwipe.z > 5) {
                gestureType = SWIPE_BACK;
            }
        }

        // circle gesture
        else if (gestures[i].type() == Leap::Gesture::TYPE_CIRCLE) {
            Leap::CircleGesture circle = gestures[i];
            float progress = circle.progress();

            if (progress >= 1.0f) {
                ofVec3f center = getMappedofPoint(circle.center());
                ofVec3f normal(circle.normal().x, circle.normal().y, circle.normal().z);
                double curAngle = 6.5;
                if (normal.z < 0) {
                    curAngle *= -1;
                }

                if (curAngle < 0) {
                    // clockwise rotation
                    gestureType = CIRCLE_CLOCKWISE;
                } else {
                    // counter-clockwise rotation
                    gestureType = CIRCLE_ANTICLOCKWISE;
                }
            }
        }

        switch (gestures[i].state()) {
        case Leap::Gesture::STATE_START: {
            ofLogVerbose("ofxLeapMotion2") << "Gesture state: START";
            GestureEventArgs gestureEventArgs;
            gestureEventArgs.type = gestureType;
            gestureEventArgs.state = Leap::Gesture::STATE_START;
            ofNotifyEvent(gestureEvent, gestureEventArgs);
            break;
        }
        case Leap::Gesture::STATE_UPDATE: {
            ofLogVerbose("ofxLeapMotion2") << "Gesture state: UPDATE";
            GestureEventArgs gestureEventArgs;
            gestureEventArgs.type = gestureType;
            gestureEventArgs.state = Leap::Gesture::STATE_UPDATE;
            ofNotifyEvent(gestureEvent, gestureEventArgs);
            break;
        }
        case Leap::Gesture::STATE_STOP: {
            ofLogVerbose("ofxLeapMotion2") << "Gesture state: STOP";
            GestureEventArgs gestureEventArgs;
            gestureEventArgs.type = gestureType;
            gestureEventArgs.state = Leap::Gesture::STATE_STOP;
            ofNotifyEvent(gestureEvent, gestureEventArgs);
            break;
        }
        default:
            ofLogVerbose("ofxLeapMotion2") << "Gesture state: INVALID";
            break;
        }
    }
}

//--------------------------------------------------------------
void ofxLeapMotion::onInit(const Controller& controller)
{
    ofLogVerbose("ofxLeapMotion - onInit");
}

//--------------------------------------------------------------
void ofxLeapMotion::onConnect(const Controller& contr)
{
    ofLogNotice("ofxLeapMotion - Leap controller connected");
}

//--------------------------------------------------------------
void ofxLeapMotion::onDisconnect(const Controller& contr)
{
    ofLogNotice("ofxLeapMotion - Leap controller disconnected");
}

//--------------------------------------------------------------
void ofxLeapMotion::onExit(const Controller& contr)
{
    ofLogVerbose("ofxLeapMotion - onExit");
}

//if you want to use the Leap Controller directly - inhereit ofxLeapMotion and implement this function
//note: this function is called in a seperate thread - so GL commands here will cause the app to crash.
//--------------------------------------------------------------
void ofxLeapMotion::onFrame(const Controller& contr)
{
    //ofLogVerbose("ofxLeapMotionApp - onFrame");

    onFrameInternal(contr); // call this if you want to use getHands() / isFrameNew() etc
}

//Simple access to the hands
//--------------------------------------------------------------
std::vector<Hand> ofxLeapMotion::getLeapHands()
{

    std::vector<Hand> handsCopy;
    if (ourMutex.try_lock()) {
        handsCopy = hands;
        ourMutex.unlock();
    }

    return handsCopy;
}

//--------------------------------------------------------------
std::vector<ofxLeapMotionSimpleHand> ofxLeapMotion::getSimpleHands()
{

    std::vector<ofxLeapMotionSimpleHand> simpleHands;
    std::vector<Hand> leapHands = getLeapHands();

    for (int i = 0; i < leapHands.size(); i++) {
        ofxLeapMotionSimpleHand curHand;

        curHand.isLeft = leapHands[i].isLeft();

        curHand.handPos = getMappedofPoint(leapHands[i].palmPosition());
        curHand.handNormal = getofPoint(leapHands[i].palmNormal());
        curHand.direction = getofPoint(leapHands[i].direction());
        curHand.wristPos = getMappedofPoint(leapHands[i].wristPosition());

        curHand.yaw = leapHands[i].direction().yaw();
        curHand.roll = leapHands[i].palmNormal().roll();
        curHand.pitch = leapHands[i].direction().pitch();

        FingerType fingerTypes[] = { THUMB, INDEX, MIDDLE, RING, PINKY };
        for (int j = 0; j < 5; j++) {
            const Finger& finger = leapHands[i].fingers()[fingerTypes[j]];
            ofxLeapMotionSimpleHand::simpleFinger f;
            f.id = finger.id();

            f.pos = getMappedofPoint(finger.tipPosition());
            f.vel = getMappedofPoint(finger.tipVelocity());

            f.dip = getMappedofPoint(finger.jointPosition(finger.JOINT_DIP));
            f.mcp = getMappedofPoint(finger.jointPosition(finger.JOINT_MCP));
            f.pip = getMappedofPoint(finger.jointPosition(finger.JOINT_PIP));
            f.tip = getMappedofPoint(finger.jointPosition(finger.JOINT_TIP));

            curHand.fingers[fingerTypes[j]] = f;
        }
        simpleHands.push_back(curHand);
    }

    return simpleHands;
}

//--------------------------------------------------------------
bool ofxLeapMotion::isConnected()
{
    return (ourController && ourController->isConnected());
}

//--------------------------------------------------------------
void ofxLeapMotion::setReceiveBackgroundFrames(bool bReceiveBg)
{
    if (ourController) {
        ourController->setPolicyFlags(bReceiveBg ? Leap::Controller::POLICY_BACKGROUND_FRAMES : Leap::Controller::POLICY_DEFAULT);
    }
}

//--------------------------------------------------------------
bool ofxLeapMotion::isFrameNew()
{
    return currentFrameID != preFrameId;
}

//--------------------------------------------------------------
void ofxLeapMotion::markFrameAsOld()
{
    preFrameId = currentFrameID;
}

//--------------------------------------------------------------
int64_t ofxLeapMotion::getCurrentFrameID()
{
    return currentFrameID;
}

//--------------------------------------------------------------
void ofxLeapMotion::resetMapping()
{
    xOffsetIn = 0;
    yOffsetIn = 0;
    zOffsetIn = 0;

    xOffsetOut = 0;
    yOffsetOut = 0;
    zOffsetOut = 0;

    xScale = 1;
    yScale = 1;
    zScale = 1;
}

//--------------------------------------------------------------
void ofxLeapMotion::setMappingX(float minX, float maxX, float outputMinX, float outputMaxX)
{
    xOffsetIn = minX;
    xOffsetOut = outputMinX;
    xScale = (outputMaxX - outputMinX) / (maxX - minX);
}

//--------------------------------------------------------------
void ofxLeapMotion::setMappingY(float minY, float maxY, float outputMinY, float outputMaxY)
{
    yOffsetIn = minY;
    yOffsetOut = outputMinY;
    yScale = (outputMaxY - outputMinY) / (maxY - minY);
}

//--------------------------------------------------------------
void ofxLeapMotion::setMappingZ(float minZ, float maxZ, float outputMinZ, float outputMaxZ)
{
    zOffsetIn = minZ;
    zOffsetOut = outputMinZ;
    zScale = (outputMaxZ - outputMinZ) / (maxZ - minZ);
}

//helper function for converting a Leap::Vector to an ofPoint with a mapping
//--------------------------------------------------------------
ofPoint ofxLeapMotion::getMappedofPoint(Vector v)
{
    ofPoint p = getofPoint(v);
    p.x = xOffsetOut + (p.x - xOffsetIn) * xScale;
    p.y = yOffsetOut + (p.y - yOffsetIn) * yScale;
    p.z = zOffsetOut + (p.z - zOffsetIn) * zScale;

    return p;
}

//helper function for converting a Leap::Vector to an ofPoint
//--------------------------------------------------------------
ofPoint ofxLeapMotion::getofPoint(Vector v)
{
    return ofPoint(v.x, v.y, v.z);
}

//--------------------------------------------------------------
void ofxLeapMotion::onFrameInternal(const Controller& contr)
{
    ourMutex.lock();

    const Frame& curFrame = contr.frame();
    const HandList& handList = curFrame.hands();

    hands.clear();
    for (int i = 0; i < handList.count(); i++) {
        hands.push_back(handList[i]);
    }

    currentFrameID = curFrame.id();

    ourMutex.unlock();
}
