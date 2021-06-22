#ifndef CALIBRATE_H_INCLUDED
#define CALIBRATE_H_INCLUDED

#include "gui.h"
#include "surface.h"
#include "touch.h"

class CalibratePage : public GUI::Page {
public:
	CalibratePage(void(*callback)(void*))
		: m_callback(callback)
	{}

	void init() override {
		m_i = 0;
		m_col = m_row = 0;
		m_draw = true;
		lcd.clrScr();
		touch.setSendRawData(true);
	}

	void draw() override {
		if (m_draw) {
			m_draw = false;

			int x = 20 + m_col * (WIDTH - 40) / 2;
			int y = 20 + m_row * (HEIGHT - 40) / 2;
			lcd.clrScr();
			lcd.setColor(255, 255, 255);
			lcd.drawRect(x - 10, y - 10, x + 10, y + 10);
			lcd.drawLine(x - 15, y, x + 15, y);
			lcd.drawLine(x, y - 15, x, y + 15);
		}
	}

	void onRelease() override {
		m_draw = true;
		m_r[m_i] = touch.lastPoint();
		++m_i;
		if (++m_col > 2) {
			m_col = 0;
			if (++m_row > 2) {
				m_i = 0;
				m_row = m_col = 2;
				m_draw = false;

				int clx = (m_r[0].x + m_r[3].x + m_r[5].x) / 3;
				int chx = (m_r[2].x + m_r[4].x + m_r[7].x) / 3;
				int cly = (m_r[0].y + m_r[1].y + m_r[2].y) / 3;
				int chy = (m_r[5].y + m_r[6].y + m_r[7].y) / 3;
				float px = float(chx - clx) / (WIDTH - 40);
				float py = float(chy - cly) / (HEIGHT - 40);
				clx -= px * 10;
				chx += px * 10;
				cly -= py * 10;
				chy += py * 10;
				uint32_t calx = (uint32_t(clx) << 14) + long(chx);
				uint32_t caly = (uint32_t(cly) << 14) + long(chy);

				uint32_t left   = (calx >> 14) & 0x3FFF;
				uint32_t right  = (calx >>  0) & 0x3FFF;
				uint32_t top    = (caly >> 14) & 0x3FFF;
				uint32_t bottom = (caly >>  0) & 0x3FFF;
				
				touch.calibrate(left, right, top, bottom);

				touch.setSendRawData(false);

				m_callback(nullptr);
			}
		}
		if (m_col == 1 && m_row == 1)
			++m_col;
		VERIFY(m_i < 8);
	}

private:
	void(*m_callback)(void*);
	int m_i;
	int m_col, m_row;
	bool m_draw;
	Point m_r[8];
};

#endif
