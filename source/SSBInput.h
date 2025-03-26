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
#ifndef __PF_INPUT_H__
#define __PF_INPUT_H__
#include <cugl/cugl.h>
#include <unordered_set>

/**
 * This class represents player input in the Platform demo.
 *
 * This input handler uses the CUGL input API.  It uses the polling API for
 * keyboard, but the callback API for touch.  This demonstrates a mix of ways
 * to handle input, and the reason for hiding it behind an abstraction like
 * this class.
 *
 * In the case of touch devices, this implements the platformer controls
 * found in the popular game Oddmar.  The left side is a floating joystick
 * for left-right controls.  The right side is swipe up to jump and double
 * tap to shoot.
 *
 * Unlike CUGL input devices, this class is not a singleton.  It must be
 * allocated before use.  However, you will notice that we do not do any
 * input initialization in the constructor.  This allows us to allocate this
 * controller as a field without using pointers. We simply add the class to the
 * header file of its owner, and delay initialization (via the method init())
 * until later. This is one of the main reasons we like to avoid initialization
 * in the constructor.
 */
class PlatformInput {
public:
    /**
     * The status of the inventory.
     */
    enum InventoryStatus {
        /** Waiting for an item to be selected */
        WAITING,
        /** Dragging an item */
        PLACING,
        /** Placed an item in a grid */
        PLACED
    };

private:
    /** Whether or not this input is active */
    bool _active;
    // KEYBOARD EMULATION
    /** Whether the jump key is down */
    bool  _keyJump;
    /** Whether the fire key is down */
    bool  _keyFire;
    /** Whether the reset key is down */
    bool  _keyReset;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the exit key is down */
    bool  _keyExit;
    /** Whether the left arrow key is down */
    bool  _keyLeft;
    /** Whether the right arrow key is down */
    bool  _keyRight;
    /** Whether the touch is currently down */
    bool _currDown;
    /** Whether the touch was down one frame ago */
    bool _prevDown;

    bool _prev2Down;
public: 
    cugl::Vec2 originalPosition;
    cugl::Vec2 finalPosition;
    bool _touchDown;
  
    bool _mouseDown;

#pragma mark glide bullshit
    //Here is the glide bullshit
    /**Whether or not we are holding the right side of the screen*/
    bool _holdRight;
    //This is set to true the moment we tap the right side of the screen. It should be immediately set to false
    //Once we perform a game update
    bool _rightTapped;
    //Set _tapRight. 
    void setRightTapped(bool set) { _rightTapped = set; };
    //get rightTapped
    bool getRightTapped() { return _rightTapped; };

    bool canGlide() { return _canGlide; }
    void setGlide(bool v) { _canGlide = v; }

protected:
    // INPUT RESULTS
    /** Whether the reset action was chosen. */
    bool _resetPressed;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    /** Whether the exit action was chosen. */
    bool _exitPressed;
    /** Whether the fire action was chosen. */
    bool _firePressed;
    /** Whether the jump action was chosen. */
    bool _jumpPressed;
    /** How much did we move horizontally? */
    float _horizontal;
    /** Touch position on screen */
    cugl::Vec2 _touchPosForDrag;
    /** Touch position on screen */
    cugl::Vec2 _mousePosForDrag;
    /** The key for the mouse listeners */
    Uint32 _mouseKey;
    
    // INVENTORY
    /** Whether the player is placing an item in build mode */
    InventoryStatus _inventoryStatus;
    /** The screen position of the placed item */
    cugl::Vec2 _placedPos;


    //GLIDING CONTROLS
/**Whether or not we can glide*/
    bool _canGlide;

#pragma mark Internal Touch Management   
	// The screen is divided into four zones: Left, Bottom, Right and Main/
	// These are all shown in the diagram below.
	//
	//   |---------------|
	//   |   |       |   |
	//   | L |   M   | R |
	//   |   |       |   |
	//   -----------------
	//
	// The meaning of any touch depends on the zone it begins in.

