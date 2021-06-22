#include "display.h"
#include "array.h"
#include "util.h"
#include "gui.h"
#include "touch.h"
#include "surface.h"
#include "calibrate.h"

#include <MCUFRIEND_kbv.h>

// 480 x 320
// Origin is at the Bottom-Left

UTFTGLUE lcd(0, 1, 2, 3, 4, 5); // Those are dummy values
Touch touch;
Surface surface;


class TestPage : public GUI::Page {
public:
	TestPage(void(*callback)(void*))
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

static GUI::Page* currentPage = nullptr;
static void setCurrentPage(GUI::Page *page) {
	VERIFY(page);
	currentPage = page;
	currentPage->init();
}

static GUI::MenuPage menuPage(
	openCalibrateTouchPage,
	openConnnectToWifiPage,
	openHostGamePage,
	openJoinGamePage
);
static CalibratePage calibratePage(openMenuPage);
static TestPage testPage(openMenuPage);

static void openMenuPage(void *)           { setCurrentPage(&menuPage); }
static void openCalibrateTouchPage(void *) { setCurrentPage(&calibratePage); }
static void openConnnectToWifiPage(void *) { setCurrentPage(&testPage); }
static void openHostGamePage(void *)       { setCurrentPage(nullptr); }
static void openJoinGamePage(void *)       { setCurrentPage(nullptr); }

TaskHandle_t guiTask;
TaskHandle_t logicTask;
SemaphoreHandle_t mutex;

void setup() {
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
	for (;;) {
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

		delay(20); // TODO: use timer
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
