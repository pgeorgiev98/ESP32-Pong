#include "messagequeue.h"

MessageQueue::MessageQueue()
	: m_mutex(xSemaphoreCreateMutex())
{}

void MessageQueue::push(const Message &message) {
	xSemaphoreTake(m_mutex, portMAX_DELAY);
	m_queue.push(message);
	xSemaphoreGive(m_mutex);
}

bool MessageQueue::wait(Message &outMessage) {
	bool ok;
	xSemaphoreTake(m_mutex, portMAX_DELAY);
	ok = !m_queue.empty();
	if (ok) {
		outMessage = m_queue.front();
		m_queue.pop();
	}
	xSemaphoreGive(m_mutex);
	return ok;
}
