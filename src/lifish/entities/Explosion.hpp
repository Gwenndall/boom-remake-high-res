#pragma once

#include <array>
#include <unordered_set>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"

namespace lif {

class LevelManager;
class Collider;
class Player;
class Animated;
class Explosion;
class BufferedSpawner;

/**
 * A bomb's explosion; like in original BOOM, the explosion
 * doesn't propagate in time: rather, it blossoms in all involved
 * tiles at once and has a duration of ~200 ms (framerate: 0.05)
 */
class Explosion : public lif::Entity, public sf::Drawable {
	sf::Texture explosionHTexture,
		    explosionVTexture;

	lif::Animated *explosionC = nullptr, // central explosion
	               *explosionH = nullptr, // horizontal explosion
		       *explosionV = nullptr; // vertical explosion

	/** This is only non-null if explosion is incendiary */
	lif::BufferedSpawner *spawner = nullptr;

	/** Colliders used to check explosion's hits */
	lif::Collider *explColliderH = nullptr,
	               *explColliderV = nullptr;

	/** The radius of this explosion */
	unsigned short radius;
	/** The damage caused by this explosion per tick */
	unsigned short damage;

	/** The tiles involved in this explosion (valid after calling propagate());
	 *  more specifically, this is a 4-element array containing the propagation
	 *  distance in the 4 directions. The array is indexed with ANIM_UP etc
	 */
	std::array<unsigned short, 4> propagation;

	/** The Entity that generated this explosion, if any. Usually, this means the Player who
	 *  dropped the bomb that cause this explosion.
	 */
	const lif::Entity *const sourceEntity;

	/** The set of the entities already damaged by this Explosion.
	 *  Note: we keep const pointers instead of references because
	 *  reference_wrapper is not trivially hashable.
	 */
	std::unordered_set<const lif::Entity*> damagedEntities;


	/** To be called after `propagate()`; sets the correct positions for explosionH/V */
	void _setPropagatedAnims();

public:
	/** If sourcePlayer == nullptr, the explosion wasn't originated by a bomb.
	 *  In this case, whenever points should be given to a player, no points
	 *  are given at all.
	 */
	explicit Explosion(const sf::Vector2f& pos, unsigned short radius, 
			const lif::Entity *const sourceEntity = nullptr,
			bool isIncendiary = false,
			unsigned short damage = 1);

	/** Calculate the tiles this explosion propagates to and fill `propagation`
	 *  (fixed walls and borders stop the explosion). Also kills enemies and
	 *  walls within the explosion. (called once in the Explosion's lifetime).
	 *  @return self
	 */
	lif::Explosion* propagate(lif::LevelManager& lm);

	void draw(sf::RenderTarget& window, sf::RenderStates states) const override;

	const lif::Entity* getSourceEntity() const { return sourceEntity; }

	unsigned short getDamage() const { return damage; }

	/** Use this function to tell this Explosion it damaged `entity`;
	 *  Explosion can be then queried via `hasDamaged(entity)`.
	 *  Useful for interacting with entities that can only take 1 hit from
	 *  an Explosion.
	 */
	void dealDamageTo(const lif::Entity& entity);
	bool hasDamaged(const lif::Entity& entity) const;
};

}
