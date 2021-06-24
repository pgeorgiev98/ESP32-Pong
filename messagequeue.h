#ifndef MESSAGEQUEUE_H_INCLUDED
#define MESSAGEQUEUE_H_INCLUDED

#include "messages.h"

#include <queue>

class MessageQueue {
public:
	MessageQueue();
	void push(const Message &message);
	bool wait(Message &outMessage);

private:
	SemaphoreHandle_t m_mutex;
	std::queue<Message> m_queue;
};

#endif
