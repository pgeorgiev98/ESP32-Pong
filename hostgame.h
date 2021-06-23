#ifndef HOSTGAME_H_INCLUDED
#define HOSTGAME_H_INCLUDED

#include "gui.h"
#include "surface.h"

class HostGamePage : public GUI::Page {
public:
	HostGamePage() {}

	void init() override {
		lcd.clrScr();
		WiFi.softAP(m_ssid, m_pass);
		m_needsRedraw = true;
	}

	void draw() override {
		if (m_needsRedraw) {
			lcd.setColor(100, 100, 100);
			Rect rect {int(WIDTH * 0.2), HEIGHT / 2 - 70, int(WIDTH * 0.9), HEIGHT / 2 + 70};
			rect.draw();
			lcd.setColor(255, 255, 255);
			lcd.setBackColor(100, 100, 100);
			lcd.print("Waiting for other device to join...", rect.x1 + 20, rect.y1 + (rect.height() - 12) / 2);
			
			m_needsRedraw = false;
		}
	}

private:
	static constexpr const char *m_ssid = "ESP32-Project";
	static constexpr const char *m_pass = "password";

	bool m_needsRedraw;
};

#endif
