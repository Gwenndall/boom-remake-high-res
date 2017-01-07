#include "PreferencesScreen.hpp"
#include "game.hpp"
#include "MusicManager.hpp"
#include "Interactable.hpp"
#include "Options.hpp"
#include "ShadedText.hpp"
#include "GameCache.hpp"
#include "utils.hpp"
#include <memory>
#include <iostream>

using lif::UI::PreferencesScreen;
using lif::UI::Interactable;
using Action = lif::UI::Action;

PreferencesScreen::PreferencesScreen(const sf::RenderWindow& window, const sf::Vector2u& sz) 
	: lif::UI::Screen(window, sz) 
{
	name = "preferences";
	parent = "home";
	_loadBGSprite(lif::getAsset("graphics", "screenbg1.png"));

	/* Layout:
	 * 
	 * MUSIC: - ||||||| + (m)
	 * FX:    - ||||||| + (m)
	 * Controls
	 *
	 * Exit
	 */
	const auto font = lif::getAsset("fonts", lif::Fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, lif::WINDOW_WIDTH, lif::WINDOW_HEIGHT);

	const auto size = 24;
	const float ipadx = 25,
		    ipady = 15;
	auto text = new lif::ShadedText(font, "Music:", sf::Vector2f(ipadx, ipady));
	text->setCharacterSize(size);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	// Music volume
	auto pos = text->getPosition();

	text = new lif::ShadedText(font, "-", sf::Vector2f(ipadx + 150, ipady - 8));
	text->setCharacterSize(32);
	interactables["music_volume_down"] = std::unique_ptr<Interactable>(new Interactable(text));

	text = new lif::ShadedText(font, "placeholder", sf::Vector2f(ipadx + 200, ipady));
	// Draw the full volume bar to get the measure of this element's max width
	// (also, the volume is maxed by default, so we don't need to do any further checks here)
	std::stringstream ss;
	for (unsigned short i = 0; i < MAX_VOLUME; ++i) {
		ss << "|";	
	}
	text->setCharacterSize(20);
	text->setString(ss.str());
	musicVolumeBar = text;
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	auto bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, "+", sf::Vector2f(ipadx + 200 + bounds.width + 40, ipady - 6));
	text->setCharacterSize(30);
	interactables["music_volume_up"] = std::unique_ptr<Interactable>(new Interactable(text));

	bounds = text->getGlobalBounds();
	auto image = new sf::Sprite;
	speakerTexture = lif::cache.loadTexture(lif::getAsset("graphics", "speaker.png"));
	image->setTexture(*speakerTexture);
	image->setTextureRect(sf::IntRect(prevMusicVolume >= 0 ? 25 : 0, 0, 25, 25));
	image->setPosition(sf::Vector2f(bounds.left + bounds.width + 20, ipady));
	interactables["music_mute_toggle"] = std::unique_ptr<Interactable>(new Interactable(image));

	// FX Volume
	text = new lif::ShadedText(font, "FX:", sf::Vector2f(ipadx, ipady + bounds.height + 20));
	text->setCharacterSize(size);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	pos = text->getPosition();
	text = new lif::ShadedText(font, "-", sf::Vector2f(ipadx + 150, pos.y - 8));
	text->setCharacterSize(32);
	interactables["sounds_volume_down"] = std::unique_ptr<Interactable>(new Interactable(text));

	text = new lif::ShadedText(font, ss.str(), sf::Vector2f(ipadx + 200, pos.y));
	text->setCharacterSize(20);
	soundsVolumeBar = text;
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));

	bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, "+", sf::Vector2f(ipadx + 200 + bounds.width + 40, pos.y - 6));
	text->setCharacterSize(30);
	interactables["sounds_volume_up"] = std::unique_ptr<Interactable>(new Interactable(text));

	bounds = text->getGlobalBounds();
	image = new sf::Sprite;
	speakerTexture = lif::cache.loadTexture(lif::getAsset("graphics", "speaker.png"));
	image->setTexture(*speakerTexture);
	image->setTextureRect(sf::IntRect(lif::options.soundsMute ? SPEAKER_SPRITE_SIZE : 0,
				0, SPEAKER_SPRITE_SIZE, SPEAKER_SPRITE_SIZE));
	image->setPosition(sf::Vector2f(bounds.left + bounds.width + 20, bounds.top));
	interactables["sounds_mute_toggle"] = std::unique_ptr<Interactable>(new Interactable(image));

	text = new lif::ShadedText(font, "Controls", sf::Vector2f(ipadx, pos.y + bounds.height + 20));
	text->setCharacterSize(size);
	interactables["controls"] = std::unique_ptr<Interactable>(new Interactable(text));
	
	text = new lif::ShadedText(font, "Back", pos);
	text->setCharacterSize(size);
	bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(lif::center(bounds, win_bounds).x, win_bounds.height - 2 * bounds.height));
	interactables["back"] = std::unique_ptr<Interactable>(new Interactable(text));

	// Setup internal callbacks
	callbacks["music_volume_up"] = [this] () { return _changeVolume(VolumeType::MUSIC, VolumeAction::RAISE); };
	callbacks["music_volume_down"] = [this] () { return _changeVolume(VolumeType::MUSIC, VolumeAction::LOWER); };
	callbacks["music_mute_toggle"] = [this] () { return _changeVolume(VolumeType::MUSIC, VolumeAction::MUTE_TOGGLE); };
	callbacks["sounds_volume_up"] = [this] () { return _changeVolume(VolumeType::SOUND, VolumeAction::RAISE); };
	callbacks["sounds_volume_down"] = [this] () { return _changeVolume(VolumeType::SOUND, VolumeAction::LOWER); };
	callbacks["sounds_mute_toggle"] = [this] () { return _changeVolume(VolumeType::SOUND, VolumeAction::MUTE_TOGGLE); };
}

