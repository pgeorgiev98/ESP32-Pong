#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "display.h"
#include "array.h"
#include "surface.h"

#include <functional>

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

	class Button {
	public:
		typedef std::function<void(void*)> ClickCallback;

		static void drawFunc(void *button, Rect rect) {
			static_cast<Button *>(button)->draw(rect);
		}

		Button() {}

		Button(const String &text, ClickCallback callback, void *callbackData, Rect rect, Color normalColor, Color pressedColor, Color textColor)
			: m_shapeID(surface.createShape({[this](Rect r) { this->draw(r); }, rect, true, false}))
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
				lcd.print(m_text, rect.x1 + 20, rect.y1 + (rect.height() - 12) / 2);
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

		void setCallbackData(void *data) {
			m_callbackData = data;
		}

		void setText(const String &text) {
			m_text = text;
			update();
		}

		const String &text() const {
			return m_text;
		}

	private:
		int m_shapeID;
		String m_text;
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
}

#endif
