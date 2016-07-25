#pragma once

#include "Component.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"

namespace Game {

/** 
 * A Component which animates an Animated entity based on its direction.
 */
class MovingAnimator : public Game::Component {
	
	Game::AxisMoving *moving = nullptr;
	Game::Animated *animated = nullptr;
	Game::Direction prev = Game::Direction::NONE;

public:
	explicit MovingAnimator(Game::Entity& owner);
	explicit MovingAnimator(Game::Entity& owner, Game::AxisMoving *m, Game::Animated *a);

	void update() override;
};

}
