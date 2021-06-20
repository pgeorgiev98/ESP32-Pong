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

	Event event() const {
		return m_event;
	}

	Point currentPoint() const {
		VERIFY(m_event == Event::Press || m_event == Event::Drag);
		return m_currentPoint;
	}

private:
	Event m_event;
	Point m_currentPoint;
};

#endif
