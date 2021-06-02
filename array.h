#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

#include "debug.h"

template<typename Type, int MaxSize>
class Array {
public:
	Array() : m_size(0) {}

	int size() const {
		return m_size;
	}

	bool isEmpty() const {
		return m_size == 0;
	}

	void append(const Type &value) {
		VERIFYMSG(m_size < MaxSize, "Array overflow");
		m_data[m_size++] = value;
	}

	void removeLast() {
		VERIFYMSG(isEmpty(), "Array underflow");
		--m_size;
	}

	void clear() {
		m_size = 0;
	}

	const Type &operator[](int index) const {
		return m_data[index];
	}

	Type &operator[](int index) {
		return m_data[index];
	}

private:
	Type m_data[MaxSize];
	int m_size;
};

#endif
