#ifndef GAMEPAGE_H_INCLUDED
#define GAMEPAGE_H_INCLUDED

#include "gui.h"
#include "surface.h"
#include "common.h"

class GamePage : public GUI::Page {
public:
	GamePage() {}

	void setPlayer(int player) {
		m_player = player;
	}

	void setPosition(int player, int position);

	void setOponentPosition(int position) {
		VERIFY(m_player != -1);
		setPosition(!m_player, position);
	}

	void setPlayerPosition(int position) {
		VERIFY(m_player != -1);
		setPosition(m_player, position);
	}

	void draw() override;

	void init() override;

	void onPress(Point p) override;
	void onDrag(Point p) override;
	void onRelease() override;

private:
	int m_player;
	int m_position[2];
	int m_playerShapeIDs[2];
};

#endif
