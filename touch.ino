#include "touch.h"

#include <Preferences.h>


#define xp 15
#define xm 12
#define yp 33
#define ym 13

#define xm_2 34

#define NUMSAMPLES 3

#define DRAG_THRESHOLD 12

static Preferences preferences;

static int median(int arr[NUMSAMPLES]) {
	for (int i = 0; i < NUMSAMPLES - 1; ++i)
		if (arr[i] > arr[i + 1])
			std::swap(arr[i], arr[i + 1]);
	return arr[NUMSAMPLES / 2];
}

Touch::Touch()
	: m_event(Event::None)
	, m_sendRawData(false)
	, m_left(0)
	, m_right(0)
	, m_top(0)
	, m_bottom(0)
{
}

void Touch::poll() {
	int x, y, z;
	int samples[NUMSAMPLES];
	uint8_t i;

	pinMode(xm_2, INPUT);
	digitalWrite(xm_2, LOW);

	pinMode(yp, INPUT);
	pinMode(ym, INPUT);

	digitalWrite(yp, LOW);
	digitalWrite(ym, LOW);

	pinMode(xp, OUTPUT);
	pinMode(xm, OUTPUT);
	digitalWrite(xp, HIGH);
	digitalWrite(xm, LOW);

	for (i = 0; i < NUMSAMPLES; i++)
		samples[i] = analogRead(yp) >> 2;
	x = (1023 - median(samples)); //samples[NUMSAMPLES / 2]); //choose median

	pinMode(xp, INPUT);
	pinMode(xm, INPUT);
	digitalWrite(xp, LOW);
	digitalWrite(xm, LOW);

	pinMode(yp, OUTPUT);
	digitalWrite(yp, HIGH);
	pinMode(ym, OUTPUT);
	digitalWrite(ym, LOW);

	for (i = 0; i < NUMSAMPLES; i++)
		samples[i] = analogRead(xm_2) >> 2;

	y = (1023 - median(samples));

	// Set X+ to ground
	pinMode(xp, OUTPUT);
	digitalWrite(xp, LOW);

	// Set Y- to VCC
	pinMode(ym, OUTPUT);
	digitalWrite(ym, HIGH);

	// Hi-Z X- and Y+
	digitalWrite(xm, LOW);
	pinMode(xm, INPUT);
	digitalWrite(yp, LOW);
	pinMode(yp, INPUT);

	int z1 = analogRead(xm_2) >> 2;
	int z2 = analogRead(yp) >> 2;

	z = (1023 - (z2 - z1));

	pinMode(xp, OUTPUT);
	pinMode(xm, OUTPUT);
	pinMode(yp, OUTPUT);
	pinMode(ym, OUTPUT);

	bool isTouched = (z > 150);
	Point point;
	if (m_sendRawData)
		point = Point { x, y };
	else if (m_left + m_right + m_top + m_bottom == 0)
		point = Point { int(x * (WIDTH/1024.0)), int(y * (HEIGHT/1024.0)) };
	else
		point = Point { map(x, m_left, m_right, 0, WIDTH), map(y, m_top, m_bottom, 0, HEIGHT) };

	if (isTouched) {
		switch (m_event) {
		case Event::None:
		case Event::Release:
			m_event = Event::Press;
			break;

		case Event::Press:
			if (m_currentPoint.distanceSquared(point) < DRAG_THRESHOLD * DRAG_THRESHOLD)
				point = m_currentPoint;
			m_event = Event::Drag;
			break;

		case Event::Drag:
			break;
		}
		m_currentPoint = point;
	} else {
		switch (m_event) {
		case Event::None:
		case Event::Release:
			m_event = Event::None;
			break;

		case Event::Press:
		case Event::Drag:
			m_event = Event::Release;
			break;
		}
	}
}

void Touch::loadSettings() {
	preferences.begin("Touch", true);
	m_left   = preferences.getULong("left",   0);
	m_right  = preferences.getULong("right",  0);
	m_top    = preferences.getULong("top",    0);
	m_bottom = preferences.getULong("bottom", 0);
	preferences.end();
}

void Touch::saveSettings() {
	preferences.begin("Touch", false);
	preferences.putULong("left",   m_left);
	preferences.putULong("right",  m_right);
	preferences.putULong("top",    m_top);
	preferences.putULong("bottom", m_bottom);
	preferences.end();
}
