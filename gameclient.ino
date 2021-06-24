#include "gameclient.h"
#include "gamecommon.h"

GameClient::GameClient()
	: Messenger(m_client)
{}

bool GameClient::connect() {
	m_connected = m_client.connect(IPAddress(192, 168, 4, 1), SERVER_PORT);
	return r;
}

void GameClient::tick() {
	if (m_connected)
		Messenger::poll();
}

void GameClient::onMessage(const Message &m) {
	switch (m.type) {

	case Message::Type::PlayerMoved: {
		guiQueue.push(m);
		break;
	}

	default: {}

	}
}
