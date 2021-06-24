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
#include "gamepage.h"
#include "common.h"
#include "gameserver.h"
#include "gameclient.h"

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
static GamePage gamePage;

static void openMenuPage(void *)           { setCurrentPage(&menuPage); }
static void openCalibrateTouchPage(void *) { setCurrentPage(&calibratePage); }
static void openTouchTestPage(void *)      { setCurrentPage(&touchTestPage); }
static void openHostGamePage(void *)       { setCurrentPage(&hostGamePage); }
static void openJoinGamePage(void *)       { setCurrentPage(&selectWifiNetworkPage); }

TaskHandle_t guiTask;
TaskHandle_t logicTask;

MessageQueue guiQueue;
MessageQueue logicQueue;

void setup() {
	disableCore0WDT();
	disableCore1WDT();

	randomSeed(analogRead(0));
	Serial.begin(9600);

	xTaskCreatePinnedToCore(
			guiTaskCode,   /* Task function. */
			"GUI",         /* name of task. */
			10000,         /* Stack size of task */
			nullptr,       /* parameter of the task */
			1,             /* priority of the task */
			&guiTask,      /* Task handle to keep track of created task */
			1);            /* pin task to core 0 */

	xTaskCreatePinnedToCore(
			logicTaskCode, /* Task function. */
			"GUI",         /* name of task. */
			10000,         /* Stack size of task */
			nullptr,       /* parameter of the task */
			1,             /* priority of the task */
			&logicTask,    /* Task handle to keep track of created task */
			0);            /* pin task to core 1 */
}

void loop() {
	delay(10000);
}

void guiTaskCode(void *) {
	Message m;
	touch.loadSettings();

	lcd.InitLCD();
	lcd.setFont(SmallFont);
	lcd.clrScr();
	openMenuPage(nullptr);

	int lastEvent = -1;
	Timer frameTimer;
	for (;;) {
		frameTimer.reset();

		while (guiQueue.wait(m)) {
			switch (m.type) {

			case Message::Type::JoinedGame: {
				if (currentPage == &selectWifiNetworkPage) {
					setCurrentPage(&gamePage);
					gamePage.setPlayer(1);
				} else if (currentPage == &hostGamePage) {
					setCurrentPage(&gamePage);
					gamePage.setPlayer(0);
				}
				break;
			}

			case Message::Type::FailedToJoin: {
				if (currentPage == &selectWifiNetworkPage) {
					selectWifiNetworkPage.onFailedToConnect();
				}
				break;
			}

			case Message::Type::PlayerMoved: {
				if (currentPage == &gamePage) {
					gamePage.setOponentPosition(m.data.playerMoved.position);
				}
				break;
			}

			default: {}

			}
		}

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

static GameServer gameServer;
static GameClient gameClient;
void logicTaskCode(void *) {
	Message m;
	bool isHost = false;
	for (;;) {
		while (logicQueue.wait(m)) {
			switch (m.type) {

			case Message::Type::HostGame: {
				isHost = true;
				gameServer.begin();
				break;
			}

			case Message::Type::JoinGame: {
				isHost = false;
				String ssid = WiFi.SSID(m.data.joinGame.ssid);
				bool ok;
				WiFi.mode(WIFI_STA);
				WiFi.disconnect();
				int r = WiFi.begin(ssid.c_str(), "password");
				while ((r = WiFi.status()) == WL_DISCONNECTED)
					delay(100);
				ok = (r == WL_CONNECTED);
				if (ok) {
					gameClient.begin();
					gameClient.sendMessage(Message::joinedGame());
					guiQueue.push(Message::joinedGame());
				}

				if (!ok) {
					guiQueue.push(Message::failedToJoin());
				}

				break;
			}

			case Message::Type::PlayerMoved: {
				if (currentPage == &gamePage) {
					if (isHost) {
						gameServer.sendMessage(m);
					} else {
						gameClient.sendMessage(m);
					}
				}
				break;
			}

			default: {}

			}
		}
		gameServer.tick();
		gameClient.tick();
		delay(20);
	}
}
