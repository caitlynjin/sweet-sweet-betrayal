////
////  TransitionScene.cpp
////  SweetSweetBetrayal
////
////  Created by Angelica Borowy on 4/29/25.
////
//
//#include <stdio.h>
//
//#include "TransitionScene.h"
//#include "Constants.h"
//
//#include <ctime>
//#include <string>
//#include <iostream>
//#include <sstream>
//#include <random>
//
//using namespace cugl;
//using namespace cugl::graphics;
//using namespace cugl::physics2;
//using namespace cugl::audio;
//using namespace Constants;
//
//#pragma mark -
//#pragma mark Scene Constants
//
///** This is adjusted by screen aspect ratio to get the height */
//#define SCENE_WIDTH 1024
//#define SCENE_HEIGHT 576
//
//
//
//#pragma mark -
//#pragma mark Constructors
///**
// * Creates a new game world with the default values.
// *
// * This constructor does not allocate any objects or start the controller.
// * This allows us to use a controller without a heap pointer.
// */
//TransitionScene::TransitionScene() : Scene2() {}
//
///**
// * Disposes of all (non-static) resources allocated to this mode.
// */
//void TransitionScene::dispose() {
//    if (_active)
//    {
//        //TODO: Set class variables to nullptr
//
//        Scene2::dispose();
//    }
//};
//
///**
// * Initializes the scene contents
// *
// * @param assets    The (loaded) assets for this game mode
// *
// * @return true if the controller is initialized properly, false otherwise.
// */
//bool TransitionScene::init(const std::shared_ptr<AssetManager>& assets) {
//    if (assets == nullptr)
//    {
//        return false;
//    }
//    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT)))
//    {
//        return false;
//    }
//
//    _assets = assets;
//    
//
//    return true;
//}
