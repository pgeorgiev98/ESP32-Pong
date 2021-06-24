#ifndef GAMECLIENT_H_INCLUDED
#define GAMECLIENT_H_INCLUDED

#include "gamecommon.h"
#include "messenger.h"

#include <WiFi.h>

class GameClient : public Messenger {
public:
	GameClient();
	bool connect();
	void tick();
	void sendMessage(const Message &m);
	void onMessage(const Message &m, IPAddress ip) override;

private:
	WiFiUDP m_client;
};


#endif
