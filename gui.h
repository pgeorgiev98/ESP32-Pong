#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "display.h"
#include "array.h"

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

	struct Point {
		int x, y;
	};

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

	class Button {
	public:
		Button() {}

		Button(const char *text, void(*callback)(void), Rect rect, Color normalColor, Color pressedColor, Color textColor)
			: m_text(text), m_callback(callback), m_rect(rect), m_normalColor(normalColor), m_pressedColor(pressedColor), m_textColor(textColor), m_pressed(false) {}

		void draw() const {
			const Color c = m_pressed ? m_pressedColor : m_normalColor;
			m_rect.draw(c);
			lcd.setColor(m_textColor.r, m_textColor.g, m_textColor.b);
			lcd.setBackColor(c.r, c.g, c.b);
			lcd.print(m_text, m_rect.x1 + 20, m_rect.y1 + (m_rect.height() - 15) / 2);
		}

		void onPress(Point p) {
			bool pressed = m_rect.contains(p);
			if (pressed != m_pressed) {
				m_pressed = pressed;
				//draw();
			}
		}

		void onRelease() {
			if (m_pressed) {
				m_pressed = false;
				//draw();
				m_callback();
			}
		}

		void onDrag(Point p) {
			onPress(p);
		}

	private:
		const char *m_text;
		void(*m_callback)(void);
		Rect m_rect;
		Color m_normalColor, m_pressedColor, m_textColor;
		bool m_pressed;
	};

	class Page {
	public:
		void draw() const {
			for (int i = 0; i < m_buttons.size(); ++i)
				m_buttons[i].draw();
		}

		void addButton(const Button &button) {
			m_buttons.append(button);
		}

		void onPress(Point p) {
			for (int i = 0; i < m_buttons.size(); ++i)
				m_buttons[i].onPress(p);
		}

		void onRelease() {
			for (int i = 0; i < m_buttons.size(); ++i)
				m_buttons[i].onRelease();
		}

		void onDrag(Point p) {
			for (int i = 0; i < m_buttons.size(); ++i)
				m_buttons[i].onDrag(p);
		}

	protected:
		Array<Button, 16> m_buttons;
	};
}

#endif
