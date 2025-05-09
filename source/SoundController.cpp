//
//  SoundController.cpp
//  SweetSweetBetrayal
//
//  Created by Benjamin Isaacson on 3/14/25.
//

#include "SoundController.h"

using namespace cugl;
using namespace cugl::audio;

SoundController::SoundController() {}
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
bool SoundController::init(const std::shared_ptr<cugl::AssetManager>& assets) {

	std::vector<std::string> musicTracks = {
		"move_phase"
	};
	std::vector<std::string> soundNames = {
		"glide", 
		"button_click",
		"jump",
		"mushroom_boing",
		"placeItem",
		"yay",
		"heeheehee",
		"heehee",
		"takethat",
		"ow",
		"aw",
		"placeItem",
		"bomb",
		"redSelect",
		"blueSelect",
		"yellowSelect",
		"greenSelect",
		"timer",
		"discardItem"
	};
	std::string name;
	std::shared_ptr<Sound> sound;
	for (auto it = soundNames.begin(); it != soundNames.end(); ++it) {
		name = *it;
		sound = assets->get<Sound>(name);
		_sounds.push_back(sound);
		_soundMap.emplace(name, sound);
		_soundOriginalVolumeMap.emplace(name, sound->getVolume());
	}
	for (auto it = musicTracks.begin(); it != musicTracks.end(); ++it) {
		name = *it;
		sound = assets->get<Sound>(name);
		_sounds.push_back(sound);
		_musicMap.emplace(name, sound);
		_musicOriginalVolumeMap.emplace(name, sound->getVolume());
	}
	_musicQueue = AudioEngine::get()->getMusicQueue();
	return true;
}

/**
	* Plays the sound with the specified key.
	*
	* @param key The key identifying the sound effect */
void SoundController::playSound(std::string key) {
	AudioEngine::get()->play(key, _soundMap[key]);
}

/**
* Plays the music track with the specified key.
*
* @param key The key identifying the music track */

// Make sure this actually works the way it's supposed to
void SoundController::playMusic(std::string key, bool loop) {
	_musicQueue->enqueue(_musicMap[key], loop);
}

void SoundController::addMusicToQueue(std::string key) {
	_musicQueue->enqueue(_musicMap[key]);
}

/**
* Stops the sound with the specified key.
*
* @param key The key identifying the sound effect */
void SoundController::stopSound(std::string key) {
	AudioEngine::get()->pause(key);
}

/**
* Stops the music track with the specified key.
*
* @param key The key identifying the music track */
void SoundController::stopMusic(std::string key) {
	_musicQueue->pause();
}

void SoundController::setMusicVolume(float vol) {
	_musicVolume = vol;
	for (auto it = _musicMap.begin(); it != _musicMap.end(); ++it) {
		it->second->setVolume(_musicOriginalVolumeMap[it->first] * vol);
	}
}

void SoundController::setSFXVolume(float vol) {
	_sfxVolume = vol;
	for (auto it = _soundMap.begin(); it != _soundMap.end(); ++it) {
		it->second->setVolume(_soundOriginalVolumeMap[it->first] * vol);
	}
	
}

void SoundController::dispose() {}