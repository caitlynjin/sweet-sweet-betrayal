//
//  SSBLoadingScene.cpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 4/22/25.
//

#include "SSBLoadingScene.h"
#include <cugl/graphics/loaders/CUTextureLoader.h>
#include <cugl/graphics/loaders/CUFontLoader.h>
#include <cugl/scene2/CUScene2Loader.h>
#include <cugl/core/assets/CUWidgetLoader.h>

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;

/** Define the time settings for animation */
#define DURATION 1.0f
#define ACT_KEY  "current"

/**
 * Initializes a loading scene with the given scene and directory.
 *
 * This class will create its own {@link AssetManager}, which can be
 * accessed via {@link #getAssetManager}. This asset manager will only
 * attach loaders for {@link Font}, {@link Texture}, {@link scene2::SceneNode}
 * and {@link WidgetValue}.
 *
 * The string scene should be a path to a JSON file that defines the scene
 * graph for this loading scene. This file will be loaded synchronously, so
 * it should be lightweight. The scene must include a {@link scene2::SceneNode}
 * named "load". This node must have at least four children:
 *
 *     - "load.before": The scene to display while loading is in progress
 *     - "load.after": The scene to display when the loading is complete
 *     - "load.bar": A {@link ProgressBar} for showing the loading progress
 *     - "load.play" A play {@link Button} for the user to start the game
 *
 * The string directory is the asset directory to be loaded asynchronously
 * by this scene. Loading will commence after a call to {@link #start}. The
 * progress on this directory can be monitored via {@link #getProgress}.
 *
 * @param scene     A JSON file with the scene graph for this scene
 * @param directory The asset directory to load asynchronously
 *
 * @return true if the scene is initialized properly, false otherwise.
 */
bool SSBLoadingScene::init(const std::string scene, const std::string directory) {
    _assets = AssetManager::alloc();
    if (_assets == nullptr || !_assets->loadDirectory(scene) ) {
        return false;
    }
    
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    
    return init(_assets,directory);
}

/**
 * Initializes a loading scene with the given asset manager and directory.
 *
 * The asset manager must already contain the scene graph used by this
 * scene. The scene must include a {@link scene2::SceneNode} named
 * "load". This node must have at least four children:
 *
 *     - "load.before": The scene to display while loading is in progress
 *     - "load.after": The scene to display when the loading is complete
 *     - "load.bar": A {@link ProgressBar} for showing the loading progress
 *     - "load.play" A play {@link Button} for the user to start the game
 *
 * The string directory is the asset directory to be loaded asynchronously
 * by this scene. The progress on this directory can be monitored via
 * {@link #getProgress}.
 *
 * @param asset     A previously initialized asset manager
 * @param directory The asset directory to load asynchronously
 *
 * @return true if the scene is initialized properly, false otherwise.
 */
bool SSBLoadingScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
                        const std::string directory) {
    
    _assets = assets;
    _directory = directory;
    // First make sure we have the scene
    auto layer = assets->get<scene2::SceneNode>("load");
    if (layer == nullptr) {
        CUAssertLog(false,"Missing \"load\" in scene specification");
        return false;
    }
    
    
//    auto loadImage = assets->get<Texture>("loading-anim");
    
    _loadSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>("loading-anim"), 1, 20, 20);
    
        
    _timeline = ActionTimeline::alloc();
    
    std::vector<int> forward;
    for (int ii = 0; ii < 20; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animations
    _loadAnimateSprite = AnimateSprite::alloc(forward);
    _loadAction = _loadAnimateSprite->attach<scene2::SpriteNode>(_loadSpriteNode);
    
//    addChild(loadSpriteNode);
    
    
//    _before  = assets->get<scene2::SceneNode>("load.before");
//    _after = assets->get<scene2::SceneNode>("load.after");
//    _bar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("load.bar"));
//    _button = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load.play"));
//    if (_bar == nullptr) {
//        CUAssertLog(false,"Missing progress bar in scene specification");
//        return false;
//    }
//    if (_button == nullptr) {
//        CUAssertLog(false,"Missing play button in scene specification");
//        return false;
//    }
    
    // Check to see if we have a size hint
    if (layer->getJSON()->has("size")) {
        if (!Scene2::initWithHint(layer->getContentSize())) {
            return false;
        }
    } else if (!Scene2::init()) {
        return false;
    }

    // Rearrange the children to fit the screen
    _loadSpriteNode->setPosition(_size*0.5);
    layer->setContentSize(_size);
    layer->doLayout();
    
    
    // Ensure the correct visibility
    if (_before) {
        _before->setVisible(false);
    }
//    if (_after) {
        //TODO: Maybe replace with screen transition
//        _after->setVisible(false);
//    }


    Application::get()->setClearColor(Color4(0,0,0,255));
//    addChild(layer);
    addChild(_loadSpriteNode);
    
    return true;
}

/**
 * Performs a film strip action
 *
 * @param key   The action key
 * @param action The film strip action
 * @param slide  The associated movement slide
 */
void SSBLoadingScene::doStrip(std::string key, cugl::ActionFunction action, float duration = DURATION) {
    if (_timeline->isActive(key)) {
        // NO OP
    } else {
        _timeline->add(key, action, duration);
    }
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void SSBLoadingScene::dispose() {

    _assets = nullptr;
    _before = nullptr;
//    _after  = nullptr;
    _progress = 0.0f;
    _completed = false;
}

/**
 * Starts the loading progress for this scene
 *
 * This method has no affect if loading is already in progress.
 */
void SSBLoadingScene::start() {
    if (_started) {
        return;
    }
    _started = true;
    _assets->loadDirectoryAsync(_directory, nullptr);
}

/**
 * Updates the loading scene progress.
 *
 * This method queries the asset manager to update the progress bar amount.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void SSBLoadingScene::update(float progress) {
    if (_progress < 1) {
        _progress = _assets->progress();
        if (_progress >= 1) {
            _progress = 1.0f;
            _completed = true;
            if (_before) {
                _before->setVisible(false);
            }
//            if (_after) {
//                _after->setVisible(true);
//            }
//            _bar->setVisible(false);
//            _button->setVisible(true);
//            _button->activate();
        }
//        _bar->setProgress(_progress);
        doStrip("load", _loadAction, DURATION);
        _timeline->update(progress);
        
    }
//    doStrip("load", _loadAction, DURATION);
//    _timeline->update(progress);
}

