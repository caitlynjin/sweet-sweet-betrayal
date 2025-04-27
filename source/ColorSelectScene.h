//
//  ColorSelectScene.h
//  SweetSweetBetrayal
//
//  Created by Grace Sawatyanon on 27/4/25.
//

#ifndef __COLOR_SELECT_SCENE_H__
#define __COLOR_SELECT_SCENE_H__

#include <cugl/cugl.h>
#include "SoundController.h"
#include "SSBInput.h"

class ColorSelectScene : public cugl::scene2::Scene2 {
public:
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to return to the previous scene */
        BACK
    };
protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    std::shared_ptr<SoundController> _sound;
    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;
    /** The menu button for returning to the previous scene  */
    std::shared_ptr<cugl::scene2::Button> _backbutton;
    Choice _choice;
    
public:
#pragma mark -
#pragma mark Constructors
    
    ColorSelectScene() :cugl::scene2::Scene2() {}
    
    ~ColorSelectScene() { dispose(); }
    
    void dispose() override;
    
    void reset() override;
    
    /**
     * Initialize this scene with assets and sound controller.
     *
     * @param assets  Loaded asset manager
     * @param sound   Sound controller
     * @return true if successful
     */
    bool init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<SoundController>& sound);
    
    /**
     * Sets whether the scene is currently active
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
    
    Choice getChoice() const { return _choice; }
};

#endif // __COLOR_SELECT_SCENE_H__
