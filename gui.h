#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "display.h"
#include "array.h"
#include "surface.h"

namespace GUI {
	struct Color {
		byte r, g, b;

		static Color fromHex(const char hex[7]) {
			auto d = [](char h) -> byte {
				if (h >= '0' && h <= '9') return byte(h - '0');
				else if (h >= 'A' && h <= 'Z') return byte(h - 'A' + 10);
				else if (h >= 'a' && h <= 'z') return byte(h - 'a' + 10);
				else { FAIL("Invalid HEX"); return 0; }
			};
			return Color{
					byte((d(hex[0]) << 4) | d(hex[1])),
					byte((d(hex[2]) << 4) | d(hex[3])),
					byte((d(hex[4]) << 4) | d(hex[5]))};
		}
	};

	/*
	struct Rect {
		static Rect fromCenter(int centerX, int centerY, int width, int height) {
			int x1 = centerX - width / 2;
			int y1 = centerY - height / 2;
			return Rect{x1, y1, x1 + width, y1 + height};
		}

		void draw(Color c) const {
			lcd.setColor(c.r, c.g, c.b);
			lcd.fillRect(x1, y1, x2, y2);
		}

		bool contains(Point p) const {
			return p.x >= x1 && p.x < x2 && p.y >= y1 && p.y < y2;
		}

		int xCenter() const {
			return (x1 + x2) / 2;
		}

		int yCenter() const {
			return (y1 + y2) / 2;
		}

		int width() const {
			return x2 - x1;
		}

		int height() const {
			return y2 - y1;
		}

		int x1, y1, x2, y2;
	};
	*/

	class Button {
	public:
		typedef void(*ClickCallback)(void *);

		static void drawFunc(void *button, Rect rect) {
			static_cast<Button *>(button)->draw(rect);
		}

		Button() {}

		Button(const char *text, ClickCallback callback, void *callbackData, Rect rect, Color normalColor, Color pressedColor, Color textColor)
			: m_shapeID(surface.createShape({this, drawFunc, rect, true, false}))
			, m_text(text)
			, m_callback(callback)
			, m_callbackData(callbackData)
			, m_normalColor(normalColor)
			, m_pressedColor(pressedColor)
			, m_textColor(textColor)
			, m_wasPressed(false)
			, m_pressed(false)
		{
		}

		Button(const Button &) = delete;
		Button &operator=(const Button &) = delete;

		const Rect &rect() const {
			return surface.rectForShape(m_shapeID);
		}

		void draw(Rect drawRect) const {
			const Rect &rect = surface.rectForShape(m_shapeID);
			if (rect.intersects(drawRect)) {
				const Color c = m_pressed ? m_pressedColor : m_normalColor;
				lcd.setColor(c.r, c.g, c.b);
				rect.draw();
				lcd.setColor(m_textColor.r, m_textColor.g, m_textColor.b);
				lcd.setBackColor(c.r, c.g, c.b);
				lcd.print(m_text, rect.x1 + 20, rect.y1 + (rect.height() - 15) / 2);
			}
		}

		void update() {
			surface.invalidateRect(surface.rectForShape(m_shapeID));
		}

		void onPress(Point p) {
			bool pressed = rect().contains(p);
			if (pressed && !m_pressed) {
				m_wasPressed = true;
				m_pressed = true;
				update();
			}
		}

		void onDrag(Point p) {
			bool pressed = rect().contains(p);
			if (m_wasPressed && pressed != m_pressed) {
				m_pressed = pressed;
				update();
			}
		}

		void onRelease() {
			m_wasPressed = false;
			if (m_pressed) {
				m_pressed = false;
				update();
				m_callback(m_callbackData);
			}
		}

	private:
		int m_shapeID;
		const char *m_text;
		ClickCallback m_callback;
		void *m_callbackData;
		Color m_normalColor, m_pressedColor, m_textColor;
		bool m_wasPressed;
		bool m_pressed;
	};

	class Page {
	public:
		virtual void init() {}
		virtual void draw() { surface.draw(); }
		virtual void onPress(Point) {}
		virtual void onDrag(Point) {}
		virtual void onRelease() {}
	};

	class MenuPage : public Page {
	public:
		MenuPage( Button::ClickCallback calibrateCallback
				, Button::ClickCallback wifiMenuCallback
				, Button::ClickCallback hostGameCallback
				, Button::ClickCallback joinGameCallback
				)
			: m_calibrateCallback(calibrateCallback)
			, m_wifiMenuCallback(wifiMenuCallback)
			, m_hostGameCallback(hostGameCallback)
			, m_joinGameCallback(joinGameCallback)
		{}

		~MenuPage() {
			for (int i = 0; i < m_buttons.size(); ++i)
				delete m_buttons[i];
		}

		void init() override {
			for (int i = 0; i < m_buttons.size(); ++i)
				delete m_buttons[i];
			m_buttons.clear();

			int currentPosition = 20;
			auto createButton = [&currentPosition](const char *text, Button::ClickCallback callback) {
				const int h = 50;
				Rect r = Rect::withCenter(WIDTH / 2, currentPosition + h / 2, WIDTH * 0.75, h);
				auto c = GUI::Color::fromHex;
				currentPosition += h + 30;
				return new GUI::Button(text, callback, nullptr, r, c("1e88e5"), c("005cb2"), c("212121"));
			};

			m_buttons.append(createButton("Calibrate Touch", m_calibrateCallback));
			m_buttons.append(createButton("Connect to Wifi", m_wifiMenuCallback));
			m_buttons.append(createButton("Host Game", m_hostGameCallback));
			m_buttons.append(createButton("Join Game", m_joinGameCallback));
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

	protected:
		Button::ClickCallback m_calibrateCallback;
		Button::ClickCallback m_wifiMenuCallback;
		Button::ClickCallback m_hostGameCallback;
		Button::ClickCallback m_joinGameCallback;

		Array<Button *, 16> m_buttons;
	};
}

#endif
