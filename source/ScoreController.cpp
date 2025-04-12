//
//  ScoreController.cpp
//  SweetSweetBetrayal
//
//  Created by jessie jia on 4/9/25.
//

#include "ScoreController.h"

ScoreController::ScoreController() {}

bool ScoreController::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    _assets = assets;
    return true;
}

void ScoreController::dispose() {
    _playerRoundScores.clear();
    _playerTotalScores.clear();
    _assets = nullptr;
}

void ScoreController::reset() {
    _playerRoundScores.clear();
    _playerTotalScores.clear();
}

void ScoreController::processScoreEvent(const std::shared_ptr<ScoreEvent>& event) {
    int playerID = event->getPlayerID();
    int round = event->getRoundNumber();
    int score = event->getScore();

    _playerRoundScores[playerID][round] = score;
    _playerTotalScores[playerID] += score;
    CULog("Processed ScoreEvent: PlayerID = %d, Round = %d, Score = %d, Total Score = %d\n",
        playerID, round, score, _playerTotalScores[playerID]);
}

int ScoreController::getTotalScore(int playerID) const {
    auto it = _playerTotalScores.find(playerID);
    return (it != _playerTotalScores.end()) ? it->second : 0;
}

int ScoreController::getRoundScore(int playerID, int round) const {
    auto playerIt = _playerRoundScores.find(playerID);
    if (playerIt != _playerRoundScores.end()) {
        auto roundIt = playerIt->second.find(round);
        if (roundIt != playerIt->second.end()) {
            return roundIt->second;
        }
    }
    return 0;
}

void ScoreController::sendScoreEvent(const std::shared_ptr<NetEventController>& network, int playerID, ScoreEvent::ScoreType type, int roundNum) {
    auto event = std::dynamic_pointer_cast<ScoreEvent>(
        ScoreEvent::allocScoreEvent(playerID, type, roundNum)
    );
    network->pushOutEvent(event);
    CULog("ScoreEvent sent: PlayerID = %d, Type = %d, Round = %d\n", playerID, static_cast<int>(type), roundNum);
}

/**
    Iterates through all players and checks for a win.
 */
bool ScoreController::checkWinCondition(){
    for (const auto& pair : _playerTotalScores){
        if (pair.second >= WIN_SCORE){
            return true;
        }
    }
    return false;
}

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
void ScoreController::preUpdate(float dt){
}

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
void ScoreController::fixedUpdate(float step){
    
}

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
void ScoreController::postUpdate(float remain){
    
}
