#include "MovingEntity.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "LevelRenderer.hpp"

using Game::MovingEntity;
using Game::TILE_SIZE;
using Game::Direction;

std::ostream& Game::operator<<(std::ostream& stream, const Direction& dir) {
	switch (dir) {
	case Direction::UP: stream << "UP"; break;
	case Direction::LEFT: stream << "LEFT"; break;
	case Direction::DOWN: stream << "DOWN"; break;
	case Direction::RIGHT: stream << "RIGHT"; break;
	default: stream << "NONE"; break;
	}
	return stream;
}

MovingEntity::MovingEntity(sf::Vector2f pos, const std::string& texture_name) 
	: Entity(pos, texture_name)
{
	transparentTo.bullets = false;
}

void MovingEntity::move() {
	move(direction);
}

void MovingEntity::move(const Direction dir) {
	moving = true;
	direction = dir;

	sf::Vector2f shift(0.f, 0.f);
	sf::Time frameTime = frameClock.restart();

	Animation *anim;
		 
	switch (direction) {
	case Direction::UP:
		anim = &animations[ANIM_UP];
		shift.y -= speed;
		break;
	case Direction::LEFT:
		anim = &animations[ANIM_LEFT];
		shift.x -= speed;
		break;
	case Direction::DOWN:
		anim = &animations[ANIM_DOWN];
		shift.y += speed;
		break;
	case Direction::RIGHT:
		anim = &animations[ANIM_RIGHT];
		shift.x += speed;
		break;
	case Direction::NONE:
		return;
	}

        animatedSprite.play(*anim);
	if (!colliding) {
		animatedSprite.move(shift * frameTime.asSeconds());
		pos = animatedSprite.getPosition();
	} 
	animatedSprite.update(frameTime);
}

void MovingEntity::stop() {
	animatedSprite.setAnimation(animations[ANIM_DOWN]);
	animatedSprite.stop();
	animatedSprite.update(frameClock.restart());
	moving = false;
	direction = prevDirection = Game::Direction::NONE;
	realign();
}

void MovingEntity::realign() {
	switch (direction) {
	case Game::Direction::UP:
		pos = sf::Vector2f(pos.x, ((unsigned short)((pos.y-1) / TILE_SIZE) + 1) * TILE_SIZE);
		break;
	case Game::Direction::LEFT:
		pos = sf::Vector2f(((unsigned short)((pos.x-1) / TILE_SIZE) + 1) * TILE_SIZE, pos.y);
		break;
	case Game::Direction::DOWN:
		pos = sf::Vector2f(pos.x, (unsigned short)(pos.y / TILE_SIZE) * TILE_SIZE);

		break;
	case Game::Direction::RIGHT:
		pos = sf::Vector2f((unsigned short)(pos.x / TILE_SIZE) * TILE_SIZE, pos.y);
		break;
	default: 
		pos = sf::Vector2f((unsigned short)(pos.x / TILE_SIZE) * TILE_SIZE, (unsigned short)(pos.y / TILE_SIZE) * TILE_SIZE);
		break;
	}
	animatedSprite.setPosition(pos);
}

bool MovingEntity::canGo(const Direction dir, const Game::LevelRenderer *const lr) const {
	unsigned short iposx = (unsigned short)(pos.x / TILE_SIZE),
		       iposy = (unsigned short)(pos.y / TILE_SIZE);
	
	switch (dir) {
	case Direction::UP:
		--iposy;
		break;
	case Direction::LEFT:
		--iposx;
		break;
	case Direction::DOWN:
		++iposy;
		break;
	case Direction::RIGHT:
		++iposx;
		break;
	default: return true;
	}
	unsigned short idx = iposy * LEVEL_WIDTH + iposx;
	auto fixed = lr->getFixedEntities();
	if (idx < 0 || idx >= fixed.size()) {
		return false;
	}
	return fixed[idx] == nullptr;
}