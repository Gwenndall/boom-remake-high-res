#pragma once

#include <deque>
#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Window.hpp>
#include "EventHandler.hpp"
#include "WithOrigin.hpp"
#include "Activable.hpp"

namespace lif {

/**
 * A WindowContext defines what portion of the game is currently active
 * and displayed. This includes the event loop, logic loop and rendering loop.
 * Only one context should be active at a time.
 */
class WindowContext
	: public sf::Drawable
	, public lif::WithOrigin
	, public lif::Activable
	, public lif::EventHandler
{
protected:
	/** The chain of fallback handlers.
	 *  If this handler didn't catch any event, it may delegate said event to other handlers,
	 *  which will try sequentially to catch it.
	 *  Useful to share and reuse functionality of event handlers.
	 */
	std::deque<std::unique_ptr<lif::EventHandler>> handlers;
	int newContext = -1;

public:
	/** When this method returns a non-negative number, the current WindowContext
	 *  should be switched with that indexed by the returned number.
	 *  The number=>context association is game-dependent, and should be defined
	 *  per-case with an enum or something alike.
	 */
	int getNewContext() const { return newContext; }
	/** This must be called after switching context to reset the `newContext` variable. */
	void resetNewContext() { newContext = -1; }

	virtual void update() = 0;
	void handleEvents(sf::Window& window);
};

}
