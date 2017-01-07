#pragma once

#include <array>
#include <memory>
#include "Boss.hpp"
#include "game.hpp"

namespace lif {

class HauntedStatue;
class Animated;
class Clock;
class ShootingPoint;
class ShootingPattern;

class HauntingSpiritBoss : public lif::Boss {
	enum class State {
		START,
		SEARCHING,
		SELECT_NEW_STATUE,
		TRANSITIONING_BEGIN,
		TRANSITIONING_END,
		HAUNTING,
		DYING
	} state;

	std::vector<std::weak_ptr<lif::HauntedStatue>> statues;
	std::weak_ptr<lif::HauntedStatue> targetStatue;
	std::array<lif::ShootingPattern*, 3> shootPatterns;
	lif::ShootingPattern *curShootPattern = nullptr;

	lif::Clock *animClock = nullptr,
	            *atkClock = nullptr,   // used for shooting
		    *hauntClock = nullptr; // used for changing haunted statue after delay

	void _updateStart();
	void _updateSearching();
	void _updateSelectNewStatue();
	void _updateTransitioningBegin();
	void _updateTransitioningEnd();
	void _updateHaunting();
	void _updateDying();
	bool _isShooting() const;
public:
	explicit HauntingSpiritBoss(const sf::Vector2f& pos);

	void update() override;
};

}