Action PreferencesScreen::_changeVolume(VolumeType which, VolumeAction what) {
	// TODO better mute handling
	if (what == VolumeAction::MUTE_TOGGLE) {
		switch (which) {
		case VolumeType::MUSIC: 
			if (prevMusicVolume < 0) {
				// unmute->mute
				prevMusicVolume = lif::options.musicVolume;
				lif::options.musicVolume = 0;
			} else {
				// mute->unmute
				lif::options.musicVolume = prevMusicVolume;
				prevMusicVolume = -1;
			}
			interactables["music_mute_toggle"]->getSprite()->setTextureRect(
					sf::IntRect(prevMusicVolume >= 0 ? SPEAKER_SPRITE_SIZE : 0,
						0, SPEAKER_SPRITE_SIZE, SPEAKER_SPRITE_SIZE));
			if (lif::musicManager != nullptr)
				lif::musicManager->setVolume(lif::options.musicVolume);
			break;
		case VolumeType::SOUND:
			lif::options.soundsMute = !lif::options.soundsMute;
			interactables["sounds_mute_toggle"]->getSprite()->setTextureRect(
					sf::IntRect(lif::options.soundsMute ? SPEAKER_SPRITE_SIZE : 0,
						0, SPEAKER_SPRITE_SIZE, SPEAKER_SPRITE_SIZE));
			break;
		default: 
			break;
		}

		return Action::DO_NOTHING;
	}
	short &vol = which == VolumeType::MUSIC ? relMusicVolume : relSoundVolume; 
	const bool raise = what == VolumeAction::RAISE;

	if ((raise && vol == MAX_VOLUME) || (!raise && vol == 0))
		return Action::DO_NOTHING;

	vol += (raise ? 1 : -1);

	std::stringstream ss;
	for (unsigned short i = 0; i < vol; ++i) {
		ss << "|";	
	}

	if (which == VolumeType::MUSIC) {
		lif::options.musicVolume = vol * 100 / MAX_VOLUME;
		musicVolumeBar->setString(ss.str());
		if (lif::musicManager != nullptr)
			lif::musicManager->setVolume(lif::options.musicVolume);
	} else {
		lif::options.soundsVolume = vol * 100 / MAX_VOLUME;
		soundsVolumeBar->setString(ss.str());
	}

	return Action::DO_NOTHING;
}
