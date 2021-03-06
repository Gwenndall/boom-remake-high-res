#include "SimpleCollisionDetector.hpp"
#include "Direction.hpp"
#include "AxisMoving.hpp"
#include "EntityGroup.hpp"
#include "collision_layers.hpp"
#include "collision_utils.hpp"
#include <algorithm>

using namespace lif::collision_utils;
using lif::Direction;
using lif::SimpleCollisionDetector;
using lif::TILE_SIZE;

//////// SimpleCollisionDetector methods ////////

SimpleCollisionDetector::SimpleCollisionDetector(lif::EntityGroup& group, const sf::FloatRect& limit)
	: lif::CollisionDetector(group, limit)
{}

void SimpleCollisionDetector::update() {
	/* For each moving entity, check (towards its direction):
	 * 1) has it reached the level boundaries?
	 * 2) is there another non-trasparent entity occupying the cell ahead?
	 */
	auto& colliding = group.getColliding();
	for (auto& cld : colliding) {
		// No need to check for expired, as EntityGroup prunes them before we're called
		auto collider = cld.lock();
		// reset collider
		collider->reset();
		collider->setAtLimit(false);
	}

#ifndef RELEASE
	// Total time taken
	dbgStats.timer.start("tot");
	// Time taken by all narrow checks
	dbgStats.timer.set("tot_narrow", 0);
	// Number of narrow-checked entities
	dbgStats.counter.reset("checked");
#endif

	// Collision detection loop
	for (auto it = colliding.begin(); it != colliding.end(); ++it) {
		auto collider = it->lock();

		// Fixed entities only collide passively
		if (collider->getOwner().get<lif::Fixed>() != nullptr)
			continue;

		const auto moving = collider->getOwner().get<lif::Moving>();
		const auto axismoving = moving ? dynamic_cast<lif::AxisMoving*>(moving) : nullptr;
		if (moving && isAtBoundaries(*collider, axismoving, levelLimit)) {
			collider->setAtLimit(true);
			continue;
		}

		// Very simple (aka quadratic) check with all others
		for (auto jt = colliding.begin(); jt != colliding.end(); ++jt) {
			if (it == jt)  continue;

#ifndef RELEASE
			dbgStats.counter.inc("checked");
			dbgStats.timer.start("single");
#endif

			auto othcollider = jt->lock();
			if (axismoving) {
				// Only check entities ahead of this one
				if (!directionIsViable(*collider, *axismoving, *othcollider))
					continue;

				if (collider->collidesWith(*othcollider)
						&& collide(*collider, *othcollider, axismoving->getDirection()))
				{
					//std::cerr << &collider->getOwner() << " colliding with " << &othcollider->getOwner()<<std::endl;
					collider->addColliding(*jt);
					if (collider->requestsForceAck() || othcollider->requestsForceAck()
							|| othcollider->getOwner().get<lif::Moving>() == nullptr)
					{
						// Let the entity know we collided with it.
						// We only do that for non-moving entities to avoid problems with
						// multiple collisions between two moving entities.
						othcollider->addColliding(*it);
						//std::cerr << "[moving] colliding\n";
					}
				}
			} else if (collider->contains(*othcollider) && collider->collidesWith(*othcollider)) {
				//std::cerr << &collider->getOwner() << " colliding with " << &othcollider->getOwner()<<std::endl;
				collider->addColliding(*jt);
				othcollider->addColliding(*it);
			}

#ifndef RELEASE
			dbgStats.timer.set("tot_narrow", dbgStats.timer.get("tot_narrow")
					+ dbgStats.timer.end("single"));
#endif
		}
	}

#ifndef RELEASE
	dbgStats.timer.end("tot");
#endif
}
