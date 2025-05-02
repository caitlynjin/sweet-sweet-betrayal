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
#include "PlayerModel.h"
#include "Platform.h"
#include "Constants.h"
#include "MessageEvent.h"
#include "ColorEvent.h"
#include "MushroomBounceEvent.h"
#include "ReadyEvent.h"
#include "ScoreEvent.h"
#include "TreasureEvent.h"
#include "AnimationEvent.h"
#include "ScoreController.h"
#include "Treasure.h"
#include "Mushroom.h"
#include "WindObstacle.h"
#include "Thorn.h"
#include "Bomb.h"
#include "Message.h"

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
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(Vec2 pos, float scale, ColorType color);

    /**
     * Helper method for converting normal parameters into byte vectors used for syncing.
     */
    std::shared_ptr<std::vector<std::byte>> serializeParams(Vec2 pos, float scale, ColorType color);
    
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
 * The factory class for thorn objects.
 */
class ThornFactory : public ObstacleFactory {
    public:
        std::shared_ptr<AssetManager> _assets;
        LWSerializer _serializer;
        LWDeserializer _deserializer;

        static std::shared_ptr<ThornFactory> alloc(std::shared_ptr<AssetManager>& assets) {
            auto f = std::make_shared<ThornFactory>();
            f->init(assets);
            return f;
        }

        void init(std::shared_ptr<AssetManager>& assets) {
            _assets = assets;
        }

        std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(Vec2 pos, Size size);

        std::shared_ptr<std::vector<std::byte>> serializeParams(Vec2 pos, Size size);

        std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(const std::vector<std::byte>& params) override;
    };


/**
 * The factory class for wind objects.
 */
class WindFactory : public ObstacleFactory {
    public:
        std::shared_ptr<AssetManager> _assets;
        LWSerializer _serializer;
        LWDeserializer _deserializer;

        static std::shared_ptr<WindFactory> alloc(std::shared_ptr<AssetManager>& assets) {
            auto f = std::make_shared<WindFactory>();
            f->init(assets);
            return f;
        }

        void init(std::shared_ptr<AssetManager>& assets) {
            _assets = assets;
        }

        std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(Vec2 pos, Size size, float scale, Vec2 windDirection, Vec2 windStrength);

        std::shared_ptr<std::vector<std::byte>> serializeParams(Vec2 pos, Size size, float scale, Vec2 windDirection, Vec2 windStrength);

        std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(const std::vector<std::byte>& params) override;
    };

/**
 * The factory class for bomb objects.
 */
class BombFactory : public ObstacleFactory {
public:
    std::shared_ptr<AssetManager> _assets;
    LWSerializer _serializer;
    LWDeserializer _deserializer;

    static std::shared_ptr<BombFactory> alloc(std::shared_ptr<AssetManager>& assets) {
        auto f = std::make_shared<BombFactory>();
        f->init(assets);
        return f;
    }

    void init(std::shared_ptr<AssetManager>& assets) {
        _assets = assets;
    }

    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(Vec2 pos, Size size);

    std::shared_ptr<std::vector<std::byte>> serializeParams(Vec2 pos, Size size);

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
    
    /** The treasure */
    std::shared_ptr<Treasure> _treasure; 
    
    /** Current spawn location for the treasure */
    Vec2 _treasureSpawn;
    
    /** List of all possible treasure spawn points */
    std::vector<Vec2> _tSpawnPoints;
    
    /** List of all spawn points that have currently been used  */
    std::vector<Vec2> _usedSpawns;
    
    /** The local player */
    std::shared_ptr<PlayerModel> _localPlayer;
    
    /** The list of all players */
    std::vector<std::shared_ptr<PlayerModel>> _playerList;
    
    /** Map for accessing player color based off network id */
    std::unordered_map<int, ColorType> _playerColorsById;
    
    /** The player color */
    ColorType _color;
    
    /** The callback function when any player picks a color */
    std::function<void(ColorType, int)> _onColorTaken = nullptr;
    
