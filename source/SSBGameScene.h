//
//  SSBGameScene.h
//  SweetSweetBetrayal
//
//  Created by Grace Sawatyanon on 18/2/25.
//

#ifndef __PF_GAME_SCENE_H__
#define __PF_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"
#include "SSBInput.h"
#include "SSBDudeModel.h"
#include "SSBGridManager.h"
#include "Platform.h"
#include "Spike.h"
#include "WindObstacle.h"
#include "Treasure.h"
#include "UIScene.h"
//#include <cmath>

using namespace cugl;
using namespace Constants;

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public scene2::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;

    /** A list of all objects to be updated during each animation frame. */
    std::vector<std::shared_ptr<Object>> _objects;
    
    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    
    // VIEW
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _worldnode;
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _debugnode;
    /** Reference to the win message label */
    std::shared_ptr<scene2::Label> _winnode;
    /** Reference to the lose message label */
    std::shared_ptr<scene2::Label> _losenode;
    /** Reference to the rounds message label */
    std::shared_ptr<scene2::Label> _roundsnode;
    /** Reference to the gems message label */
    std::shared_ptr<scene2::Label> _gemsnode;
    /** Reference to the left joystick image */
    std::shared_ptr<scene2::PolygonNode> _leftnode;
    /** Reference to the right joystick image */
    std::shared_ptr<scene2::PolygonNode> _rightnode;
    /** The camera for this scene */
    std::shared_ptr<scene2::ScrollPane> _scrollpane;
    /** Reference to the ready button */
    std::shared_ptr<cugl::scene2::Button> _readyButton;
    /** Reference to build mode inventory buttons */
    std::vector<std::shared_ptr<scene2::Button>> _inventoryButtons;
    /** Reference to the grid manager */
    std::shared_ptr<GridManager> _gridManager;

    /** Reference to the label for counting rounds */
    std::shared_ptr<cugl::scene2::Label> _roundsLabel;
    
    std::vector<std::shared_ptr<scene2::PolygonNode>> _scoreImages;

    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;
    /** Reference to the background of the inventory */
    std::shared_ptr<scene2::PolygonNode> _inventoryBackground;
    /** Reference to the overlay of the inventory */
    std::shared_ptr<scene2::PolygonNode> _inventoryOverlay;

    /** The primary controller for the UI */
    UIScene _ui;
    /** The Box2D world */
    std::shared_ptr<physics2::ObstacleWorld> _world;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;
    /** The offset from the world */
    Vec2 _offset;

    // Physics objects for the game
    /** Reference to the goalDoor (for collision detection) */
    std::shared_ptr<physics2::BoxObstacle>    _goalDoor;
    /** Reference to the player avatar */
    std::shared_ptr<DudeModel>              _avatar;

    std::shared_ptr<Platform> _platformTest;
    
    /** Reference to the treasure */
    std::shared_ptr<Treasure> _treasure;

    /** Whether the player is currently in the middle of a sling */
    bool _slingInProgress;

    cugl::Path2 _trajectoryPath;

    cugl::Poly2 _trajectoryPoly;

    std::shared_ptr<cugl::scene2::SceneNode>  _pathNode;

    std::shared_ptr<cugl::scene2::SceneNode>  _pathNode2;

    /** Previous position of object in build phase */
    Vec2 _prevPos = Vec2(0, 0);

    /** Whether we have completed this "game" */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;
    /** Whether we have failed at this world (and need a reset) */
    bool _failed;
    /** Whether the player has died */
    bool _died = false;
    /** Whether the player has reached the goal */
    bool _reachedGoal = false;
    /** Countdown active for winning or losing */
    int _countdown;
    /** Whether we are in build mode */
    bool _buildingMode;
    /** The selected item in build mode (new object) */
    Item _selectedItem;
    /** The selected object in build mode (object being moved) */
    std::shared_ptr<Object> _selectedObject;
    /** The initial camera position */
    Vec2 _camerapos;

    /** The total amount of rounds */
    int const TOTAL_ROUNDS = 5;
    /** The total amount of gems */
    int const TOTAL_GEMS = 3;
    /** The current round the player is on */
    int _currRound = 1;
    /** How many gems the player collected and won */
    int _currGems = 0;
    /** The number of items currently placed */
    int _itemsPlaced = 0;

    /** Mark set to handle more sophisticated collision callbacks */
    std::unordered_set<b2Fixture*> _sensorFixtures;
