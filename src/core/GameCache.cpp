#include "GameCache.hpp"
#include "core.hpp"
#include "Options.hpp"
#include <iostream>

using lif::GameCache;

GameCache::GameCache() {
	sounds.reserve(maxParallelSounds);
}

void GameCache::setMaxParallelSounds(std::size_t n) {
	maxParallelSounds = n;
	sounds.reserve(maxParallelSounds);
}

sf::Texture* GameCache::loadTexture(const std::string& texture_name) {
	// Check if image is already in cache
	auto it = textures.find(texture_name);
	if (it != textures.end()) 
		return &it->second;

	// Not in cache: load from file
	auto& txt = textures[texture_name];
	if (!txt.loadFromFile(texture_name)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load texture " 
			<< texture_name << " from file!" << std::endl;
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << texture_name << std::endl;
	}
#endif
	return &txt;
}

bool GameCache::loadSound(sf::Sound& sound, const std::string& sound_name) {
	// Check if sound buffer is already in cache
	auto it = soundBuffers.find(sound_name);
	if (it != soundBuffers.end()) {
		sound.setBuffer(it->second);
		return true;
	}
	// Load from file and update the cache
	auto& buf = soundBuffers[sound_name];
	if (!buf.loadFromFile(sound_name)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load sound " << sound_name << " from file!" << std::endl;
		return false;
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << sound_name << std::endl;
	}
#endif
	sound.setBuffer(buf);
	return true;
}

void GameCache::playSound(const std::string& sound_name) {
	if (lif::options.soundsMute) return;

	unsigned idx = 0;
	do {
		if (idx == sounds.size()) {
			// Sounds queue has less than `maxParallelSounds` sounds
			sounds.push_back(sf::Sound());
			break;
		}
		if (sounds[idx].getStatus() != sf::Sound::Status::Playing)
			break;
		++idx;
	} while (idx < maxParallelSounds);

	// No empty slot left
	if (idx == maxParallelSounds) return;

	// Replace this dead sound with the new one
	auto& sound = sounds[idx];
	if (!loadSound(sound, sound_name))
		return;

	sound.setVolume(lif::options.soundsVolume);
	sound.play();
}

sf::Font* GameCache::loadFont(const std::string& font_name) {
	auto it = fonts.find(font_name);
	if (it != fonts.end())
		return &it->second;

	// Load from file and update the cache
	auto& font = fonts[font_name];
	if (!font.loadFromFile(font_name)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load font " 
			<< font_name << " from file!" << std::endl;
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << font_name << std::endl;
	}
#endif
	return &font;
}

void GameCache::finalize() {
	textures.clear();
	sounds.clear();
	soundBuffers.clear();
	fonts.clear();
}
