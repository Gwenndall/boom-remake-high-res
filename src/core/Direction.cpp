#include "Direction.hpp"

using lif::Direction;

static Direction directions[] = {
	Direction::UP,
	Direction::RIGHT,
	Direction::DOWN,
	Direction::LEFT
};

Direction lif::turnRight(const Direction dir, short times) {
	if (dir == Direction::NONE) return Direction::NONE;

	int i = 0;
	for ( ; i < 4; ++i)
		if (directions[i] == dir) break;
	
	return directions[(4 + (i + times) % 4) % 4];
}

Direction lif::getDirection(const sf::Vector2i& from, const sf::Vector2i& to) {
	if (from.x == to.x)
		return from.y < to.y ? Direction::UP : from.y == to.y ? Direction::NONE : Direction::DOWN;
	else if (from.y == to.y)
		return from.x < to.x ? Direction::LEFT : from.x == to.x ? Direction::NONE : Direction::RIGHT;
	else
		return Direction::NONE;
}
