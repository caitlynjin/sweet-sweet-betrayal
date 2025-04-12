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

class ScoreController {
private:
    
    struct RoundScore {
        int score;
        ScoreEvent::ScoreType scoreType;
    };

    /** Pointer to the AssetManager for texture access, etc. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    //             playerID             roundNumber  score, scoreType
    std::unordered_map<int, std::unordered_map<int, RoundScore>> _playerRoundScores;


    //             playerID  total_scoe
    std::unordered_map<int, int> _playerTotalScores;
    
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

    void processScoreEvent(const std::shared_ptr<ScoreEvent>& event);
    
    int getTotalScore(int playerID) const;

    int getRoundScore(int playerID, int round) const;
    
    ScoreEvent::ScoreType getRoundScoreType(int playerID, int round) const;
    
    void sendScoreEvent(const std::shared_ptr<NetEventController>& network, int playerID, ScoreEvent::ScoreType type, int roundNum);

};




#endif
