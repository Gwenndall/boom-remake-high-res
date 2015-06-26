#include "BreakableWall.hpp"
#include "Game.hpp"

using Game::BreakableWall;
using Game::TILE_SIZE;

BreakableWall::BreakableWall(sf::Vector2f pos, const std::string& texture_name, unsigned short _life) 
	: Wall(pos, texture_name), life(_life)
{
	// Breakable walls have 4 sprites: the "animatedSprites" array is used
	// to keep these sprites, and the animation starts when the wall is being destroyed.
	animations["destroy"].setSpriteSheet(texture);
	animations["destroy"].addFrame(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	animations["destroy"].addFrame(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations["destroy"].addFrame(sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations["destroy"].addFrame(sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));

	animatedSprite.setAnimation(animations["destroy"]);
	animatedSprite.setLooped(false);
	animatedSprite.pause();
}

void BreakableWall::draw(sf::RenderTarget& window) {
	window.draw(animatedSprite);
}
