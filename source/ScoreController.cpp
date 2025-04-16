//
//  ScoreController.cpp
//  SweetSweetBetrayal
//
//  Created by jessie jia on 4/9/25.
//

#include "ScoreController.h"

using namespace cugl::graphics;

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
    ScoreEvent::ScoreType type = event->getScoreType();
    
    //update total score
    int prevTotal = _playerTotalScores[playerID];
    _playerTotalScores[playerID] += score;
    int newTotal = _playerTotalScores[playerID];
        
    //update round score
    RoundScore rs;
    rs.score = score;
    rs.scoreType = type;
    _playerRoundScores[playerID][round] = rs;
    
    CULog("Processed ScoreEvent: PlayerID = %d, Round = %d, Score = %d (Type: %d), Total Score = %d\n",
          playerID, round, score, static_cast<int>(type), _playerTotalScores[playerID]);
    
    //update scoreboardUI
    auto it = _playerColors.find(playerID);
    ColorType color = it->second;
    std::string playerName = colorToString(color);
    
    std::string iconTextureKey;
    if (type == ScoreEvent::ScoreType::END_TREASURE) {
        iconTextureKey = "score-treasure";        
    }
    else if (type == ScoreEvent::ScoreType::END) {
        iconTextureKey = "score-finish";
    }
    auto newTexture = _assets->get<Texture>(iconTextureKey);
    
    for (int i = prevTotal; i < newTotal; i++) {
        
        Vec2 basePos = _playerBaseDotPos[playerName];
        Vec2 overlayPos = basePos + offset_betw_points * static_cast<float>(i);
        auto newIconNode = createIcon(iconTextureKey, 1.0f, overlayPos, _anchor, false);
        _scoreboardParent->addChild(newIconNode);
        _scoreIcons[playerName + "-" + iconTextureKey + "-" + std::to_string(i)] = newIconNode;
        CULog(" -> Added icon '%s' for %s at position (%.1f, %.1f)\n",
                      iconTextureKey.c_str(), playerName.c_str(), overlayPos.x, overlayPos.y);
    }
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

/**
    Iterates through all players and checks for a win.
 */
bool ScoreController::checkWinCondition(){
    for (const auto& pair : _playerTotalScores){
        if (pair.second >= WIN_SCORE){
            CULog("PLAYER WON");
            return true;
        }
    }
    return false;
}

