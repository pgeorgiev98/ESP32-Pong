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
		None, Touch, Drag
	};

	struct Drag {
		Point start, end;
	};

	Touch();
	void poll();

	Event event() const {
		return m_event;
	}

	Point currentPoint() const {
		VERIFY(m_event != Event::None);
		return m_currentPoint;
	}

	Drag currentDrag() const {
		VERIFY(m_event == Event::Drag);
		return Drag { m_startPoint, m_currentPoint };
	}

private:
	Event m_event;
	Point m_startPoint;
	Point m_currentPoint;
};

#endif
