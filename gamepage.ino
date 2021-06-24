#include "gamepage.h"
#include "messagequeue.h"

extern MessageQueue logicQueue;

void GamePage::init() {
	surface.reset();
	m_player = -1;
	m_position[0] = HEIGHT / 2;
	m_position[1] = HEIGHT / 2;
	Rect r1{30, m_position[0] - 50, 50, m_position[0] + 50};
	Rect r2{WIDTH - 50, m_position[1] - 50, WIDTH - 30, m_position[1] + 50};
	auto drawPlayer = [this](int player, Rect r1) {
		int shapeID = m_playerShapeIDs[player];
		Rect r2 = surface.rectForShape(shapeID);
		if (r1.intersects(r2)) {
			Rect r3;
			Rect::getIntersection(r1, r2, r3);
			lcd.setColor(255, 0, 0);
			r3.draw();
		}
	};
	m_playerShapeIDs[0] = surface.createShape(Surface::Shape{[drawPlayer](Rect r){ drawPlayer(0, r); }, r1, true, true});
	m_playerShapeIDs[1] = surface.createShape(Surface::Shape{[drawPlayer](Rect r){ drawPlayer(1, r); }, r2, true, true});
}

void GamePage::draw() {
	surface.draw();
}

void GamePage::onPress(Point p) {
	logicQueue.push(Message::playerMoved(p.y));
	setPlayerPosition(p.y);
}

void GamePage::onDrag(Point p) {
	onPress(p);
}

void GamePage::onRelease() {
}

void GamePage::setPosition(int player, int position) {
	int delta = position - m_position[player];
	m_position[player] += delta;
	surface.moveShape(m_playerShapeIDs[player], 0, delta);
}
