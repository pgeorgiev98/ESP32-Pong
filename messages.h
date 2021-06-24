#ifndef MESSAGES_H_INCLUDED
#define MESSAGES_H_INCLUDED

struct Message {
	enum class Type {
		HostGame,
		JoinGame,
		JoinedGame,
		FailedToJoin,

		PlayerMoved,
	};

	struct JoinGameData {
		int ssid;
	};

	struct PlayerMovedData {
		int position;
	};

	Type type;

	union Data {
		JoinGameData joinGame;
		PlayerMovedData playerMoved;
	} data;

	static Message hostGame() { return Message{Type::HostGame}; }
	static Message joinGame(int ssid) { return Message{Type::JoinGame, Data{.joinGame = JoinGameData{ssid}}}; }
	static Message joinedGame() { return Message{Type::JoinedGame}; }
	static Message failedToJoin() { return Message{Type::FailedToJoin}; }
	static Message playerMoved(int position) { return Message{Type::PlayerMoved, Data{.playerMoved = PlayerMovedData{position}}}; }
};

#endif
