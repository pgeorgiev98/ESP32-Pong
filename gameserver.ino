#include "gameserver.h"
#include "gamecommon.h"
#include "messagequeue.h"

extern MessageQueue guiQueue;

GameServer::GameServer()
	: Messenger(m_client)
	, m_state(State::Disabled)
	, m_server(SERVER_PORT)
{}

void GameServer::begin() {
	WiFi.softAP(m_ssid, m_pass);
	m_server.begin();
	m_state = State::WaitingForConnection;
}

void GameServer::tick() {
	switch (m_state) {
	case State::WaitingForConnection: {
		if (m_server.hasClient()) {
			m_client = m_server.available();
			guiQueue.push(Message::joinedGame());
			m_state = State::Connected;
	  	}
		break;
	}

	case State::Connected: {
	   Messenger::poll();
	   break;
	}

	default: {}
	}
}

void GameServer::onMessage(const Message &m) {
	switch (m.type) {

	case Message::Type::PlayerMoved: {
		guiQueue.push(m);
		break;
	}

	default: {}

	}
}
