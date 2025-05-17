//
//  PFInput.h
//  PlatformDemo
//
//  This input controller is primarily designed for keyboard control.  On mobile
//  you will notice that we use gestures to emulate keyboard commands. They even
//  use the same variables (though we need other variables for internal keyboard
//  emulation).  This simplifies our design quite a bit.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author:  Walker White and Anthony Perello
//  Version: 2/9/21
//
#include "SSBInput.h"

using namespace cugl;

#pragma mark Input Constants

/** The key to use for reseting the game */
#define RESET_KEY KeyCode::R
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::D
/** The key for exitting the game */
#define EXIT_KEY  KeyCode::ESCAPE
/** The key for firing a bullet */
#define FIRE_KEY KeyCode::SPACE
/** The key for jumping up */
#define JUMP_KEY KeyCode::ARROW_UP

/** How close we need to be for a multi touch */
#define NEAR_TOUCH      100
/** The key for the event handlers */
#define LISTENER_KEY      1

/** This defines the joystick "deadzone" (how far we must move) */
#define JSTICK_DEADZONE  40
/** This defines the joystick radial size (for reseting the anchor) */
#define JSTICK_RADIUS    120
/** How far to display the virtual joystick above the finger */
#define JSTICK_OFFSET    80
/** How far we must swipe up for a jump gesture */
#define SWIPE_LENGTH    50
/** How fast a double click must be in milliseconds */
#define DOUBLE_CLICK    400

// The screen is divided into four zones: Left, Bottom, Right and Main/
// These are all shown in the diagram below.
//
//   |---------------|
//   |   |       |   |
//   | L |   M   | R |
//   |   |       |   |
//   |---------------|
//
// The meaning of any touch depends on the zone it begins in.

/** The portion of the screen used for the left zone */
#define LEFT_ZONE       0.35f
/** The portion of the screen used for the right zone */
#define RIGHT_ZONE      0.35f

#pragma mark -
#pragma mark Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
PlatformInput::PlatformInput() :
_active(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_firePressed(false),
_jumpPressed(false),
_keyJump(false),
_keyFire(false),
_keyReset(false),
_keyDebug(false),
_keyExit(false),
_keyLeft(false),
_keyRight(false),
_horizontal(0.0f),
_joystick(false),
_hasJumped(false),
_rightTapped(false),
_currDown(false),
_prevDown(false),
_prev2Down(false),
_mouseKey(0),
_mouseDown(false),
_holdRight(false),
_canGlide(false){
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void PlatformInput::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();
        Mouse* mouse = Input::get<Mouse>();
        if (mouse) {
            mouse->removePressListener(_mouseKey);
            mouse->removeReleaseListener(_mouseKey);
            mouse->removeDragListener(_mouseKey);
            mouse->setPointerAwareness(Mouse::PointerAwareness::BUTTON);
        }
#else
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
        touch->removeMotionListener(LISTENER_KEY);
#endif
        _active = false;
    }
}

/**
 * Initializes the input control for the given bounds
 *
 * The bounds are the bounds of the scene graph.  This is necessary because
 * the bounds of the scene graph do not match the bounds of the display.
 * This allows the input device to do the proper conversion for us.
 *
 * @param bounds    the scene graph bounds
 *
 * @return true if the controller was initialized successfully
 */
bool PlatformInput::init(const Rect bounds) {
    bool success = true;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    
    originalPosition = Vec2(0, 0);
    createZones();
    clearTouchInstance(_ltouch);
    clearTouchInstance(_rtouch);
    clearTouchInstance(_mtouch);
    
    _inventoryStatus = WAITING;
    
#ifndef CU_TOUCH_SCREEN
    success = Input::activate<Keyboard>();
    Mouse* mouse = Input::get<Mouse>();
    if (mouse) {
        mouse->setPointerAwareness(Mouse::PointerAwareness::DRAG);
        _mouseKey = mouse->acquireKey();
        mouse->addPressListener(_mouseKey, [=, this](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->buttonDownCB(event, clicks, focus);
            _mouseDown = true;
            _mousePosForDrag = mouse2Screen(event.position);
            });
        mouse->addReleaseListener(_mouseKey, [=, this](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->buttonUpCB(event, clicks, focus);
            _mouseDown = false;
            });
        mouse->addDragListener(_mouseKey, [=, this](const cugl::MouseEvent& event, const Vec2 previous, bool focus) {
            this->motionCB(event, previous, focus);
            _mouseDown = true;
            _mousePosForDrag = mouse2Screen(event.position);
            });
        _active = true;
    }
#else
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);

        _touchDown = true;
        _touchPosForDrag = touch2Screen(event.position);
    });
    touch->addEndListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);

        _touchDown = false;
    });
    touch->addMotionListener(LISTENER_KEY,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
        
        _touchDown = true;
        _touchPosForDrag = touch2Screen(event.position);
    });
	
