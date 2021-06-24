#ifndef SELECTNETWORK_H_INCLUDED
#define SELECTNETWORK_H_INCLUDED

#include "array.h"
#include "gui.h"
#include "surface.h"
#include "common.h"

#include <WiFi.h>

class SelectWifiNetworkPage : public GUI::Page {
public:
	static void onNextPressed(void *data) {
		static_cast<SelectWifiNetworkPage *>(data)->showNextPage();
	}

	static void onPrevPressed(void *data) {
		static_cast<SelectWifiNetworkPage *>(data)->showPrevPage();
	}

	static void onScanPressed(void *data) {
		static_cast<SelectWifiNetworkPage *>(data)->scan();
	}

	void showNextPage() {
		if ((m_page + 1) * 4 < m_networkCount) {
			++m_page;
			showNetworks();
		}
	}

	void showPrevPage() {
		if (m_page > 0) {
			--m_page;
			showNetworks();
		}
	}

	void showMessage(String message) {
		lcd.setColor(100, 100, 100);
		Rect rect {int(WIDTH * 0.1), HEIGHT / 2 - 70, int(WIDTH * 0.9), HEIGHT / 2 + 70};
		rect.draw();
		lcd.setColor(255, 255, 255);
		lcd.setBackColor(100, 100, 100);
		lcd.print(message.c_str(), rect.x1 + 20, rect.y1 + (rect.height() - 12) / 2);
	}

	void scan() {
		m_page = 0;
		showMessage("Scanning...");
		m_networkCount = 0;
		m_networkCount = WiFi.scanNetworks();
		surface.update();
		showNetworks();
	}

	void showNetworks() {
		for (int i = 0; i < 4; ++i) {
			int id = m_page * 4 + i;
			String ssid = id < m_networkCount ? WiFi.SSID(id) : "";

			m_buttons[i]->setText(ssid);
		}
	}

	void onFailedToConnect() {
		showMessage("Failed to connect");
		delay(5000);
		m_state = State::SelectingNetwork;
		scan();
	}

	SelectWifiNetworkPage() {
	}

	~SelectWifiNetworkPage() {
		for (int i = 0; i < m_buttons.size(); ++i)
			delete m_buttons[i];
		m_buttons.clear();
	}

	void init() override {
		m_page = 0;
		m_networkCount = 0;
		m_state = State::SelectingNetwork;

		WiFi.mode(WIFI_STA);
		WiFi.disconnect();

		surface.reset();

		for (int i = 0; i < m_buttons.size(); ++i)
			delete m_buttons[i];
		m_buttons.clear();

		int currentPosition = 20;
		int i = 0;
		auto createButton = [this, &currentPosition, &i]() {
			const int h = 50;
			Rect r { int(WIDTH * 0.1), currentPosition, int(WIDTH * 0.7), currentPosition + h };
			auto c = GUI::Color::fromHex;
			currentPosition += h + 30;
			auto onButtonPressed = [this, i](void *) {
				int ssid = i + m_page * 4;
				m_state = State::ConnectingToServer;
				showMessage("Connecting to server...");
				logicQueue.push(Message::joinGame(ssid));
			};
			auto button = new GUI::Button(String(), onButtonPressed, nullptr, r, c("1e88e5"), c("005cb2"), c("212121"));
			++i;
			return button;
		};

		m_buttons.append(createButton());
		m_buttons.append(createButton());
		m_buttons.append(createButton());
		m_buttons.append(createButton());

		auto c = GUI::Color::fromHex;
		m_buttons.append(new GUI::Button("Next", onNextPressed, this, Rect{WIDTH - 120, HEIGHT - 80, WIDTH - 20, HEIGHT - 20}, c("1e88e5"), c("005cb2"), c("212121")));
		m_buttons.append(new GUI::Button("Scan", onScanPressed, this, Rect{WIDTH - 120, HEIGHT / 2 - 30, WIDTH - 20, HEIGHT / 2 + 30}, c("1e88e5"), c("005cb2"), c("212121")));
		m_buttons.append(new GUI::Button("Prev", onPrevPressed, this, Rect{WIDTH - 120, 20, WIDTH - 20, 80}, c("1e88e5"), c("005cb2"), c("212121")));

		scan();
	}
	
	void draw() override {
		if (m_state == State::SelectingNetwork) {
			Page::draw();
		}
	}

	void onPress(Point p) override {
		if (m_state == State::SelectingNetwork) {
			for (int i = 0; i < m_buttons.size(); ++i)
				m_buttons[i]->onPress(p);
		}
	}

	void onDrag(Point p) override {
		if (m_state == State::SelectingNetwork) {
			for (int i = 0; i < m_buttons.size(); ++i)
				m_buttons[i]->onDrag(p);
		}
	}

	void onRelease() override {
		if (m_state == State::SelectingNetwork) {
			for (int i = 0; i < m_buttons.size(); ++i)
				m_buttons[i]->onRelease();
		}
	}

private:
	Array<GUI::Button *, 16> m_buttons;

	int m_networkCount;
	int m_page;

	enum class State {
		SelectingNetwork,
		ConnectingToServer,
	} m_state;
};

#endif