    /**stores score controller instance**/
    std::shared_ptr<ScoreController> _scoreController;
    
    /** Whether the local player data has been recorded */
    bool _playerColorAdded = false;
    
    
    /** The number of players ready to proceed from Color Select Scene*/
    float _numColorReady = 0;
    /** The number of players ready to proceed from BuildPhase */
    float _numReady = 0;
    /** The number of players ready to proceed from MovementPhase into BuildPhase */
    float _numReset = 0;
    /** Whether the player is the host */
    bool _isHost;
    /** Whether the message has been sent */
    bool _readyMessageSent = false;
    /** The player's ID */
    int _localID;
    /** List of all players ids */
    std::vector<int> _playerIDs;
    /** Whether we have set collision filters for all players */
    bool _filtersSet = false;
    /** Whether we have synced all colors across players */
    bool _colorsSynced = false;
    /** Whether the level should reset */
    bool _resetLevel = false;
    
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

    /** Variables for Thorn Factory */
    std::shared_ptr<ThornFactory> _thornFact;
    Uint32 _thornFactID;
    
    /** Variables for Wind Factory */
    std::shared_ptr<WindFactory> _windFact;
    Uint32 _windFactID;

    /** Variables for Bomb Factory */
    std::shared_ptr<BombFactory> _bombFact;
    Uint32 _bombFactID;

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
    
    /**
     Resets the properties of the network
     */
    void resetNetwork();
    
    /** This method allocates a NetworkController. */
    static std::shared_ptr<NetworkController> alloc(const std::shared_ptr<AssetManager>& assets) {
        std::shared_ptr<NetworkController> result = std::make_shared<NetworkController>();
        return (result->init(assets) ? result : nullptr);
    }
    
    /** Flushes the connection and clears all events */
    void flushConnection();

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
    void setWorld(std::shared_ptr<cugl::physics2::distrib::NetWorld> world);
    
    /**
     * Sets the network world.
     *
     * @param world the world to be used for networked physics.
     */
    void setObjects(std::vector<std::shared_ptr<Object>>* objects){
        _objects = objects;
    }
    
    /**
     * Sets the networked treasure
     *
     * @param treasure the treasure to be networked
     */
    void setTreasure(std::shared_ptr<Treasure> treasure){
        _treasure = treasure;
    }
    
    /**
     * Sets the spawn location of the treasure.
     *
     * @param spawn the spawn location of the treasure
     */
    void setTreasureSpawn(Vec2 spawn){
        _treasureSpawn = spawn;
    }
    
    
    /**
     * Sets the local player.
     *
     * @param player the reference to the local player
     */
    void setLocalPlayer(std::shared_ptr<PlayerModel> player){
        _localPlayer = player;
    }
    
    /**
     Returns the reference to the local player.
     */
    std::shared_ptr<PlayerModel> getLocalPlayer(){
        return _localPlayer;
    }
    
    /**
     * Returns the spawn location of the treasure
     */
    Vec2 getTreasureSpawn(){
        return _treasureSpawn;
    }
    
    /**
     * Returns whether the level should reset
     */
    bool getResetLevel(){
        return _resetLevel;
    }
    
    /**
     * Returns whether the local player data has been set.
     */
    bool getPlayerColorAdded(){
        return _playerColorAdded;
    }
    
    
    /**
     Resets the treasure to its spawn location and removes any possession
     */
    void resetTreasure();
    
    /**
     Resets the treasure to its spawn location and removes any possession
     */
    void resetTreasureRandom();
    
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
     * Returns the set of player objects in game
     */
    std::vector<std::shared_ptr<PlayerModel>> getPlayerList(){
        return _playerList;
    }
    
    /**
     * Returns the color of the player by their shortUID
     */
    ColorType getPlayerColor(int ID){
        return _playerColorsById[ID];
    }
    
    /**
     * Returns the color of the local player.
     */
    ColorType getLocalColor(){
        return _color;
    }
    
    /**
     * Returns the localID
     */
    int getLocalID(){
        return _localID;
    }
    
