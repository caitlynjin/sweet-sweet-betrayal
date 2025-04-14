//
//  ScoreController.h
//  SweetSweetBetrayal
//
//  Created by jessie jia on 4/9/25.
//

#ifndef SCORE_CONTROLLER_H
#define SCORE_CONTROLLER_H

#include <stdio.h>
#include <cugl/cugl.h>
#include "ScoreEvent.h"
#include <unordered_map>
#include "PlayerModel.h"
#include "ColorEvent.h"

class ScoreController {
private:
    // CONSTANTS
    /** The score needed to win */
    int const WIN_SCORE = 6;
    
    struct RoundScore {
        int score;
        ScoreEvent::ScoreType scoreType;
    };
    
    //constant for scoreboard initialization
    Vec2 bar_position;
    Vec2 glider_position;
    Vec2 offset_betw_points;
    Vec2 offset_betw_players;
    
    //stores first dot position for each player for reference later
    std::unordered_map<std::string, Vec2> _playerBaseDotPos;


    /** Pointer to the AssetManager for texture access, etc. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    //             playerID             roundNumber  score, scoreType
    std::unordered_map<int, std::unordered_map<int, RoundScore>> _playerRoundScores;

    //             playerID  total_scoe
    std::unordered_map<int, int> _playerTotalScores;
    
    //maps player UID to color
    std::unordered_map<int, ColorType> _playerColors;
    
    Vec2 _anchor;
    
    //list storing all the nodes created
    std::unordered_map<std::string, std::shared_ptr<scene2::PolygonNode>> _scoreIcons;
    
    std::vector<std::shared_ptr<PlayerModel>> _playerList;
    
    cugl::scene2::Scene2* _scoreboardParent;
    

public:

    /**
     * Creates a new network controller with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    ScoreController();

    /**
     * Initializes the sound controller contents
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /** Allocates a SoundController
    *
    * @param assets    The (loaded) assets for this game mode
    */
    static std::shared_ptr<ScoreController> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<ScoreController> result = std::make_shared<ScoreController>();
        return (result->init(assets) ? result : nullptr);
    }

    /**
    * Disposes of all (non-static) resources allocated to this mode.
    *
    * This method is different from dispose() in that it ALSO shuts off any
    * static resources, like the input controller.
    */
    ~ScoreController() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    void reset();

    // TODO: add descriptions for each method
    
    void processScoreEvent(const std::shared_ptr<ScoreEvent>& event);
    
    int getTotalScore(int playerID) const;

    int getRoundScore(int playerID, int round) const;
    
    const std::unordered_map<int, ColorType>& getPlayerColors() const {
        return _playerColors;
    }
    
    ScoreEvent::ScoreType getRoundScoreType(int playerID, int round) const;
    
    void sendScoreEvent(const std::shared_ptr<NetEventController>& network, int playerID, ScoreEvent::ScoreType type, int roundNum);
    
    void setPlayerColors(const std::unordered_map<int, ColorType>& colors) {
        _playerColors = colors;

//        CULog("=== Player Colors Set ===\n");
        for (const auto& pair : _playerColors) {
            int playerID = pair.first;
            ColorType color = pair.second;

            std::string colorStr = colorToString(color);  // Assuming you have this helper
        }
    }

        
    std::string colorToString(ColorType color) const {
        switch (color) {
            case ColorType::RED:    return "playerRed";
            case ColorType::BLUE:   return "playerBlue";
            case ColorType::GREEN:  return "playerGreen";
            case ColorType::YELLOW: return "playerYellow";
            default:                return "playerRed";
        }
    }
    //set scoreboard visible by looping over all elements
    void setScoreboardVisible(bool visible) {
        for (auto const& entry : _scoreIcons) {
            entry.second->setVisible(visible);
        }
    }
    
    /** 
        Iterates through all players and checks for a win.
     */
    bool checkWinCondition();
    
    void initScoreboardNodes(cugl::scene2::Scene2* parent, const Vec2 &anchor, std::vector<std::shared_ptr<PlayerModel>> playerList, float size_width, float size_height);
    
    std::shared_ptr<scene2::PolygonNode> createIcon(const std::string& textureKey,
                                                    float scale,
                                                    const Vec2& position,
                                                    const Vec2 &anchor,
                                                    bool visible = false);
    
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
#endif