void ScoreController::initScoreboardNodes(cugl::scene2::Scene2* parent, const Vec2 &anchor,
                                          std::vector<std::shared_ptr<PlayerModel>> playerList,
                                          float size_width, float size_height) {
    _anchor = anchor;
    _scoreboardParent = parent;

    float scale = 1.0f;
    bar_position = Vec2(size_width * 0.5f, size_height * 0.8f);
    glider_position = Vec2(size_width * 0.25f, size_height * 0.8f);
    offset_betw_points = Vec2(size_width * 0.05f, 0);
    offset_betw_players = Vec2(0, -size_height * 0.2f);

    _playerList = playerList;

    for (auto& player : _playerList) {
        std::string name = player->getName();

        if (name == "playerRed") {
            CULog("Red player detected");
            _scoreIcons["red-bar"] = createIcon("red-bar", scale, bar_position, anchor, false);
            parent->addChild(_scoreIcons["red-bar"]);
            _scoreIcons["redglider"] = createIcon("redglider", scale, glider_position, anchor, false);
            parent->addChild(_scoreIcons["redglider"]);
            float glider_width = _scoreIcons["redglider"]->getContentSize().width;
            Vec2 base_dot_position = glider_position + Vec2(glider_width, 0);
            _playerBaseDotPos[name] = base_dot_position;


            for (int i = 0; i < 10; ++i) {
                std::string dot_key = name + "-dot-" + std::to_string(i);
                Vec2 dot_pos = base_dot_position + offset_betw_points * static_cast<float>(i);
                _scoreIcons[dot_key] = createIcon("dot", scale, dot_pos, anchor, false);
                parent->addChild(_scoreIcons[dot_key]);
            }
        }
        else if (name == "playerBlue") {
            CULog("Blue player detected");
            _scoreIcons["blue-bar"] = createIcon("blue-bar", scale, bar_position, anchor, false);
            parent->addChild(_scoreIcons["blue-bar"]);
            _scoreIcons["blueglider"] = createIcon("blueglider", scale, glider_position, anchor, false);
            parent->addChild(_scoreIcons["blueglider"]);
            float glider_width = _scoreIcons["blueglider"]->getContentSize().width;
            Vec2 base_dot_position = glider_position + Vec2(glider_width, 0);
            _playerBaseDotPos[name] = base_dot_position;

            for (int i = 0; i < 10; ++i) {
                std::string dot_key = name + "-dot-" + std::to_string(i);
                Vec2 dot_pos = base_dot_position + offset_betw_points * static_cast<float>(i);
                _scoreIcons[dot_key] = createIcon("dot", scale, dot_pos, anchor, false);
                parent->addChild(_scoreIcons[dot_key]);
            }
        }
        else if (name == "playerGreen") {
            CULog("Green player detected");
            _scoreIcons["green-bar"] = createIcon("green-bar", scale, bar_position, anchor, false);
            parent->addChild(_scoreIcons["green-bar"]);
            _scoreIcons["greenglider"] = createIcon("greenglider", scale, glider_position, anchor, false);
            parent->addChild(_scoreIcons["greenglider"]);
            float glider_width = _scoreIcons["greenglider"]->getContentSize().width;
            Vec2 base_dot_position = glider_position + Vec2(glider_width, 0);
            _playerBaseDotPos[name] = base_dot_position;

            for (int i = 0; i < 10; ++i) {
                std::string dot_key = name + "-dot-" + std::to_string(i);
                Vec2 dot_pos = base_dot_position + offset_betw_points * static_cast<float>(i);
                _scoreIcons[dot_key] = createIcon("dot", scale, dot_pos, anchor, false);
                parent->addChild(_scoreIcons[dot_key]);
            }
        }
        else if (name == "playerYellow") {
            CULog("Yellow player detected");
            _scoreIcons["yellow-bar"] = createIcon("yellow-bar", scale, bar_position, anchor, false);
            parent->addChild(_scoreIcons["yellow-bar"]);
            _scoreIcons["yellowglider"] = createIcon("yellowglider", scale, glider_position, anchor, false);
            parent->addChild(_scoreIcons["yellowglider"]);
            float glider_width = _scoreIcons["yellowglider"]->getContentSize().width;
            Vec2 base_dot_position = glider_position + Vec2(glider_width, 0);
            _playerBaseDotPos[name] = base_dot_position;

            for (int i = 0; i < 10; ++i) {
                std::string dot_key = name + "-dot-" + std::to_string(i);
                Vec2 dot_pos = base_dot_position + offset_betw_points * static_cast<float>(i);
                _scoreIcons[dot_key] = createIcon("dot", scale, dot_pos, anchor, false);
                parent->addChild(_scoreIcons[dot_key]);
            }
        }

        bar_position += offset_betw_players;
        glider_position += offset_betw_players;
    }
}

//    _scoreIcons["score-finish"] = createIcon("score-finish", scale, basePos + Vec2(7 * spacing, 0), anchor, true);
//    parent->addChild(_scoreIcons["score-finish"]);
//
//    _scoreIcons["score-trapkill"] = createIcon("score-trapkill", scale, basePos + Vec2(8 * spacing, 0), anchor, true);
//    parent->addChild(_scoreIcons["score-trapkill"]);
//
//    _scoreIcons["score-treasure"] = createIcon("score-treasure", scale, basePos + Vec2(9 * spacing, 0), anchor, true);
//    parent->addChild(_scoreIcons["score-treasure"]);
//
//    _scoreIcons["scoreboard-background"] = createIcon("scoreboard-background", 0.2f, basePos + Vec2(10 * spacing, 0), anchor, true);
//    parent->addChild(_scoreIcons["scoreboard-background"]);



std::shared_ptr<scene2::PolygonNode> ScoreController::createIcon(const std::string& textureKey,
                                                                   float scale,
                                                                   const Vec2& position,
                                                                   const Vec2 &anchor,
                                                                   bool visible) {
    auto tex = _assets->get<Texture>(textureKey);
    auto node = scene2::PolygonNode::allocWithTexture(tex);
    node->setAnchor(anchor);
    node->setScale(scale);
    node->setPosition(position);
    node->setVisible(visible);
    return node;
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
