#include "gameclient.h"
#include "gamecommon.h"

static const IPAddress SERVER_IP = IPAddress(192, 168, 4, 1);

GameClient::GameClient()
	: Messenger(m_client)
{}

void GameClient::tick() {
	Messenger::poll();
}

void GameClient::sendMessage(const Message &m) {
	Messenger::sendMessage(SERVER_IP, m);
}

void GameClient::onMessage(const Message &m, IPAddress ip) {
	if (ip == SERVER_IP) {
		switch (m.type) {

		case Message::Type::PlayerMoved: {
			guiQueue.push(m);
			break;
		}

		case Message::Type::BallMoved: {
			guiQueue.push(m);
			break;
		}

		default: {}

		}
	}
}
