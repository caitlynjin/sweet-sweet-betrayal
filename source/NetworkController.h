//
//  NetworkController.h
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 3/13/25.
//

#ifndef NetworkController_h
#define NetworkController_h

#include <cugl/cugl.h>
#include "Object.h"
#include "SSBDudeModel.h"
#include "Platform.h"
#include "Constants.h"
#include "MessageEvent.h"

using namespace cugl;
using namespace cugl::netcode;
using namespace cugl::physics2::distrib;
using namespace Constants;


// TODO: Separate factories into separate file?


/**
 * The factory class for player/dude model.
 *
 * This class is used to support automatically syncing newly added players mid-simulation.
 * Players added throught the ObstacleFactory class from one client will be added to all
 * clients in the simulations.
 */

class DudeFactory : public ObstacleFactory {
public:
    /** Pointer to the AssetManager for texture access, etc. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Serializer for supporting parameters */
    LWSerializer _serializer;
    /** Deserializer for supporting parameters */
    LWDeserializer _deserializer;

    /**
     * Allocates a new instance of the factory using the given AssetManager.
     */
    static std::shared_ptr<DudeFactory> alloc(std::shared_ptr<AssetManager>& assets) {
        auto f = std::make_shared<DudeFactory>();
        f->init(assets);
        return f;
    };

    /**
     * Initializes empty factories using the given AssetManager.
     */
    void init(std::shared_ptr<AssetManager>& assets) {
        _assets = assets;
    }
    
    /**
     * Generate a pair of Obstacle and SceneNode using the given parameters
     */
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(Vec2 pos, float scale);

    /**
     * Helper method for converting normal parameters into byte vectors used for syncing.
     */
    std::shared_ptr<std::vector<std::byte>> serializeParams(Vec2 pos, float scale);
    
    /**
     * Generate a pair of Obstacle and SceneNode using serialized parameters.
     */
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(const std::vector<std::byte>& params) override;
};


/**
 * The factory class for trap objects.
 *
 * This class is used to support automatically syncing newly added obstacle mid-simulation.
 * Obstacles added throught the ObstacleFactory class from one client will be added to all
 * clients in the simulations.
 */
class PlatformFactory : public ObstacleFactory {
    
    enum class JsonType {
        TILE,
        PLATFORM,
        LOG
    };
    
public:
    /** Pointer to the AssetManager for texture access, etc. */
    std::shared_ptr<cugl::AssetManager> _assets;

    /** Serializer for supporting parameters */
    LWSerializer _serializer;
    /** Deserializer for supporting parameters */
    LWDeserializer _deserializer;

    /**
     * Allocates a new instance of the factory using the given AssetManager.
     */
    static std::shared_ptr<PlatformFactory> alloc(std::shared_ptr<AssetManager>& assets) {
        auto f = std::make_shared<PlatformFactory>();
        f->init(assets);
        return f;
    };

    /**
     * Initializes empty factories using the given AssetManager.
     */
    void init(std::shared_ptr<AssetManager>& assets) {
        _assets = assets;
    }
    
    /**
     * Generate a pair of Obstacle and SceneNode using the given parameters
     */
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(Vec2 pos, Size size, int jsonType, float scale);

    /**
     * Helper method for converting normal parameters into byte vectors used for syncing.
     */
    std::shared_ptr<std::vector<std::byte>> serializeParams(Vec2 pos, Size size, string jsonType, float scale);
    
    /**
     * Generate a pair of Obstacle and SceneNode using serialized parameters.
     */
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(const std::vector<std::byte>& params) override;
};


/**
 * This class is the scene for the UI of the game.
 *
 * Since the game itself has a camera that moves along with the player,
 * this class makes it so that the UI always stays on screen.
 *
 */
class NetworkController {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;
    
    /** The Box2D world */
    std::shared_ptr<cugl::physics2::distrib::NetWorld> _world;

    /** A list of all objects to be updated during each animation frame. */
    std::vector<std::shared_ptr<Object>> _objects;
    
    /** The network controller */
    std::shared_ptr<NetEventController> _network;
    
