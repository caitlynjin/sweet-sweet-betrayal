//
//  VictoryScene.h
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 4/13/25.
//

#ifndef VictoryScene_h
#define VictoryScene_h

#include <cugl/cugl.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"
#include "SSBInput.h"
#include "SoundController.h"
#include "NetworkController.h"
//#include <cmath>

using namespace cugl;
using namespace Constants;

/**
 * This class presents the start screen to the player.
 */
class VictoryScene : public cugl::scene2::Scene2 {
public:
    /**
     * The start screen choice.
     */
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to start a game */
        RESTART,
        QUIT
    };

protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<SoundController> _sound;
    
    /** The network controller */
    std::shared_ptr<NetworkController> _networkController;
    /** The network  */
    std::shared_ptr<NetEventController> _network;
    
    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;
    
    std::shared_ptr<cugl::scene2::Button> _restartButton;
    std::shared_ptr<cugl::scene2::Button> _quitButton;
    /** The player start screen choice */
    Choice _choice;

public:
#pragma mark -
#pragma mark Constructors
    VictoryScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~VictoryScene() { dispose(); }
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> sound, const std::shared_ptr<NetworkController> networkController);
    
    virtual void setActive(bool value) override;
    
    Choice getChoice() const { return _choice; }
    
    /**
     * The method called to indicate the start of a deterministic loop.
     *
     * This method is used instead of {@link #update} if {@link #setDeterministic}
     * is set to true. It marks the beginning of the core application loop,
     * which is concluded with a call to {@link #postUpdate}.
     *
     * This method should be used to process any events that happen early in
     * the application loop, such as user input or events created by the
     * {@link schedule} method. In particular, no new user input will be
     * recorded between the time this method is called and {@link #postUpdate}
     * is invoked.
     *
     * Note that the time passed as a parameter is the time measured from the
     * start of the previous frame to the start of the current frame. It is
     * measured before any input or callbacks are processed. It agrees with
     * the value sent to {@link #postUpdate} this animation frame.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void preUpdate(float dt);

    /**
     * The method called to provide a deterministic application loop.
     *
     * This method provides an application loop that runs at a guaranteed fixed
     * timestep. This method is (logically) invoked every {@link getFixedStep}
     * microseconds. By that we mean if the method {@link draw} is called at
     * time T, then this method is guaranteed to have been called exactly
     * floor(T/s) times this session, where s is the fixed time step.
     *
     * This method is always invoked in-between a call to {@link #preUpdate}
     * and {@link #postUpdate}. However, to guarantee determinism, it is
     * possible that this method is called multiple times between those two
     * calls. Depending on the value of {@link #getFixedStep}, it can also
     * (periodically) be called zero times, particularly if {@link #getFPS}
     * is much faster.
     *
     * As such, this method should only be used for portions of the application
     * that must be deterministic, such as the physics simulation. It should
     * not be used to process user input (as no user input is recorded between
     * {@link #preUpdate} and {@link #postUpdate}) or to animate models.
     *
     * The time passed to this method is NOT the same as the one passed to
     * {@link #preUpdate}. It will always be exactly the same value.
     *
     * @param step  The number of fixed seconds for this step
     */
    void fixedUpdate(float step);

    /**
     * The method called to indicate the end of a deterministic loop.
     *
     * This method is used instead of {@link #update} if {@link #setDeterministic}
     * is set to true. It marks the end of the core application loop, which was
     * begun with a call to {@link #preUpdate}.
     *
     * This method is the final portion of the update loop called before any
     * drawing occurs. As such, it should be used to implement any final
     * animation in response to the simulation provided by {@link #fixedUpdate}.
     * In particular, it should be used to interpolate any visual differences
     * between the the simulation timestep and the FPS.
     *
     * This method should not be used to process user input, as no new input
     * will have been recorded since {@link #preUpdate} was called.
     *
     * Note that the time passed as a parameter is the time measured from the
     * last call to {@link #fixedUpdate}. That is because this method is used
     * to interpolate object position for animation.
     *
     * @param remain    The amount of time (in seconds) last fixedUpdate
     */
    void postUpdate(float remain);

};

#endif /* VictoryScene_h */
