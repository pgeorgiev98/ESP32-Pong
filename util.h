#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

class Timer {
public:
	Timer() { reset(); }
	void reset() { m_startTime = millis(); }
	unsigned long time() const { return millis() - m_startTime; }

private:
	unsigned long m_startTime;
};

#endif