    /** The number of players ready to proceed from BuildPhase into MovementPhase */
    float _numReady = 0;
    /** The number of players ready to proceed from MovementPhase into BuildPhase */
    float _numReset = 0;
    /** Whether the player is the host */
    bool _isHost;
    /** Whether the message has been sent */
    bool _readyMessageSent = false;
    /** The player's ID */
    int _localID;
    /** The other player's ID */
    int _otherID;
    /** Whether we have set collision filters for all players */
    bool _filtersSet = false;
    
    /** Variables for Platform Factory */
    std::shared_ptr<PlatformFactory> _platFact;
    Uint32 _platFactId;
    
    /** Variables for Dude Factory */
    std::shared_ptr<DudeFactory> _dudeFact;
    Uint32 _dudeFactID;


public:
#pragma mark -
#pragma mark Constructors

    /**
     * Creates a new network controller with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    NetworkController();

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~NetworkController() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /** This method allocates a NetworkController. */
    static std::shared_ptr<NetworkController> alloc(const std::shared_ptr<AssetManager>& assets) {
        std::shared_ptr<NetworkController> result = std::make_shared<NetworkController>();
        return (result->init(assets) ? result : nullptr);
    }

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


#pragma mark -
#pragma mark Network Handling
    
    /**
     Returns the network.
     */
    std::shared_ptr<NetEventController> getNetwork(){
        return _network;
    }
    
    /**
     * Sets the network world.
     *
     * @param world the world to be used for networked physics.
     */
    void setWorld(std::shared_ptr<cugl::physics2::distrib::NetWorld> world){
        _world = world;
        
        // Setup factories
        _platFact = PlatformFactory::alloc(_assets);
        _platFactId = _network->getPhysController()->attachFactory(_platFact);

        _dudeFact = DudeFactory::alloc(_assets);
        _dudeFactID = _network->getPhysController()->attachFactory(_dudeFact);
    }
    
    /**
     * Sets the network world.
     *
     * @param world the world to be used for networked physics.
     */
    void setObjects(std::vector<std::shared_ptr<Object>> objects){
        _objects = objects;
    }
    
    /**
     * Sets whether this local user is the host.
     *
     * @param isHost whether is host.
     */
    void setIsHost(bool isHost){
        _isHost = isHost;
    }
    
    /**
     * Returns whether this local user is the host.
     */
    bool getIsHost(){
        return _isHost;
    }
    
    /**
     * Returns the localID
     */
    int getLocalID(){
        return _localID;
    }
    
    /**
     * Returns the number of players ready to proceed to Movement Phase
     */
    int getNumReady(){
        return _numReady;
    }
    
    /**
     * Sets how many players are ready to proceed to Movement Phase.
     *
     * @param numReady number of players ready
     */
    void setNumReady(int numReady){
        _numReady = numReady;
    }
    
    /**
     * This method attempts to set all the collision filters for the networked players.
     *
     * All filters should be set once the world contains the amount of connected players to avoid possible race condition.
     */
    void trySetFilters();
    
    /**
     * Returns whether game can switch to movement mode for all players.
     */
    bool canSwitchToMove(){
        return _numReady >= _network->getNumPlayers();
    }
    
    /**
     * Returns whether game can switch to movement mode for all players.
     */
    bool canSwitchToBuild(){
        return _numReset >= _network->getNumPlayers();
    }
    
    
#pragma mark -
#pragma mark Message Handling
    
    /**
     * This method takes a MessageEvent and processes it.
     */
    void processMessageEvent(const std::shared_ptr<MessageEvent>& event);
    
    
    
#pragma mark -
#pragma mark Create Networked Objects
    /**
     * Creates a networked platform.
     *
     * @return the platform being created
     *
     * @param The platform being created (that has not yet been added to the physics world).
     */
    std::shared_ptr<Object> createPlatformNetworked(Vec2 pos, Size size, string jsonType, float scale);
    
    /**
     * Creates a networked player.
     *
     * @return the player being created
     *
     * @param The player being created (that has not yet been added to the physics world).
     */
    std::shared_ptr<DudeModel> createPlayerNetworked(Vec2 pos, float scale);
    
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

#endif /* NetworkController_h */