	/** Information representing a single "touch" (possibly multi-finger) */
	struct TouchInstance {
		/** The anchor touch position (on start) */
		cugl::Vec2 position;
        /** The current touch time */
        cugl::Timestamp timestamp;
		/** The touch id(s) for future reference */
        std::unordered_set<Uint64> touchids;
	};

	/** Enumeration identifying a zone for the current touch */
	enum class Zone {
		/** The touch was not inside the screen bounds */
		UNDEFINED,
		/** The touch was in the left zone (as shown above) */
		LEFT,
		/** The touch was in the right zone (as shown above) */
		RIGHT,
		/** The touch was in the main zone (as shown above) */
		MAIN
	};

	/** The bounds of the entire game screen (in touch coordinates) */
    cugl::Rect _tbounds;
    /** The bounds of the entire game screen (in scene coordinates) */
	cugl::Rect _sbounds;
	/** The bounds of the left touch zone */
	cugl::Rect _lzone;
	/** The bounds of the right touch zone */
	cugl::Rect _rzone;

    cugl::Vec2 _touchReleasePos;

	// Each zone can have only one touch
	/** The current touch location for the left zone */
	TouchInstance _ltouch;
	/** The current touch location for the right zone */
	TouchInstance _rtouch;
	/** The current touch location for the bottom zone */
	TouchInstance _mtouch;
    
    /** Whether the virtual joystick is active */
    bool _joystick;
    /** The position of the virtual joystick */
    cugl::Vec2 _joycenter;
    /** Whether or not we have processed a jump for this swipe yet */
    bool _hasJumped;
    
    /** The timestamp for a double tap on the right */
    cugl::Timestamp _rtime;
	/** The timestamp for a double tap in the middle */
	cugl::Timestamp _mtime;

    /**
     * Defines the zone boundaries, so we can quickly categorize touches.
     */
	void createZones();
  
    /**
     * Populates the initial values of the TouchInstances
     */
    void clearTouchInstance(TouchInstance& touchInstance);

    /**
     * Returns the correct zone for the given position.
     *
     * See the comments above for a description of how zones work.
     *
     * @param  pos  a position in screen coordinates
     *
     * @return the correct zone for the given position.
     */
    Zone getZone(const cugl::Vec2 pos) const;
    
    /**
     * Returns the scene location of a touch
     *
     * Touch coordinates are inverted, with y origin in the top-left
     * corner. This method corrects for this and scales the screen
     * coordinates down on to the scene graph size.
     *
     * @return the scene location of a touch
     */
    cugl::Vec2 touch2Screen(const cugl::Vec2 pos) const;

    cugl::Vec2 mouse2Screen(const cugl::Vec2 pos) const;

    /**
     * Processes movement for the floating joystick.
     *
     * This will register movement as left or right (or neither).  It
     * will also move the joystick anchor if the touch position moves
     * too far.
     *
     * @param  pos  the current joystick position
     */
    void processJoystick(const cugl::Vec2 pos);
    
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
	int processSwipe(const cugl::Vec2 start, const cugl::Vec2 stop, cugl::Timestamp current);
  
public:
    
    /**
     * Returns the state of the inventory.
     *
     * @return the state of the inventory
     */
    InventoryStatus getInventoryStatus() { return _inventoryStatus; }
    
    /**
     *
     */
    void setInventoryStatus(InventoryStatus status) { _inventoryStatus = status; }
    
    /**
     * Returns the screen position that the most recently selected item should be placed at.
     *
     * @return the screen position to place the selected item
     */
    cugl::Vec2 getPlacedPos() { return _placedPos; }

