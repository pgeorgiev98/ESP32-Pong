#include "gui.h"
#include "surface.h"

class BouncingRectangle : public GUI::Page {
public:
	void init() {
		m_rectID = surface.createRect(100, 0, 140, 40, true);
	}

	void draw() override {
	}

private:
	int m_rectID;
};

class TouchTracking : public GUI::Page {
public:
	void init() {
	}

	void draw() override {
	}

private:
	int m_rectID;
};
