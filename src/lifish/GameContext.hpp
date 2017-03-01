#pragma once

#include <string>
#include <vector>
#include <memory>
#include "WindowContext.hpp"
#include "LevelSet.hpp"
#include "LevelManager.hpp"
#include "WinLoseHandler.hpp"
#include "SidePanel.hpp"

namespace lif {

class Player;
struct SaveData;

#ifndef RELEASE
namespace debug {

class DebugEventHandler;

}
#endif

class GameContext : public lif::WindowContext {
public:
	enum : unsigned {
		DBG_DRAW_COLLIDERS   = 1,
		DBG_DRAW_SH_CELLS    = 1 << 1,
		DBG_PRINT_CD_STATS   = 1 << 2,
		DBG_PRINT_GAME_STATS = 1 << 3
	};
private:
#ifndef RELEASE
       friend class lif::debug::DebugEventHandler;
#endif

	unsigned debug = 0;
	unsigned cycle = 0;

	/** Whether this Context was already active (false if it has just been activated) */
	bool wasActive = true;

	mutable sf::RenderTexture gameRenderTex;
	mutable sf::RenderTexture sidePanelRenderTex;

	const sf::Window& window;
	lif::LevelSet ls;
	lif::LevelManager lm;
	lif::SidePanel sidePanel;
	lif::WinLoseHandler wlHandler;

	void _initLM(const sf::Window& window, short lvnum);
#ifndef RELEASE
	void _printCDStats() const;
	void _printGameStats() const;
#endif
	void _advanceLevel();

public:
	explicit GameContext(sf::Window& window, const std::string& levelsetName, short startLv);

	lif::WinLoseHandler& getWLHandler() { return wlHandler; }
	const lif::WinLoseHandler& getWLHandler() const { return wlHandler; }
	lif::LevelManager& getLM() { return lm; }
	const lif::LevelManager& getLM() const { return lm; }
	lif::SidePanel& getSidePanel() { return sidePanel; }
	const lif::SidePanel& getSidePanel() const { return sidePanel; }

	void loadGame(const lif::SaveData& saveData);

	void update() override;
	bool handleEvent(sf::Window& window, sf::Event evt) override;
	void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
	void setActive(bool b) override;

	void toggleDebug(unsigned dbg);
};

}
