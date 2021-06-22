#ifndef MENUPAGE_H_INCLUDED
#define MENUPAGE_H_INCLUDED

#include "gui.h"
#include "surface.h"

class MenuPage : public GUI::Page {
public:
	MenuPage( GUI::Button::ClickCallback calibrateCallback
			, GUI::Button::ClickCallback testTouchCallback
			, GUI::Button::ClickCallback hostGameCallback
			, GUI::Button::ClickCallback joinGameCallback
			)
		: m_calibrateCallback(calibrateCallback)
		, m_testTouchCallback(testTouchCallback)
		, m_hostGameCallback(hostGameCallback)
		, m_joinGameCallback(joinGameCallback)
	{}

	~MenuPage() {
		for (int i = 0; i < m_buttons.size(); ++i)
			delete m_buttons[i];
	}

	void init() override {
		surface.reset();

		for (int i = 0; i < m_buttons.size(); ++i)
			delete m_buttons[i];
		m_buttons.clear();

		int currentPosition = 20;
		auto createButton = [&currentPosition](const char *text, GUI::Button::ClickCallback callback) {
			const int h = 50;
			Rect r = Rect::withCenter(WIDTH / 2, currentPosition + h / 2, WIDTH * 0.75, h);
			auto c = GUI::Color::fromHex;
			currentPosition += h + 30;
			return new GUI::Button(text, callback, nullptr, r, c("1e88e5"), c("005cb2"), c("212121"));
		};

		m_buttons.append(createButton("Calibrate Touch", m_calibrateCallback));
		m_buttons.append(createButton("Test Touch", m_testTouchCallback));
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
	GUI::Button::ClickCallback m_calibrateCallback;
	GUI::Button::ClickCallback m_testTouchCallback;
	GUI::Button::ClickCallback m_hostGameCallback;
	GUI::Button::ClickCallback m_joinGameCallback;

	Array<GUI::Button *, 16> m_buttons;
};

#endif