private:
    /** Initial width */
    float _growingWallWidth = 1.0f;
    /** Growth rate per second  */        
    float _growingWallGrowthRate = 0.4f;      
    std::shared_ptr<physics2::PolygonObstacle> _growingWall;
    std::shared_ptr<scene2::PolygonNode> _growingWallNode;
    /**
    * Create the growing wall if not created. Otherwise, increase its width
    *
    * @param timestep  The elapsed time since the last frame.
    */
    void updateGrowingWall(float timestep);

#pragma mark Internal Object Management


    /** Creates a spike.
    * @param pos The position of the bottom left corner of the spike in Box2D coordinates.
    * @param size The size of the spike in Box2D coordinates.
    */
    std::shared_ptr<Object> createSpike(Vec2 pos, Size size, float scale, float angle = 0);

    std::shared_ptr<Object> createSpike(std::shared_ptr<Spike> spk);
    
    /** Creates a treasure
    * @param pos The position of the bottom left corner of the treasure in Box2D coordinates.
    * @param size The size of the treasure in Box2D coordinates.
    */
    std::shared_ptr<Object> createTreasure(Vec2 pos, Size size);

    std::shared_ptr<Object> createTreasure(std::shared_ptr<Treasure> treasure);

    /**
     * Creates a platform.
     *
     * @return the platform being created
     *
     * @param pos The position of the bottom left corner of the platform in Box2D coordinates.
     * @param size The size of the platform in Box2D coordinates.
     * @param wall Whether this is a wall or not (if not it is a user placed platform)
     */
    std::shared_ptr<Object> createPlatform(Vec2 pos, Size size, bool wall);

    /**
     * Creates a platform.
     *
     * @return the platform being created
     *
     * @param The platform being created (that has not yet been added to the physics world).
     */
    std::shared_ptr<Object> createPlatform(std::shared_ptr<Platform> plat);

    /**
     * Creates a new windobstacle
     *
     * @return the wind obstacle
     *
     * @param pos The position of the bottom left corner of the platform in Box2D coordinates.
     * @param size The dimensions (width, height) of the platform.
     */
    std::shared_ptr<Object> createWindObstacle(Vec2 pos, Size size, Vec2 gustDir);

    std::shared_ptr<Object> createWindObstacle(std::shared_ptr<WindObstacle> wind);

    /**
     * Creates a moving platform.
     *
     * @return the moving platform
     *
     * @param pos The bottom left position of the platform starting position
     * @param size The dimensions of the platform.
     * @param end The bottom left position of the platform's destination.
     * @param speed The speed at which the platform moves.
     */
    std::shared_ptr<Object> createMovingPlatform(Vec2 pos, Size size, Vec2 end, float speed);

    /**
     * Lays out the game geography.
     *
     * Pay close attention to how we attach physics objects to a scene graph.
     * The simplest way is to make a subclass, like we do for the dude.  However,
     * for simple objects you can just use a callback function to lightly couple
     * them.  This is what we do with the crates.
     *
     * This method is really, really long.  In practice, you would replace this
     * with your serialization loader, which would process a level file.
     *
     */
    void populate();
    
    /**
     * Adds the physics object to the physics world and loosely couples it to the scene graph
     *
     * There are two ways to link a physics object to a scene graph node on the
     * screen.  One way is to make a subclass of a physics object, like we did
     * with dude.  The other is to use callback functions to loosely couple
     * the two.  This function is an example of the latter.
     *
     * @param obj    The physics object to add
     * @param node   The scene graph node to attach it to
     * @param useObjPosition  Whether to update the node's position to be at the object's position
     */
    void addObstacle(const std::shared_ptr<physics2::Obstacle>& obj,
                     const std::shared_ptr<scene2::SceneNode>& node,
                     bool useObjPosition=true);
    