#endif
    _active = success;
    return success;
}

/**
 * Processes the currently cached inputs.
 *
 * This method is used to to poll the current input state.  This will poll the
 * keyboad and accelerometer.
 *
 * This method also gathers the delta difference in the touches. Depending on
 * the OS, we may see multiple updates of the same touch in a single animation
 * frame, so we need to accumulate all of the data together.
 */
void PlatformInput::update(float dt) {
#ifndef CU_TOUCH_SCREEN
    // DESKTOP CONTROLS
    Keyboard* keys = Input::get<Keyboard>();

    // Map "keyboard" events to the current frame boundary
    _keyReset  = keys->keyPressed(RESET_KEY);
    _keyDebug  = keys->keyPressed(DEBUG_KEY);
    _keyExit   = keys->keyPressed(EXIT_KEY);
    _keyFire   = keys->keyPressed(FIRE_KEY);
    _keyJump   = keys->keyPressed(JUMP_KEY);

    _keyLeft = keys->keyDown(KeyCode::ARROW_LEFT);
    _keyRight = keys->keyDown(KeyCode::ARROW_RIGHT);
#endif

    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed  = _keyExit;
	_firePressed  = _keyFire;
	_jumpPressed  = _keyJump;
    _prev2Down = _prevDown;
    _prevDown = _currDown;

	// Directional controls
	_horizontal = 0.0f;
	if (_keyRight) {
		_horizontal += 1.0f;
	}
	if (_keyLeft) {
		_horizontal -= 1.0f;
	}

// If it does not support keyboard, we must reset "virtual" keyboard
#ifdef CU_TOUCH_SCREEN
    _keyExit = false;
    _keyReset = false;
    _keyDebug = false;
    _keyJump  = false;
    _keyFire  = false;
#endif
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void PlatformInput::clear() {
    _resetPressed = false;
    _debugPressed = false;
    _exitPressed  = false;
    _jumpPressed = false;
    _firePressed = false;
    _currDown = false;
    
}

#pragma mark -
#pragma mark Touch Controls

/**
 * Defines the zone boundaries, so we can quickly categorize touches.
 */
void PlatformInput::createZones() {
	_lzone = _tbounds;
	_lzone.size.width *= LEFT_ZONE;
	_rzone = _tbounds;
	_rzone.size.width *= RIGHT_ZONE;
	_rzone.origin.x = _tbounds.origin.x+_tbounds.size.width-_rzone.size.width;
}

/**
 * Populates the initial values of the input TouchInstance
 */
void PlatformInput::clearTouchInstance(TouchInstance& touchInstance) {
    touchInstance.touchids.clear();
    touchInstance.position = Vec2::ZERO;
}


/**
 * Returns the correct zone for the given position.
 *
 * See the comments above for a description of how zones work.
 *
 * @param  pos  a position in screen coordinates
 *
 * @return the correct zone for the given position.
 */
PlatformInput::Zone PlatformInput::getZone(const Vec2 pos) const {
	if (_lzone.contains(pos)) {
		return Zone::LEFT;
	} else if (_rzone.contains(pos)) {
		return Zone::RIGHT;
	} else if (_tbounds.contains(pos)) {
		return Zone::MAIN;
	}
	return Zone::UNDEFINED;
}

/**
 * Returns the scene location of a touch
 *
 * Touch coordinates are inverted, with y origin in the top-left
 * corner. This method corrects for this and scales the screen
 * coordinates down on to the scene graph size.
 *
 * @return the scene location of a touch
 */
Vec2 PlatformInput::touch2Screen(const Vec2 pos) const {
    float px = pos.x/_tbounds.size.width -_tbounds.origin.x;
    float py = pos.y/_tbounds.size.height-_tbounds.origin.y;
    Vec2 result;
    result.x = px*_sbounds.size.width +_sbounds.origin.x;
    result.y = (1-py)*_sbounds.size.height+_sbounds.origin.y;
    return result;
}

/**
 * Returns the scene location of a mouse click. This is identical to touch2Screen, but named again for programming clarity.
 *
 * Mouse coordinates are inverted, with y origin in the top-left
 * corner. This method corrects for this and scales the screen
 * coordinates down on to the scene graph size.
 *
 * @return the scene location of a mouse click.
 */
Vec2 PlatformInput::mouse2Screen(const Vec2 pos) const {
    float px = pos.x / _tbounds.size.width - _tbounds.origin.x;
    float py = pos.y / _tbounds.size.height - _tbounds.origin.y;
    Vec2 result;
    result.x = px * _sbounds.size.width + _sbounds.origin.x;
    result.y = (1 - py) * _sbounds.size.height + _sbounds.origin.y;
    return result;
}
/**
 * Processes movement for the floating joystick.
 *
 * This will register movement as left or right (or neither).  It
 * will also move the joystick anchor if the touch position moves
 * too far.
 *
 * @param  pos  the current joystick position
 */
void PlatformInput::processJoystick(const cugl::Vec2 pos) {
    Vec2 diff =  _ltouch.position-pos;

    // Reset the anchor if we drifted too far
    if (diff.lengthSquared() > JSTICK_RADIUS*JSTICK_RADIUS) {
        diff.normalize();
        diff *= (JSTICK_RADIUS+JSTICK_DEADZONE)/2;
        _ltouch.position = pos+diff;
    }
    _ltouch.position.y = pos.y;
    _joycenter = touch2Screen(_ltouch.position);
    _joycenter.y += JSTICK_OFFSET;
    
    if (std::fabsf(diff.x) > JSTICK_DEADZONE) {
        _joystick = true;
        if (diff.x > 0) {
            _keyLeft = true;
            _keyRight = false;
        } else {
            _keyLeft = false;
            _keyRight = true;
        }
        if (std::fabsf(diff.x) > JSTICK_DEADZONE +30) {
            //CULog("BEEGSWIPE");
        }
    } else {
        _joystick = false;
        _keyLeft = false;
        _keyRight = false;
    }
}

/**
 * Returns a nonzero value if this is a quick left or right swipe
 *
 * The function returns -1 if it is left swipe and 1 if it is a right swipe.
 *
 * @param  start    the start position of the candidate swipe
 * @param  stop     the end position of the candidate swipe
 * @param  current  the current timestamp of the gesture
 *
 * @return a nonzero value if this is a quick left or right swipe
 */
int PlatformInput::processSwipe(const Vec2 start, const Vec2 stop, Timestamp current) {
	// Look for swipes up that are "long enough"
	float xdiff = (stop.x-start.x);
    float thresh = SWIPE_LENGTH;
    if (xdiff > thresh) {
        return 1;
    } else if (xdiff < thresh * -1) {
        return -1;
    }
	return 0;
}



#pragma mark -
#pragma mark Touch and Mouse Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param event The associated event
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchBeganCB(const TouchEvent& event, bool focus) {

    //CULog("Touch began %lld", event.touch);
    Vec2 pos = event.position;
    Zone zone = getZone(pos);
    if (zone == Zone::MAIN) {
        _currDown = true;
        originalPosition = pos;
    }
    switch (zone) {
        case Zone::LEFT:
            // Only process if no touch in zone
            if (_ltouch.touchids.empty()) {
                // Left is the floating joystick
                _ltouch.position = event.position;
                _ltouch.timestamp.mark();
                _ltouch.touchids.insert(event.touch);

                _joystick = true;
                _joycenter = touch2Screen(event.position);
                _joycenter.y += JSTICK_OFFSET;
            }
            break;
        case Zone::RIGHT:
            CULog("TOUCHEDRIGHT");
            // Only process if no touch in zone
            if (_rtouch.touchids.empty()) {
                // Right is jump AND fire controls
                _keyFire = (event.timestamp.ellapsedMillis(_rtime) <= DOUBLE_CLICK);
                _rtouch.position = event.position;
                _rtouch.timestamp.mark();
                _rtouch.touchids.insert(event.touch);
                _hasJumped = false;
                _holdRight = true;
                _rightTapped = true;
            }
            break;
        case Zone::MAIN:
            // Only check for double tap in Main if nothing else down
            if (_ltouch.touchids.empty() && _rtouch.touchids.empty() && _mtouch.touchids.empty()) {
                _keyDebug = (event.timestamp.ellapsedMillis(_mtime) <= DOUBLE_CLICK);
            }
            
            // Keep count of touches in Main zone if next to each other.
            if (_mtouch.touchids.empty()) {
                _mtouch.position = event.position;
                _mtouch.touchids.insert(event.touch);
            } else {
                Vec2 offset = event.position-_mtouch.position;
                if (offset.lengthSquared() < NEAR_TOUCH*NEAR_TOUCH) {
                    _mtouch.touchids.insert(event.touch);
                }
            }
            break;
        default:
            CUAssertLog(false, "Touch is out of bounds");
            break;
    }
}

 
/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchEndedCB(const TouchEvent& event, bool focus) {
    _currDown = false;
    // Reset all keys that might have been set
    Vec2 pos = event.position;
    
    if (_inventoryStatus == PLACING) {
        _inventoryStatus = PLACED;
        _placedPos = touch2Screen(pos);
    }
    
    Zone zone = getZone(pos);
    finalPosition = pos;
    if (_ltouch.touchids.find(event.touch) != _ltouch.touchids.end()) {
        _ltouch.touchids.clear();
        _keyLeft = false;
        _keyRight = false;
        _joystick = false;
    } else if (_rtouch.touchids.find(event.touch) != _rtouch.touchids.end()) {
        _hasJumped = false;
        _holdRight = false;
        _rtime = event.timestamp;
        _rtouch.touchids.clear();
    } else if (zone == Zone::MAIN) {
        if (_mtouch.touchids.find(event.touch) != _mtouch.touchids.end()) {
            _mtouch.touchids.erase(event.touch);
        }
        _mtime = event.timestamp;
    }
    
}


/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    Vec2 pos = event.position;

    // THIS WILL NOT WORK WITH MULTIPLE TOUCHES
    // Just a placeholder for now. TODO: make this compatible with multiple touches in main zone???
    if (getZone(pos) == Zone::MAIN) {
        if ((pos - originalPosition).length() < 500) {
            finalPosition = pos;
        }
        
    }
    // Only check for swipes in the main zone if there is more than one finger.
    if (_ltouch.touchids.find(event.touch) != _ltouch.touchids.end()) {
        processJoystick(pos);
    } else if (_rtouch.touchids.find(event.touch) != _rtouch.touchids.end()) {
        if (!_hasJumped) {
            if ((_rtouch.position.y-pos.y) > SWIPE_LENGTH) {
                _keyJump = true;
                _hasJumped = true;
            }
        }
    } else if (_mtouch.touchids.size() > 1) {
        // We only process multifinger swipes in main
        int swipe = processSwipe(_mtouch.position, event.position, event.timestamp);
        if (swipe == 1) {
            _keyReset = true;
        } else if (swipe == -1) {
            _keyExit = true;
        }
    }
    _placedPos = touch2Screen(event.position);
}

