#include "display.h"
#include "array.h"
#include "util.h"
#include "gui.h"
#include "touch.h"
#include "surface.h"
#include "calibrate.h"
#include "menupage.h"
#include "touchtestpage.h"
#include "selectnetwork.h"
#include "hostgame.h"

#include <MCUFRIEND_kbv.h>

// 480 x 320
// Origin is at the Top-Left

UTFTGLUE lcd(0, 1, 2, 3, 4, 5); // Those are dummy values
Touch touch;
Surface surface;

static GUI::Page* currentPage = nullptr;
static void setCurrentPage(GUI::Page *page) {
	VERIFY(page);
	currentPage = page;
	currentPage->init();
}

static MenuPage menuPage(
	openCalibrateTouchPage,
	openTouchTestPage,
	openHostGamePage,
	openJoinGamePage
);
static CalibratePage calibratePage(openMenuPage);
static TouchTestPage touchTestPage(openMenuPage);
static HostGamePage hostGamePage;
static SelectWifiNetworkPage selectWifiNetworkPage;

static void openMenuPage(void *)           { setCurrentPage(&menuPage); }
static void openCalibrateTouchPage(void *) { setCurrentPage(&calibratePage); }
static void openTouchTestPage(void *)      { setCurrentPage(&touchTestPage); }
static void openHostGamePage(void *)       { setCurrentPage(&hostGamePage); }
static void openJoinGamePage(void *)       { setCurrentPage(&selectWifiNetworkPage); }

TaskHandle_t guiTask;
TaskHandle_t logicTask;
SemaphoreHandle_t mutex;

void setup() {
	disableCore0WDT();
	disableCore1WDT();

	randomSeed(analogRead(0));
	Serial.begin(9600);

	mutex = xSemaphoreCreateMutex();

	xTaskCreatePinnedToCore(
			guiTaskCode,   /* Task function. */
			"GUI",         /* name of task. */
			10000,         /* Stack size of task */
			nullptr,       /* parameter of the task */
			1,             /* priority of the task */
			&guiTask,      /* Task handle to keep track of created task */
			0);            /* pin task to core 0 */

	xTaskCreatePinnedToCore(
			logicTaskCode, /* Task function. */
			"GUI",         /* name of task. */
			10000,         /* Stack size of task */
			nullptr,       /* parameter of the task */
			1,             /* priority of the task */
			&logicTask,    /* Task handle to keep track of created task */
			1);            /* pin task to core 1 */
}

void loop() {
	delay(10000);
}

void guiTaskCode(void *) {
	touch.loadSettings();

	lcd.InitLCD();
	lcd.setFont(SmallFont);
	lcd.clrScr();
	openMenuPage(nullptr);

	int lastEvent = -1;
	Timer frameTimer;
	for (;;) {
		frameTimer.reset();

		touch.poll();
		Touch::Event touchEvent = touch.event();
		if (int(touchEvent) != lastEvent) {
			lastEvent = int(touchEvent);
		}
		switch (touchEvent) {
		case Touch::Event::None:
			break;

		case Touch::Event::Press:
			currentPage->onPress(touch.currentPoint());
			break;

		case Touch::Event::Drag:
			currentPage->onDrag(touch.currentPoint());
			break;

		case Touch::Event::Release:
			currentPage->onRelease();
			break;
		}

		currentPage->draw();

		int frameTime = frameTimer.time();
		delay(max(0, 17 - frameTime)); // Target ~60 FPS
	}
}

void logicTaskCode(void *) {
	/*
	xSemaphoreTake( mutex, TickType_t(10) );
	xSemaphoreGive( mutex );
	*/
	for (;;) {
		delay(10000);
	}
}

/*
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
*/
