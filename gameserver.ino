#include "gameserver.h"
#include "gamecommon.h"
#include "messagequeue.h"

extern MessageQueue guiQueue;

GameServer::GameServer()
	: Messenger(m_client)
	, m_state(State::Disabled)
{}

void GameServer::begin() {
	WiFi.softAP(m_ssid, m_pass);
	m_state = State::WaitingForConnection;
	m_clientIP = IPAddress();
	Messenger::begin();
}

void GameServer::tick() {
   Messenger::poll();
}

void GameServer::sendMessage(const Message &m) {
	VERIFY(m_state == State::Connected);
	Messenger::sendMessage(m_clientIP, m);
}

void GameServer::onMessage(const Message &m, IPAddress ip) {
	switch (m.type) {

	case Message::Type::JoinedGame: {
		if (m_state == State::WaitingForConnection) {
			m_clientIP = ip;
			guiQueue.push(Message::joinedGame());
			m_state = State::Connected;
		}
		break;
	}

	case Message::Type::PlayerMoved: {
		if (m_state == State::Connected && ip == m_clientIP) {
			guiQueue.push(m);
		}
		break;
	}

	default: {}

	}
}
