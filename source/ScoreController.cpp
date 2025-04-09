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
