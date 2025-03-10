//
// Created by chvel on 3/5/2025.
//

#ifndef SWEETSWEETBETRAYAL_UISCENE_H
#define SWEETSWEETBETRAYAL_UISCENE_H
#include <cugl/cugl.h>
#include "Constants.h"
#include "Platform.h"
#include "SSBInput.h"

using namespace cugl;
using namespace Constants;

/**
 * This class is the scene for the UI of the game.
 *
 * Since the game itself has a camera that moves along with the player,
 * this class makes it so that the UI always stays on screen.
 *
 */
class UIScene : public scene2::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;

    /** A list of all objects to be updated during each animation frame. */
    std::vector<std::shared_ptr<Object>> _objects;

    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;

    /** Reference to the ready button */
    std::shared_ptr<cugl::scene2::Button> _readyButton;
    /** Reference to the right button */
    std::shared_ptr<cugl::scene2::Button> _rightButton;
    /** Reference to the left button */
    std::shared_ptr<cugl::scene2::Button> _leftButton;

    /** The total amount of rounds */
    int const TOTAL_ROUNDS = 5;

    /** Reference to the label for counting rounds */
    std::shared_ptr<cugl::scene2::Label> _roundsLabel;

    /** Reference to the rounds message label */
    std::shared_ptr<scene2::Label> _roundsnode;

    /** Whether ready button was pressed */
    bool _readypressed;

    /** Whether right camera button is being pressed */
    bool _rightpressed;

    /** Whether left camera button is being pressed */
    bool _leftpressed;

    std::vector<std::shared_ptr<scene2::PolygonNode>> _scoreImages;
public:
#pragma mark -
#pragma mark Constructors

    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    UIScene();

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~UIScene() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  This initializer uses the default scale.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<AssetManager>& assets);

    /**
     * @return true if the ready button was pressed
     */
    bool getReadyPressed();

    /**
     * @return true if the right button was pressed
     */
    bool getRightPressed();

    /**
     * @return true if the left button was pressed
     */
    bool getLeftPressed();

    /**
     * Makes the buttons in the building mode visible
     */
    void visibleButtons(bool isVisible);

    /**
    * Updates round counter
    *
    * @param cur       The current round number
    * @param total     The total number of rounds
    */
    void updateRound(int cur, int total);

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This is the nondeterministic version of a physics simulation. It is
     * provided for comparison purposes only.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);


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


    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();

};


#endif //SWEETSWEETBETRAYAL_UISCENE_H
