#include "gamepage.h"
#include "messagequeue.h"

extern MessageQueue logicQueue;

const int BALL_R = 10;
const int BALL_D = 20;

void GamePage::init() {
	surface.reset();
	m_gameOver = false;
	m_player = -1;
	m_position[0] = HEIGHT / 2;
	m_position[1] = HEIGHT / 2;
	Rect r1{30, m_position[0] - 50, 50, m_position[0] + 50};
	Rect r2{WIDTH - 50, m_position[1] - 50, WIDTH - 30, m_position[1] + 50};

	auto drawBall = [this](Rect r) {
		Rect rect = surface.rectForShape(m_ballID);
		if (rect.intersects(r)) {
			lcd.setColor(0, 0, 190);
			rect.draw();
		}
	};

	m_ballPos = Point{100, HEIGHT/2};
	m_ballSpeed = Point{5, 5};
	Rect ballRect = Rect::withCenter(m_ballPos.x, m_ballPos.y, BALL_D, BALL_D);
	m_ballID = surface.createShape(Surface::Shape{[drawBall](Rect r){ drawBall(r); }, ballRect, true, true});

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
	if (m_player == 0) {
		if (m_gameOver && m_ballSpeed == Point{0, 0} && m_gameOverTimer.time() > 5000) {
			m_ballSpeed = Point{5, 5};
			m_gameOver = false;
		}

		if (m_ballPos.y <= BALL_R && m_ballSpeed.y < 0)
			m_ballSpeed.y = -m_ballSpeed.y;
		if (m_ballPos.y >= HEIGHT - BALL_R && m_ballSpeed.y > 0)
			m_ballSpeed.y = -m_ballSpeed.y;

		if (m_ballPos.x <= -BALL_R && m_ballSpeed.x < 0)
			m_ballSpeed = Point{0, 0};
		if (m_ballPos.x >= WIDTH + BALL_R && m_ballSpeed.x > 0)
			m_ballSpeed = Point{0, 0};

		Rect ballRect = surface.rectForShape(m_ballID);
		Rect leftRect = surface.rectForShape(m_playerShapeIDs[0]);
		Rect rightRect = surface.rectForShape(m_playerShapeIDs[1]);

		if (m_ballSpeed.x < 0 && ballRect.intersects(leftRect)) {
			m_ballSpeed.x = -m_ballSpeed.x;
		}

		if (m_ballSpeed.x > 0 && ballRect.intersects(rightRect)) {
			m_ballSpeed.x = -m_ballSpeed.x;
		}

		surface.moveShape(m_ballID, m_ballSpeed.x, m_ballSpeed.y);
		m_ballPos.x += m_ballSpeed.x;
		m_ballPos.y += m_ballSpeed.y;

		logicQueue.push(Message::ballMoved(m_ballPos.x, m_ballPos.y));
	}

	surface.draw();

	if (!m_gameOver && m_player == 0) {
		if (m_ballSpeed == Point{0, 0}) {
			setBallPosition(Point{100, HEIGHT/2});
			logicQueue.push(Message::ballMoved(m_ballPos.x, m_ballPos.y));
			m_gameOverTimer.reset();
			m_gameOver = true;
		}
	}
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

void GamePage::setBallPosition(Point p) {
	Point delta;
	delta.x = p.x - m_ballPos.x;
	delta.y = p.y - m_ballPos.y;
	surface.moveShape(m_ballID, delta.x, delta.y);
	m_ballPos = p;
}
