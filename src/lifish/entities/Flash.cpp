#include "Flash.hpp"
#include "game.hpp"
#include "Drawable.hpp"
#include "Temporary.hpp"
#include "Animated.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"

using lif::Flash;
using lif::TILE_SIZE;

Flash::Flash(const sf::Vector2f& pos) 
	: lif::Entity(pos)
{
	animated = addComponent(new lif::Animated(*this, lif::getAsset("test", "flash.png")));
	addComponent(new lif::Drawable(*this, *animated));
	addComponent(new lif::ZIndexed(*this, lif::Conf::ZIndex::FLASHES));

	animated->addAnimation("flash", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE)
	}, true);

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.10));
	animatedSprite.play();

	addComponent(new lif::Temporary(*this, [&animatedSprite] () {
		return !animatedSprite.isPlaying();
	}));
}
