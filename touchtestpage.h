#ifndef TOUCHTESTPAGE_H_INCLUDED
#define TOUCHTESTPAGE_H_INCLUDED

#include "gui.h"
#include "surface.h"

class TouchTestPage : public GUI::Page {
public:
	TouchTestPage(void(*callback)(void*))
		: m_prev({0, 0})
		, m_curr({0, 0})
		, m_callback(callback)
	{}

	void init() override {
		lcd.clrScr();
		m_timer.reset();
	}

	void drawTarget(Point p) {
		int x = p.x;
		int y = p.y;
		lcd.drawRect(x - 10, y - 10, x + 10, y + 10);
		lcd.drawLine(x - 15, y, x + 15, y);
		lcd.drawLine(x, y - 15, x, y + 15);
	}

	void draw() override {
		if (m_prev != m_curr) {
			lcd.setColor(0, 0, 0);
			drawTarget(m_prev);

			lcd.setColor(255, 255, 255);
			drawTarget(m_curr);

			m_prev = m_curr;
		}

		if (m_timer.time() > 15000)
			m_callback(nullptr);
	}

	void onPress(Point p) override {
		m_curr = p;
	}

	void onDrag(Point p) override {
		onPress(p);
	}

private:
	Point m_prev;
	Point m_curr;
	void(*m_callback)(void*);
	Timer m_timer;
};

#endif
