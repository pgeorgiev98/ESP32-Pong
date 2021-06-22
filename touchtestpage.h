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

	void draw() override {
		if (m_prev != m_curr) {
			lcd.setColor(0, 0, 0);
			Rect{m_prev.x - 20, m_prev.y - 20, m_prev.x + 20, m_prev.y + 20}.draw();
			lcd.setColor(255, 0, 0);
			Rect{m_curr.x - 20, m_curr.y - 20, m_curr.x + 20, m_curr.y + 20}.draw();
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
