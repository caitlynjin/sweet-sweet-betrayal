//
//  MainMenu.h
//  SweetSweetBetrayal
//
//  Created by jessie jia on 3/5/25.
//
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"
#include "SSBInput.h"
#include "SSBDudeModel.h"
#include "Platform.h"
#include "WindObstacle.h"
#include "Treasure.h"
//#include <cmath>

using namespace cugl;
using namespace Constants;

/**
 * This class presents the menu to the player.
 *
 * There is no need for an input controller, as all input is managed by
 * listeners on the scene graph.  We only need getters so that the main
 * application can retrieve the state and communicate it to other scenes.
 */
class MenuScene : public cugl::scene2::Scene2 {
public: 
    /**
     * The menu choice.
     *
     * This state allows the top level application to know what the user
     * chose.
     */
//    enum Choice {
//        /** User has not yet made a choice */
//        NONE,
//        /** User wants to host a game */
//        HOST,
//        /** User wants to join a game */
//        JOIN
//    };

protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new  menu scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    MenuScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~MenuScene() { dispose(); }
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents.
     *
     * In previous labs, this method "started" the scene.  But in this
     * case, we only use to initialize the scene user interface.  We
     * do not activate the user interface yet, as an active user
     * interface will still receive input EVEN WHEN IT IS HIDDEN.
     *
     * That is why we have the method {@link #setActive}.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    /**
     * Allocates a new MenuScene instance.
     *
     * @param assets The asset manager.
     * @return A shared pointer to the new MenuScene.
     */
    static std::shared_ptr<MenuScene> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<MenuScene> result = std::make_shared<MenuScene>();
        return (result->init(assets) ? result : nullptr);
    };

};

