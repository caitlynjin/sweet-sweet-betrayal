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

	/** The map linking each string key to its corresponding sound */
	std::map<std::string, std::shared_ptr<cugl::audio::Sound>> _soundMap;

	std::shared_ptr<Sound> _jumpSound;


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
	void playMusic(std::string key);

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

};



#endif /* SoundController_h */