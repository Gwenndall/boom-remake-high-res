#pragma once

#include "Component.hpp"
#include "Track.hpp"
#include "LoopingMusic.hpp"

namespace Game {

class Music : public Game::Component {
	/** The input sound file for the BGM */
	sf::InputSoundFile musicInput;

	/** The music for this level */
	std::unique_ptr<LoopingMusic> music;

	/** The music track data */
	Game::Track track;

public:
	explicit Music(Game::Entity& owner, const Game::Track& track);
	
	LoopingMusic* getMusic() const { return music.get(); }
	const Game::Track& getTrack() const { return track; }
};

}
