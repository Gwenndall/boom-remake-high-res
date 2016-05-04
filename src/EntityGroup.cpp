#include "EntityGroup.hpp"
#include "Component.hpp"
#include <algorithm>

using Game::EntityGroup;

EntityGroup::~EntityGroup() {
	// Don't destroy unowned entities
	for (auto& e : entities) {
		if (unowned.find(e.get()) != unowned.end())
			e.release();
	}
}

void EntityGroup::setOrigin(const sf::Vector2f& origin) {
	WithOrigin::setOrigin(origin);

	for (auto& e : entities)
		e->setOrigin(origin);
}

void EntityGroup::updateAll() {
	for (auto& e : entities)
		e->update();

	_removeExpiredTemporaries();
}

void EntityGroup::_removeExpiredTemporaries() {
	for (auto it = temporary.begin(); it != temporary.end(); ) {
		auto tmp = *it;
		if (tmp->isExpired()) {
			auto eit = std::find_if(entities.begin(), entities.end(), 
					[tmp] (std::unique_ptr<Game::Entity>& ptr) 
			{
				return ptr.get() == tmp->getOwner();
			});
			if (eit != entities.end()) 
				entities.erase(eit);
			
			it = temporary.erase(it);
		} else 
			++it;
	}
}
