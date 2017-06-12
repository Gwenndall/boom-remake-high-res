#include "BulletFactory.hpp"
#include "utils.hpp"
#include "collision_layers.hpp"

using lif::BulletFactory;

// Regular bullets' info
static std::map<unsigned, const lif::BulletInfo> regBulletsInfo = {
	// infoId, {dataId, damage, speed, range in pixels, collisionLayer}
	/// AxisBullets
	// shot
	{ 1, { 1, 1, 1.3 } },
	// fireball
	{ 2, { 2, 2, 0.9 } },
	// MG shot
	{ 3, { 3, 2, 1.3 } },
	// lightbolt
	{ 4, { 4, 3, 0.9 } },
	// flame
	{ 5, { 5, 3, 0.66, 4 * lif::TILE_SIZE } },
	// plasma
	{ 6, { 6, 3, 1.3 } },
	// magma
	{ 7, { 7, 4, 1.3 } },
	/// FreeBullets
	// star
	{ 101, { 101, 4, 1.1, -1, lif::c_layers::BOSS_BULLETS } },
	// spirit bullets
	{ 102, { 102, 4, 1, -1, lif::c_layers::BOSS_BULLETS } }
};

std::unique_ptr<lif::AxisBullet> BulletFactory::create(unsigned infoId, const sf::Vector2f& pos,
		lif::Direction dir, const lif::Entity *const source)
{
	switch (infoId) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		return std::make_unique<lif::AxisBullet>(pos, dir, regBulletsInfo[infoId], source);
	default:
		throw std::invalid_argument("No AxisBullet with id " + lif::to_string(infoId) + "!");
	}
	return nullptr;
}

std::unique_ptr<lif::FreeBullet> BulletFactory::create(unsigned infoId, const sf::Vector2f& pos,
		lif::Angle angle, const lif::Entity *const source)
{
	switch (infoId) {
	case 101:
	case 102:
		return std::make_unique<lif::FreeBullet>(pos, angle, regBulletsInfo[infoId], source);
	default:
		throw std::invalid_argument("No FreeBullet with id " + lif::to_string(infoId) + "!");
	}
	return nullptr;
}
