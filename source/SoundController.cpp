//
//  SoundController.cpp
//  SweetSweetBetrayal
//
//  Created by Benjamin Isaacson on 3/14/25.
//

#include "SoundController.h"

#define CROSS_FADE 0.5f

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
		"move_phase",
		"main_menu",
		"waiting_scene",
		"victory"
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
		"discardItem",
		"numpad_1",
		"numpad_2",
		"numpad_3",
		"numpad_4",
		"numpad_5",
		"numpad_6",
		"numpad_7",
		"numpad_8",
		"numpad_9",
		"numpad_0",
		"failed_placement"
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
void SoundController::playMusic(std::string key, bool loop, bool useCrossFade) {
	if (useCrossFade) {
		_musicQueue->enqueue(_musicMap[key], loop, 1.0f, CROSS_FADE);
		_musicQueue->advance(0, CROSS_FADE);
	}
	else {
		_musicQueue->enqueue(_musicMap[key], loop, 1.0f);
		_musicQueue->advance(0);
	}
	
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

void SoundController::setMusicVolume(float vol, bool savePreferences) {
	_musicVolume = vol;
	for (auto it = _musicMap.begin(); it != _musicMap.end(); ++it) {
		it->second->setVolume(_musicOriginalVolumeMap[it->first] * vol);
	}
	_musicQueue->setVolume(vol);
	if (savePreferences) {
		saveAudioPreferences();
	}
    _musicQueue->setVolume(vol);
}

void SoundController::setSFXVolume(float vol, bool savePreferences) {
	_sfxVolume = vol;
	for (auto it = _soundMap.begin(); it != _soundMap.end(); ++it) {
		it->second->setVolume(_soundOriginalVolumeMap[it->first] * vol);
	}
	if (savePreferences) {
		saveAudioPreferences();
	}
	
}

void SoundController::saveAudioPreferences() {
	std::shared_ptr<JsonWriter> jsonWriter = JsonWriter::alloc(Application::get()->getSaveDirectory() + "preferences.json");
	std::shared_ptr<JsonValue> json = JsonValue::allocObject();
	json->appendValue("musicVolume", double(_musicVolume));
	json->appendValue("sfxVolume", double(_sfxVolume));
	jsonWriter->writeJson(json);
	jsonWriter->close();
}

void SoundController::loadAudioPreferences() {
	std::shared_ptr<JsonReader> jsonReader = JsonReader::alloc(Application::get()->getSaveDirectory() + "preferences.json");
	if (jsonReader == nullptr) {
		return;
	}
	std::shared_ptr<JsonValue> json = jsonReader->readJson();

	auto mv = json->get("musicVolume");
	auto sv = json->get("sfxVolume");
	float musicVol;
	if (mv == nullptr) {
		musicVol = 0.8;
	}
	else {
		musicVol = json->get("musicVolume")->asFloat();
	}
	float sfxVol; 
	if (sv == nullptr) {
		sfxVol = 0.8;
	}
	else {
		sfxVol = json->get("sfxVolume")->asFloat();
	}
	
	jsonReader->close();
	setMusicVolume(musicVol, false);
	setSFXVolume(sfxVol, true);

}

void SoundController::dispose() {}