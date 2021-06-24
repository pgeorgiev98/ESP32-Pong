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
	void onMessage(const Message &m) override;

private:
	WiFiClient m_client;
	bool m_connected;
};


#endif
