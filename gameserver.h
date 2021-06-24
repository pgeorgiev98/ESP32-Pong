#ifndef GAMESERVER_H_INCLUDED
#define GAMESERVER_H_INCLUDED

#include "gamecommon.h"
#include "messenger.h"

#include <WiFi.h>

class GameServer : public Messenger {
public:
	GameServer();
	void begin();
	void tick();
	void onMessage(const Message &m) override;

private:
	static constexpr const char *m_ssid = "ESP32-Project";
	static constexpr const char *m_pass = "password";

	enum class State {
		Disabled,
		WaitingForConnection,
		Connected,
	};
	State m_state;

	WiFiServer m_server;
	WiFiClient m_client;
};

#endif
