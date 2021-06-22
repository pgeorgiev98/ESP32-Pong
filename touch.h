#ifndef TOUCH_H_INCLUDED
#define TOUCH_H_INCLUDED

#include "debug.h"
#include "geometry.h"

// Vertical
// RS - 15
// D0 - 12
// Horizontal
// CS - 33
// D1 - 13

class Touch {
public:
	enum class Event {
		None, Press, Drag, Release,
	};

	Touch();
	void poll();
	void loadSettings();
	void saveSettings();

	Event event() const {
		return m_event;
	}

	Point currentPoint() const {
		VERIFY(m_event == Event::Press || m_event == Event::Drag);
		return m_currentPoint;
	}

	void setSendRawData(bool sendRawData) {
		m_sendRawData = sendRawData;
	}

	void calibrate(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom) {
		m_left = left;
		m_right = right;
		m_top = top;
		m_bottom = bottom;
		saveSettings();
	}

private:
	Event m_event;
	Point m_currentPoint;
	bool m_sendRawData;
	uint32_t m_left, m_right, m_top, m_bottom;
};

extern Touch touch;

#endif
