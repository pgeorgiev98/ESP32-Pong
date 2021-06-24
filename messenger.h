#ifndef MESSENGER_H_INCLUDED
#define MESSENGER_H_INCLUDED

#include "messages.h"

class Messenger {
public:
	Messenger(WiFiClient &client) : m_client(client) {}

	void poll() {
		while (m_client.available() >= sizeof(Message)) {
			Message m;
			m_client.read(reinterpret_cast<uint8_t *>(&m), sizeof(Message));
			onMessage(m);
		}
	}

	void sendMessage(const Message &m) {
		const uint8_t *data = reinterpret_cast<const uint8_t *>(&m);
		m_client.write(data, sizeof(Message));
	}

	virtual void onMessage(const Message &m) = 0;

private:
	WiFiClient &m_client;
};

#endif
