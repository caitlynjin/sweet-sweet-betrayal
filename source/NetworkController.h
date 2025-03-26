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
#include "Treasure.h"
#include "Mushroom.h"

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

class MovingPlatFactory : public ObstacleFactory {
public:
    /** Pointer to the AssetManager for texture access, etc. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** Serializer for supporting parameters */
    LWSerializer _serializer;
    /** Deserializer for supporting parameters */
    LWDeserializer _deserializer;
    
    /**
     * Allocates a new instance of the moving platform factory using the given AssetManager.
     */
    static std::shared_ptr<MovingPlatFactory> alloc(std::shared_ptr<AssetManager>& assets) {
        auto f = std::make_shared<MovingPlatFactory>();
        f->init(assets);
        return f;
    }

    /**
     * Initializes the factory with the provided AssetManager.
     */
    void init(std::shared_ptr<AssetManager>& assets) {
        _assets = assets;
    }
    
    /**
     * Creates a moving platform obstacle and its associated scene node.
     */
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(
            Vec2 pos, Size size, Vec2 end, float speed, float scale);

    
    /**
     * Serializes the parameters for a moving platform.
     */
    std::shared_ptr<std::vector<std::byte>> serializeParams(Vec2 pos, Size size, Vec2 end, float speed, float scale);
    
    /**
     * Creates a moving platform obstacle using serialized parameters.
     */
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(
        const std::vector<std::byte>& params) override;
};
/**
 * The factory class for treasure objects.
 */
class TreasureFactory : public ObstacleFactory {
public:
    /** Pointer to the AssetManager for texture access and related resources. */
    std::shared_ptr<AssetManager> _assets;

    /** Serializer for supporting parameters */
    LWSerializer _serializer;
    /** Deserializer for supporting parameters */
    LWDeserializer _deserializer;

    /**
     * Allocates a new instance of the treasure factory using the given AssetManager.
     *
     * @param assets The asset manager containing textures and other resources.
     *
     * @return A shared pointer to the newly allocated TreasureFactory.
     */
    static std::shared_ptr<TreasureFactory> alloc(std::shared_ptr<AssetManager>& assets) {
        auto f = std::make_shared<TreasureFactory>();
        f->init(assets);
        return f;
    }

    /**
     * Initializes the treasure factory with the provided AssetManager.
     *
     * @param assets The asset manager to be used for resource access.
     */
    void init(std::shared_ptr<AssetManager>& assets) {
        _assets = assets;
    }
    
    /**
     * Creates a treasure obstacle and its associated scene node.
     *
     * @param pos The position of the treasure in Box2D coordinates.
     * @param size The size of the treasure.
     *
     * @return A pair consisting of a physics obstacle and a scene node.
     */
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>>
    createObstacle(Vec2 pos, Size size, float scale, bool taken);

    /**
     * Serializes the parameters for a treasure.
     *
     * This method converts the provided parameters into a byte vector suitable for
     * network transmission so that the treasure can be recreated on other clients.
     *
     * @param pos The position of the treasure.
     * @param size The size of the treasure.
     *
     * @return A shared pointer to a byte vector containing the serialized parameters.
     */
    std::shared_ptr<std::vector<std::byte>>
    serializeParams(Vec2 pos, Size size, float scale, bool taken);
    
    /**
     * Creates a treasure obstacle using serialized parameters.
     *
     * This method decodes the serialized parameters and creates the corresponding treasure
     * obstacle along with its scene node.
     *
     * @param params The byte vector containing serialized parameters.
     *
     * @return A pair consisting of a physics obstacle and a scene node.
     */
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>>
    createObstacle(const std::vector<std::byte>& params) override;
};
/**
 * The factory class for mushroom objects.
 */
class MushroomFactory : public ObstacleFactory {
    public:
        std::shared_ptr<AssetManager> _assets;
        LWSerializer _serializer;
        LWDeserializer _deserializer;
    
        static std::shared_ptr<MushroomFactory> alloc(std::shared_ptr<AssetManager>& assets) {
            auto f = std::make_shared<MushroomFactory>();
            f->init(assets);
            return f;
        }
    
        void init(std::shared_ptr<AssetManager>& assets) {
            _assets = assets;
        }
        
        std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(Vec2 pos, Size size, float scale);

        std::shared_ptr<std::vector<std::byte>> serializeParams(Vec2 pos, Size size, float scale);
        
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
    std::vector<std::shared_ptr<Object>>* _objects;
    
    /** The network controller */
    std::shared_ptr<NetEventController> _network;
    
    /** The number of players ready to proceed from BuildPhase */
    float _numReady = 0;
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
    
    /** Variables for Moving Platform Factory*/
    std::shared_ptr<MovingPlatFactory> _movingPlatFact;
    Uint32 _movingPlatFactID;

    std::shared_ptr<TreasureFactory> _treasureFact;
    Uint32 _treasureFactID;

    /** Variables for Mushroom Factory (external, not nested) */
    std::shared_ptr<MushroomFactory> _mushroomFact;
    Uint32 _mushroomFactID;

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
        
        _movingPlatFact = MovingPlatFactory::alloc(_assets);
        _movingPlatFactID = _network->getPhysController()->attachFactory(_movingPlatFact);
        // Setup Treasure Factory
        _treasureFact = TreasureFactory::alloc(_assets);
        _treasureFactID = _network->getPhysController()->attachFactory(_treasureFact);

        _mushroomFact = MushroomFactory::alloc(_assets);
        _mushroomFactID = _network->getPhysController()->attachFactory(_mushroomFact);
    }
    
    /**
     * Sets the network world.
     *
     * @param world the world to be used for networked physics.
     */
    void setObjects(std::vector<std::shared_ptr<Object>>* objects){
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
     * Creates a networked moving platform.
     *
     * @return the moving platform being created
     */
    std::shared_ptr<Object> createMovingPlatformNetworked(Vec2 pos, Size size, Vec2 end, float speed, float scale);

    /**
     * Creates a networked treasure.
     *
     * @return the treasure being created
     */
    std::shared_ptr<Object> createTreasureNetworked(Vec2 pos, Size size, float scale, bool taken);

    /**
     * Creates a networked mushroom.
     *
     * @return the mushroom being created
     */
    std::shared_ptr<Object> createMushroomNetworked(Vec2 pos, Size size, float scale);
    
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