#pragma mark Mouse Callbacks
/**
 * Call back to execute when a mouse button is first pressed.
 *
 * This function will record a press only if the left button is pressed.
 *
 * @param event     The event with the mouse information
 * @param clicks    The number of clicks (for double clicking)
 * @param focus     Whether this device has focus (UNUSED)
 */
void PlatformInput::buttonDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    // Only recognize the left mouse button
    if (!_mouseDown && event.buttons.hasLeft()) {
    }
}

/**
 * Call back to execute when a mouse button is first released.
 *
 * This function will record a release for the left mouse button.
 *
 * @param event     The event with the mouse information
 * @param clicks    The number of clicks (for double clicking)
 * @param focus     Whether this device has focus (UNUSED)
 */
void PlatformInput::buttonUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    // Only recognize the left mouse button
    if (_mouseDown && event.buttons.hasLeft()) {
        if (_inventoryStatus == PLACING) {
            _inventoryStatus = PLACED;
            _placedPos = mouse2Screen(event.position);
        }
    }
}

/**
 * Call back to execute when the mouse moves.
 *
 * This input controller sets the pointer awareness only to monitor a mouse
 * when it is dragged (moved with button down), not when it is moved. This
 * cuts down on spurious inputs. In addition, this method only pays attention
 * to drags initiated with the left mouse button.
 *
 * @param event     The event with the mouse information
 * @param previous  The previously reported mouse location
 * @param focus     Whether this device has focus (UNUSED)
 */
void PlatformInput::motionCB(const cugl::MouseEvent& event, const Vec2 previous, bool focus) {
    if (_mouseDown) {
        _placedPos = mouse2Screen(event.position);
    }
}



