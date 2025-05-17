//
//  SoundController.h
//  SweetSweetBetrayal
//
//  Created by Benjamin Isaacson on 3/14/25.
//

#ifndef SoundController_h
#define SoundController_h

#include <cugl/cugl.h>

using namespace cugl;
using namespace cugl::audio;

/**
 * This class manages game audio.
 *
 * This includes music and sound effect playback as well as other audio logic.
 *
 */
class SoundController {
protected:

	/** The audio buffers, used for storing sound effects */
	std::vector<std::shared_ptr<cugl::audio::Sound>> _sounds;

	/** The audio queue, used for storing music */
	std::shared_ptr<cugl::audio::AudioQueue>   _musicQueue;

	/** The global music volume, ranging from 0.0 (silent) to 1.0 (maximum) */
	float _musicVolume;

	/** The global SFX volume, ranging from 0.0 (silent) to 1.0 (maximum) */
	float _sfxVolume;

	/** The map linking each string key to its corresponding sound */
	std::map<std::string, std::shared_ptr<cugl::audio::Sound>> _soundMap;

	/** The map linking each string key to its corresponding music
	*	These are only stored in separate maps because it helps 
	*   with separating settings menu logic for music vs SFX.
	*/ 
	std::map<std::string, std::shared_ptr<cugl::audio::Sound>> _musicMap;



	/** The map linking each sound (represented by its string key) to its original volume.
	* Useful for settings menu calculations.
	*/
	std::map<std::string, float> _soundOriginalVolumeMap;

	/** The map linking each music track (represented by its string key) to its original volume.
	* Useful for settings menu calculations.
	*/
	std::map<std::string, float> _musicOriginalVolumeMap;


public:

	/**
	 * Creates a new network controller with the default values.
	 *
	 * This constructor does not allocate any objects or start the controller.
	 * This allows us to use a controller without a heap pointer.
	 */
	SoundController();

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
	static std::shared_ptr<SoundController> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
		std::shared_ptr<SoundController> result = std::make_shared<SoundController>();
		return (result->init(assets) ? result : nullptr);
	}

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*
	* This method is different from dispose() in that it ALSO shuts off any
	* static resources, like the input controller.
	*/
	~SoundController() { dispose(); }

	/**
	 * Disposes of all (non-static) resources allocated to this mode.
	 */
	void dispose();

	/**
	* Plays the sound with the specified key.
	*
	* @param key The key identifying the sound effect */
	void playSound(std::string key);

	/**
	* Plays the music track with the specified key.
	*
	* @param key The key identifying the music track */
	void playMusic(std::string key, bool loop=false, bool useCrossFade=true);

	std::string getCurrentTrackPlaying() {
		return _musicQueue->current();
	}

	/** Adds the music track to the end of the queue.
	 * This will play automatically when all music tracks earlier in the queue are finished playing. 
	 * @param key The key identifying the music track
	 
	 */
	void addMusicToQueue(std::string key);

	/**
	* Stops the sound with the specified key.
	*
	* @param key The key identifying the sound effect */
	void stopSound(std::string key);

	/**
	* Stops the music track with the specified key.
	*
	* @param key The key identifying the music track */
	void stopMusic(std::string key);

	/** Gets the global music volume. 
	* The volumes of all music tracks are multiplied by this value.
	* 
	* @return the global music volume
	*/
	float getMusicVolume() {
		return _musicVolume;
	}

	/** Gets the global SFX volume.
	* The volumes of all SFX are multiplied by this value.
	* 
	* @return the global SFX volume
	*/
	float getSFXVolume() {
		return _sfxVolume;
	}

	/** Sets the global music volume. 
	* @param vol the global music volume to be set
	* @param savePreferences whether or not the new music volume should be saved to a new JSON file.
	* This should be true when working with audio settings, and false when adjusting in-game volume for other reasons that don't need to be saved as preferences.
	*/
	void setMusicVolume(float vol, bool savePreferences=true);

	/** Sets the global SFX volume.
	* @param vol the global SFX volume to be set
	* @param savePreferences whether or not the new SFX volume should be saved to a new JSON file.
	* This should be true when working with audio settings, and false when adjusting in-game volume for other reasons that don't need to be saved as preferences.
	*/
	void setSFXVolume(float vol, bool savePreferences=true);

	/** Saves the audio preferences to a JSON file called "preferences.json" in the save directory. */
	void saveAudioPreferences();

	/** Loads the audio preferences and also sets the volume of music and SFX to match the user's preferences. */
	void loadAudioPreferences();

};



#endif /* SoundController_h */