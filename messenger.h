#ifndef MESSENGER_H_INCLUDED
#define MESSENGER_H_INCLUDED

#include "messages.h"
#include "gamecommon.h"

class Messenger {
public:
	Messenger(WiFiUDP &client) : m_client(client) {}

	void begin() {
		m_client.begin(SERVER_PORT);
	}

	void poll() {
		for (;;) {
			int packetSize = m_client.parsePacket();
			if (packetSize == 0)
				break;
			IPAddress ip = m_client.remoteIP();
			Message m;
			m_client.read(reinterpret_cast<uint8_t *>(&m), sizeof(Message));
			onMessage(m, ip);
		}
	}

	void sendMessage(IPAddress ip, const Message &m) {
		const uint8_t *data = reinterpret_cast<const uint8_t *>(&m);
		m_client.beginPacket(ip, SERVER_PORT);
		m_client.write(data, sizeof(Message));
		m_client.endPacket();
	}

	virtual void onMessage(const Message &m, IPAddress ip) = 0;

private:
	WiFiUDP &m_client;
};

#endif