public:
#pragma mark -
#pragma mark Constructors

    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    GameScene();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
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
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<AssetManager>& assets,
              const Rect& rect);
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     * @param gravity   The gravitational force on this Box2d world
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<AssetManager>& assets,
              const Rect& rect, const Vec2& gravity);

#pragma mark -
#pragma mark Build Mode
    
    /**
     * Initializes the inventory for build mode.
     */
    void initInventory();
    
    /**
     * Creates an item of type item and places it at the grid position.
     *
     * @return the object being placed
     *
     * @param gridPos   The grid position to place the item at
     * @param item  The type of the item to be placed/created
     */
    std::shared_ptr<Object> placeItem(Vec2 gridPos, Item item);

    /**
     * Returns the corresponding asset name to the item.
     *
     * @param item The item
     * @Return the item's asset name
     */
    std::string itemToAssetName(Item item);
    
#pragma mark -
#pragma mark State Access
    /**
     * Returns true if debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @return true if debug mode is active.
     */
    bool isDebug( ) const { return _debug; }
    
    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
    void setDebug(bool value) { _debug = value; _debugnode->setVisible(value); }
    
    /**
     * Returns true if the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @return true if the level is completed.
     */
    bool isComplete( ) const { return _complete; }
    
    /**
     * Sets whether the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @param value whether the level is completed.
     */
    void setComplete(bool value);

    /**
    * Returns true if the level is failed.
    *
    * If true, the level will reset after a countdown
    *
    * @return true if the level is failed.
    */
    bool isFailure() const { return _failed; }

    /**
    * Sets whether the level is failed.
    *
    * If true, the level will reset after a countdown
    *
    * @param value whether the level is failed.
    */
    void setFailure(bool value);
    
    /**
    * Sets the level up for the next round.
    *
    * When called, the level will reset after a countdown.
     
     @param reachedGoal whether the player has reached the goal.
    */
    void nextRound(bool reachedGoal = false);

     /**
     * Sets whether mode is in building or play mode.
     *
     * @param value whether the level is in building mode.
     */
    void setBuildingMode(bool value);


#pragma mark -
#pragma mark Collision Handling
    /**
    * Processes the start of a collision
    *
    * This method is called when we first get a collision between two objects.  We use
    * this method to test if it is the "right" kind of collision.  In particular, we
    * use it to test if we make it to the win door.  We also us it to eliminate bullets.
    *
    * @param  contact  The two bodies that collided
    */
    void beginContact(b2Contact* contact);

    /**
    * Processes the end of a collision
    *
    * This method is called when we no longer have a collision between two objects.
    * We use this method allow the character to jump again.
    *
    * @param  contact  The two bodies that collided
    */
    void endContact(b2Contact* contact);

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
    void reset() override;

    void setSpriteBatch(const shared_ptr<SpriteBatch> &batch);

    void render() override;


#pragma mark -
#pragma mark Helpers
    /**
     * Converts from screen to Box2D coordinates.
     *
     * @return the Box2D position
     *
     * @param screenPos    The screen position
     * @param scale             The screen to world scale
     * @param offset           The offset of the scene to the world
     */
    Vec2 convertScreenToBox2d(const Vec2& screenPos, float scale, const Vec2& offset);

    /**
     * Snaps the Box2D position to within the bounds of the build phase grid.
     *
     * @return the grid position
     *
     * @param screenPos    The screen position
     * @param item               The selected item being snapped to the grid
     */
    Vec2 snapToGrid(const Vec2 &gridPos, Item item);

  };

#endif /* __PF_GAME_SCENE_H__ */
