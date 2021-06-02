#include "display.h"
#include "array.h"
#include "util.h"
#include "gui.h"
#include "touch.h"
#include "surface.h"

#include <MCUFRIEND_kbv.h>

// 480 x 320
// Origin is at the Bottom-Left

UTFTGLUE lcd(0, 1, 2, 3, 4, 5); // Those are dummy values
Touch touch;
Surface surface;
GUI::Page page;

void setup() {
	randomSeed(analogRead(0));

	lcd.InitLCD();
	lcd.setFont(SmallFont);

	lcd.clrScr();
	Serial.begin(9600);
}

void reportTime(const char *msg, unsigned long time) {
	lcd.clrScr();
	lcd.setColor(0, 255, 0);
	lcd.setBackColor(0, 0, 255);
	lcd.print(msg, CENTER, 150);
	lcd.printNumI(time, CENTER, 165);
	delay(3000);
}

static const unsigned long sceneTime = 10000;

void bouncingRectangle() {
	surface.reset();
	Timer timer;
	int rect = surface.createRect(100, 0, 140, 40, true);
	while (timer.time() < sceneTime) {
		for (int i = 0; i < 360; ++i) {
			int pos = int(200 * abs(sin(i * 3.14 / 180.0)));
			surface.moveShape(rect, 0, pos - surface.rectForShape(rect).y1);
			surface.draw();
			delay(10);
		}
	}
	reportTime("Bouncing rectangle", timer.time());
	lcd.clrScr();
}

void touchTracking() {
	surface.reset();
	Timer timer;
	int rect = surface.createRect(0, 0, 40, 40, false);
	while (timer.time() < sceneTime) {
		Point pt = touch.currentPoint();
		if (touch.event() != Touch::Event::None)
			surface.setRectPos(rect, pt.x - 20, pt.y - 20);
		surface.setShapeVisibility(rect, touch.event() != Touch::Event::None);
		surface.draw();
		delay(20);
	}
	reportTime("Touch tracking", timer.time());
	lcd.clrScr();
}

void loop() {
	int currentPosition = 40;
	auto createButton = [&currentPosition](const char *text, void(*callback)(void)) {
		const int h = 40;
		GUI::Rect r = GUI::Rect::fromCenter(WIDTH / 2, currentPosition + h / 2, WIDTH * 0.75, h);
		auto c = GUI::Color::fromHex;
		currentPosition += h + 40;
		return GUI::Button(text, callback, r, c("1e88e5"), c("005cb2"), c("212121"));
	};

	page.addButton(createButton("Bouncing rectangle", bouncingRectangle));
	page.addButton(createButton("Touch tracking", touchTracking));

	for (;;) {
		touch.poll();
		/*
		TouchPoint touchPt = touch.lastPoint();
		GUI::Point pt{touchPt.x, touchPt.y};
		switch (event) {
		case Touch::Event::Press:   page.press(pt); break;
		case Touch::Event::Drag:    page.drag(pt); break;
		case Touch::Event::Release: page.release(); break;
		default: {}
		}
		*/
		page.draw();
		delay(20);
	}
}
