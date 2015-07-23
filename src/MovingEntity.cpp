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

MovingEntity::MovingEntity(const sf::Vector2f& pos, const std::string& texture_name) 
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

	float spd = speed;
	if (speedyTime > 0 && speedyClock.getElapsedTime().asMilliseconds() < speedyTime)
		spd *= 2;

	sf::Vector2f shift(0.f, 0.f);
	sf::Time frameTime = frameClock.restart();

	Animation *anim;
		 
	switch (direction) {
	case Direction::UP:
		anim = &animations[ANIM_UP];
		shift.y -= spd;
		break;
	case Direction::LEFT:
		anim = &animations[ANIM_LEFT];
		shift.x -= spd;
		break;
	case Direction::DOWN:
		anim = &animations[ANIM_DOWN];
		shift.y += spd;
		break;
	case Direction::RIGHT:
		anim = &animations[ANIM_RIGHT];
		shift.x += spd;
		break;
	case Direction::NONE:
		return;
	}

        animatedSprite.play(*anim);
	if (!colliding) {
		animatedSprite.move(shift * frameTime.asSeconds());
		distTravelled += spd * frameTime.asSeconds();
		pos = animatedSprite.getPosition();
		_ensureAlign();
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
		pos = Game::aligned(pos);
		break;
	}
	animatedSprite.setPosition(pos);
}

bool MovingEntity::canGo(const Direction dir, const Game::LevelRenderer *const lr) const {
	short iposx = (short)(pos.x / TILE_SIZE) - 1,
	      iposy = (short)(pos.y / TILE_SIZE) - 1;
	
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
	if (iposx < 0 || iposx >= LEVEL_WIDTH || iposy < 0 || iposy >= LEVEL_HEIGHT)
		return false;

	const short idx = iposy * LEVEL_WIDTH + iposx;
	const auto fixed = lr->getFixedEntities();
	
	if (fixed[idx] != nullptr && !_isTransparentTo(fixed[idx]))
		return false;

	const auto bosses = lr->getBosses();
	const sf::FloatRect r(iposx, iposy, TILE_SIZE, TILE_SIZE);
	for (auto& boss : bosses)
		if (boss->intersects(r)) return false;

	return true;
}

void MovingEntity::setDirection(const Direction dir) {
	if (dir == direction) return;
	switch (dir) {
	case Direction::UP: case Direction::DOWN:
		pos.x = (unsigned short)pos.x;
		break;
	case Direction::LEFT: case Direction::RIGHT:
		pos.y = (unsigned short)pos.y;
		break;
	case Direction::NONE: break;
	}
	animatedSprite.setPosition(pos);
	direction = dir;
}

void MovingEntity::setHurt(const bool b) {
	hurt = b;
	hurtAnimPrepared = false;
}

void MovingEntity::prepareHurtAnimation() {
	if (!hurtAnimPrepared) {
		animatedSprite.play(*&animations[ANIM_HURT]);
		animatedSprite.setLooped(false);
		hurtAnimPrepared = true;
	}
}

bool MovingEntity::playHurtAnimation() {
	animatedSprite.update(frameClock.restart());
	return animatedSprite.isPlaying();
}

void MovingEntity::kill() {
	if (!dead) {
		dead = true;
		deathAnimPrepared = false;
		--remainingLives;
	}
}

void MovingEntity::prepareDeathAnimation() {
	if (!deathAnimPrepared) {
		animatedSprite.play(*&animations[ANIM_DEATH]);
		animatedSprite.setLooped(true);
		deathClock.restart();
		deathAnimPrepared = true;
	}
}

bool MovingEntity::playDeathAnimation() {
	animatedSprite.update(frameClock.restart());
	return deathClock.getElapsedTime().asMilliseconds() > DEATH_TIME;
}

void MovingEntity::draw(sf::RenderTarget& window) {
	if (hasShield()) {
		const float s = shieldClock.getElapsedTime().asSeconds();
		const float diff = s - std::floor(s);
		if (shieldTime - 1000*s > 3000 || 4*diff - std::floor(4*diff) < 0.5) {
			animatedSprite.setColor(sf::Color(50, 255, 0, 255));
		} else {
			animatedSprite.setColor(sf::Color::White);
		}
	} else if (animatedSprite.getColor() != sf::Color::White) {
		animatedSprite.setColor(sf::Color::White);
	}
	Game::Animated::draw(window);
}

void MovingEntity::_ensureAlign() {
	// Ensure we are always aligned at least for one frame for
	// each tile we step in (this may not be the case if FPS are too low)
	switch (direction) {
	case Direction::RIGHT:
	case Direction::DOWN:
		if (Game::tile(pos) != prevAlign)
			pos = Game::aligned(pos);
		break;
	case Direction::LEFT:
		if (Game::tile(pos).x == prevAlign.x - 2)
			pos = Game::aligned(pos) + sf::Vector2f(Game::TILE_SIZE, 0);
		break;
	case Direction::UP:
		if (Game::tile(pos).y == prevAlign.y - 2)
			pos = Game::aligned(pos) + sf::Vector2f(0, Game::TILE_SIZE);
		break;
	case Direction::NONE:
		break;
	}
}
