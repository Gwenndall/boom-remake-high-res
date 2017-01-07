#include "Bonus.hpp"
#include "Drawable.hpp"
#include "Sounded.hpp"
#include "Sprite.hpp"
#include "Grabbable.hpp"
#include "Clock.hpp"
#include "Player.hpp"
#include "Scored.hpp"
#include "collision_layers.hpp"
#include "Temporary.hpp"
#include "Collider.hpp"
#include "game.hpp"
#include "conf/bonus.hpp"

using lif::Bonus;
using lif::TILE_SIZE;
using lif::Conf::Bonus::EXPIRE_TIME;
	
Bonus::Bonus(const sf::Vector2f& pos, const lif::BonusType type)
	: lif::Entity(pos)
	, type(type)
{
	sprite = addComponent(new lif::Sprite(*this,
				lif::getAsset("graphics", "bonuses.png"), 
				sf::IntRect(
					static_cast<unsigned short>(type) * TILE_SIZE, 
					static_cast<unsigned short>(type) / 10 * TILE_SIZE, 
					TILE_SIZE,
					TILE_SIZE)));
	addComponent(new lif::Collider(*this, [this] (const lif::Collider& cld) {
		if (cld.getLayer() != lif::Layers::PLAYERS || grabbable->isGrabbed())
			return;
		_grab(static_cast<lif::Player&>(cld.getOwnerRW()));
	}, lif::Layers::GRABBABLE));
	addComponent(new lif::Drawable(*this, *sprite));
	addComponent(new lif::Scored(*this, lif::Conf::Bonus::VALUE));
	expireClock = addComponent(new lif::Clock(*this));
	addComponent(new lif::Sounded(*this, { std::make_pair("grab", lif::getAsset("test", "bonus_grab.ogg")) }));
	addComponent(new lif::Temporary(*this, [this] () {
		// expire condition
		return grabbable->isGrabbed() || expireClock->getElapsedTime() > EXPIRE_TIME;
	}));
	grabbable = addComponent(new lif::Grabbable(*this));
}

void Bonus::update() {
	lif::Entity::update();
	const float s = expireClock->getElapsedTime().asSeconds();
	if (EXPIRE_TIME.asSeconds() - s <= 3.) {
		const float diff = s - std::floor(s);
		if (5 * diff - std::floor(5 * diff) < 0.5)
			sprite->getSprite().setColor(sf::Color(0, 0, 0, 0));
		else
			sprite->getSprite().setColor(sf::Color(255, 255, 255, 255));
	}
}

void Bonus::_grab(lif::Player& player) {
	get<lif::Scored>()->setTarget(player.getInfo().id);
	grabbable->setGrabbingEntity(&player);
}