    cugl::Vec2 getSystemDragOffset() const {
#ifdef CU_TOUCH_SCREEN
        return cugl::Vec2(-1, 1);
#else
        return cugl::Vec2(0, 0);
#endif
    }
    
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    PlatformInput(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~PlatformInput() { dispose(); }
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
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
    bool init(const cugl::Rect bounds);

    /**
     * Returns the scene coordinate of the last position on drag
     *
     * @return the location of the last touch drag position
     */
    cugl::Vec2 getPosOnDrag() {
#ifdef CU_TOUCH_SCREEN
        return _touchPosForDrag;
#else
        return _mousePosForDrag;
#endif
    }
    
    
#pragma mark -
#pragma mark Input Detection
    /**
     * Returns true if the input handler is currently active
     *
     * @return true if the input handler is currently active
     */
    bool isActive( ) const { return _active; }

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
    void update(float dt);

    /** Returns the position of the current touch in the middle/bottom zone. 
    */

    cugl::Vec2 getMTouchPosition() const {
        return _mtouch.position;
    }

    cugl::Vec2 getTouchReleasePosition() const {
        return _touchReleasePos;
    }

    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();
    
#pragma mark -
#pragma mark Input Results
    /**
     * Returns the amount of sideways movement.
     *
     * -1 = left, 1 = right, 0 = still
     *
     * @return the amount of sideways movement.
     */
	float getHorizontal() const { return _horizontal; }

    /**
     * Returns if the jump button was pressed.
     *
     * @return if the jump button was pressed.
     */
	float didJump() const { return _jumpPressed; }

    /**
     * Returns true if the fire button was pressed.
     *
     * @return true if the fire button was pressed.
     */
	bool didFire() const { return _firePressed; }

    /**
     * Returns true if the reset button was pressed.
     *
     * @return true if the reset button was pressed.
     */
	bool didReset() const { return _resetPressed; }

    /**
     * Returns true if the screen was just touched.
     *
     * @return true if the screen was just touched.
     */
    bool didPressFinger() const { return _currDown && !_prev2Down; }

    /**
     * Returns true if the touch was just released.
     *
     * @return true if the touch was just released.
     */
    bool didReleaseFinger() const { return _prev2Down && !_currDown; }

    /**
     * Returns true if the player wants to go toggle the debug mode.
     *
     * @return true if the player wants to go toggle the debug mode.
     */
	bool didDebug() const { return _debugPressed; }

	/**
	 * Returns true if the exit button was pressed.
	 *
	 * @return true if the exit button was pressed.
	 */
	bool didExit() const { return _exitPressed; }
    
    /**
     * Returns true if the virtual joystick is in use (touch only)
     *
     * @return true if the virtual joystick is in use (touch only)
     */
    bool withJoystick() const { return _joystick; }

    /**
     * Returns the scene graph position of the virtual joystick
     *
     * @return the scene graph position of the virtual joystick
     */
    cugl::Vec2 getJoystick() const { return _joycenter; }

    /**
     * Returns true if touch is down
     *
     * @return true if touch is down
     */
    bool isTouchDown() const { 
#ifdef CU_TOUCH_SCREEN
        return _touchDown;
#else
        return _mouseDown;
#endif 
    }

    /**Returns true if touch is down on the right*/
    bool isRightDown() const { return _holdRight; }
    

#pragma mark -
#pragma mark Touch and Mouse Callbacks
    /**
     * Callback for the beginning of a touch event
     *
     * @param event The associated event
     * @param focus	Whether the listener currently has focus
     *
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for the end of a touch event
     *
     * @param event The associated event
     * @param focus	Whether the listener currently has focus
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
  
    /**
     * Callback for a mouse release event.
     *
     * @param event The associated event
     * @param previous The previous position of the touch
     * @param focus	Whether the listener currently has focus
     */
    void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);

    /**
     * Call back to execute when a mouse button is first pressed.
     *
     * This function will record a press only if the left button is pressed.
     *
     * @param event     The event with the mouse information
     * @param clicks    The number of clicks (for double clicking)
     * @param focus     Whether this device has focus (UNUSED)
     */
    void buttonDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);

    /**
     * Call back to execute when a mouse button is first released.
     *
     * This function will record a release for the left mouse button.
     *
     * @param event     The event with the mouse information
     * @param clicks    The number of clicks (for double clicking)
     * @param focus     Whether this device has focus (UNUSED)
     */
    void buttonUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);

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
    void motionCB(const cugl::MouseEvent& event, const cugl::Vec2 previous, bool focus);
  
};

#endif /* __PF_INPUT_H__ */
