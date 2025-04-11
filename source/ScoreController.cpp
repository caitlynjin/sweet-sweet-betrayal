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
    ScoreEvent::ScoreType type = event->getScoreType();

    RoundScore rs;
    rs.score = score;
    rs.scoreType = type;

    // Assuming _playerRoundScores is now a mapping from playerID to a mapping of round to RoundScore.
    _playerRoundScores[playerID][round] = rs;
    _playerTotalScores[playerID] += score;
    
    CULog("Processed ScoreEvent: PlayerID = %d, Round = %d, Score = %d (Type: %d), Total Score = %d\n",
          playerID, round, score, static_cast<int>(type), _playerTotalScores[playerID]);
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
            return roundIt->second.score;
        }
    }
    return 0;
}

ScoreEvent::ScoreType ScoreController::getRoundScoreType(int playerID, int round) const {
    auto playerIt = _playerRoundScores.find(playerID);
    if (playerIt != _playerRoundScores.end()) {
        auto roundIt = playerIt->second.find(round);
        if (roundIt != playerIt->second.end()) {
            return roundIt->second.scoreType;
        }
    }
    return ScoreEvent::ScoreType::NONE;
}

void ScoreController::sendScoreEvent(const std::shared_ptr<NetEventController>& network, int playerID, ScoreEvent::ScoreType type, int roundNum) {
    auto event = std::dynamic_pointer_cast<ScoreEvent>(
        ScoreEvent::allocScoreEvent(playerID, type, roundNum)
    );
    network->pushOutEvent(event);
    CULog("ScoreEvent sent: PlayerID = %d, Type = %d, Round = %d\n", playerID, static_cast<int>(type), roundNum);
}
