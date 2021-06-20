#include "touch.h"

#define xp 15
#define xm 12
#define yp 33
#define ym 13

#define NUMSAMPLES 3

#define DRAG_THRESHOLD 12

static int median(int arr[NUMSAMPLES]) {
	for (int i = 0; i < NUMSAMPLES - 1; ++i)
		if (arr[i] > arr[i + 1])
			std::swap(arr[i], arr[i + 1]);
	return arr[NUMSAMPLES / 2];
}

Touch::Touch()
	: m_event(Event::None)
{
}

void Touch::poll() {
	int x, y, z;
	int samples[NUMSAMPLES];
	uint8_t i;

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
		samples[i] = analogRead(xm) >> 2;

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

	int z1 = analogRead(xm) >> 2;
	int z2 = analogRead(yp) >> 2;

	z = (1023 - (z2 - z1));

	pinMode(xp, OUTPUT);
	pinMode(xm, OUTPUT);
	pinMode(yp, OUTPUT);
	pinMode(ym, OUTPUT);

	bool isTouched = (z > 150);
	Point point { int(x * (WIDTH/1024.0)), int(y * (HEIGHT/1024.0)) };

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