    /**
     * Returns the no. of players ready to proceed from ColorSelect Phase
     */
    int getNumColorReady(){
        return _numColorReady;
    }
    
    /** Resets numColorReady to 0 */
    void resetColorReady() { _numColorReady = 0; }
    
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
     * This method syncs the display of all player colors within the network.
     */
    void syncColors();
    
    /**
     * Returns whether game can switch to movement mode for all players.
     */
    bool canSwitchToMove(){
        return _numReady >= _network->getNumPlayers();
    }
    
    /**
     * Returns whether game can switch to build mode for all players.
     */
    bool canSwitchToBuild(){
        return _numReset >= _network->getNumPlayers();
    }
    
    /**
     * Sets the local player's color
     */
    void setLocalColor(ColorType c) {
        _color = c;
        _playerColorAdded = true;
        CULog("Set local color: %d", _color);
    }
    std::shared_ptr<ScoreController> getScoreController() const { return _scoreController; }

#pragma mark -
#pragma mark Treasure Handling
    
    /**
     Adds a position for a treasure to spawn at.
     
     @param pos the spawn position of the treasure.
     */
    void addTreasureSpawn(const Vec2& pos){
        _tSpawnPoints.push_back(pos);
    }
    
    /**
     Returns the treasure being networked.
     */
    std::shared_ptr<Treasure> getTreasure(){
        return _treasure;
    }

    std::vector<std::shared_ptr<Object>>* getObjects(){return _objects;}
    
    /**
     Picks the next spawn point for the treasure at random.
     
     If a spawn point has been used already, it should be chosen again until all other spawn points have been used.
     */
    Vec2 pickRandSpawn();
    
#pragma mark -
#pragma mark Message Handling
    
    /**
     * This method takes a MessageEvent and processes it.
     */
    void processMessageEvent(const std::shared_ptr<MessageEvent>& event);
    
    /**
     * This method takes a ColorEvent and processes it.
     */
    void processColorEvent(const std::shared_ptr<ColorEvent>& event);

    /**
     * This method takes a ReadyEvent and processes it.
     */
    void processReadyEvent(const std::shared_ptr<ReadyEvent>& event);
    
    /**
     * This method takes a TreasureEvent and processes it.
     */
    void processTreasureEvent(const std::shared_ptr<TreasureEvent>& event);

    /**
     * This method takes a AnimationEvent and processes it.
     */
    void processAnimationEvent(const std::shared_ptr<AnimationEvent>& event);

    /**
     * This method takes a MushroomBounceEvent and processes it.
     */
    void processMushroomBounceEvent(const std::shared_ptr<MushroomBounceEvent>& event);
    
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
    std::shared_ptr<PlayerModel> createPlayerNetworked(Vec2 pos, float scale, ColorType color);
    
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
     * Creates a networked thorn.
     *
     * @return the thorn being created
     */
    std::shared_ptr<Object> createThornNetworked(Vec2 pos, Size size);
    
    
    /**
     * Creates a networked wind obstacle.
     *
     * @return the thorn being created
     */
    std::shared_ptr<Object> createWindNetworked(Vec2 pos, Size size, float scale, Vec2 dir, Vec2 str);

    /**
     * Creates a networked bomb.
     *
     * @return the bomb being created
     */
    std::shared_ptr<Object> createBombNetworked(Vec2 pos, Size size);

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
    
    
    /**
     * Resets the necessary logic to start a new round
     */
    void resetRound();
    
    /**
     Checks if win condition has been met and sends a message to reset the level.
     */
    bool checkWinCondition(){
//        if (_scoreController->checkWinCondition()){
//            
//          _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::RESET_LEVEL));
//        }
        return _scoreController->checkWinCondition();
    }
    
    /** Sets the onColorTaken callback function */
    void setOnColorTaken (const std::function<void(ColorType, int)>& function) { _onColorTaken = function; }

    void removeObject(std::shared_ptr<Object> object);

};

#endif /* NetworkController_h */
