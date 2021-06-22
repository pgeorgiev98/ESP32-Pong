#ifndef WIFI_H_INCLUDED
#define WIFI_H_INCLUDED

#include "array.h"
#include "gui.h"
#include "surface.h"

#include <WiFi.h>

class SelectWifiNetworkPage : public GUI::Page {
	public:
		static void onButtonPressed(void *data) {
			auto button = static_cast<GUI::Button *>(data);
		}

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

		void scan() {
			m_page = 0;
			lcd.setColor(100, 100, 100);
			Rect rect {int(WIDTH * 0.2), HEIGHT / 2 - 70, int(WIDTH * 0.8), HEIGHT / 2 + 70};
			rect.draw();
			lcd.setColor(255, 255, 255);
			lcd.setBackColor(100, 100, 100);
			lcd.print("Scanning...", rect.x1 + 20, rect.y1 + (rect.height() - 12) / 2);
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

			WiFi.mode(WIFI_STA);
			WiFi.disconnect();

			surface.reset();

			for (int i = 0; i < m_buttons.size(); ++i)
				delete m_buttons[i];
			m_buttons.clear();

			int currentPosition = 20;
			auto createButton = [&currentPosition]() {
				const int h = 50;
				Rect r { int(WIDTH * 0.1), currentPosition, int(WIDTH * 0.7), currentPosition + h };
				auto c = GUI::Color::fromHex;
				currentPosition += h + 30;
				auto button = new GUI::Button(String(), onButtonPressed, nullptr, r, c("1e88e5"), c("005cb2"), c("212121"));
				button->setCallbackData(button);
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

		void onPress(Point p) override {
			for (int i = 0; i < m_buttons.size(); ++i)
				m_buttons[i]->onPress(p);
		}

		void onDrag(Point p) override {
			for (int i = 0; i < m_buttons.size(); ++i)
				m_buttons[i]->onDrag(p);
		}

		void onRelease() override {
			for (int i = 0; i < m_buttons.size(); ++i)
				m_buttons[i]->onRelease();
		}

	private:
		Array<GUI::Button *, 16> m_buttons;

		int m_networkCount;
		int m_page;
};

#endif
