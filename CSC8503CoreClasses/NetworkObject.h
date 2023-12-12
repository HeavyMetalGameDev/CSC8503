#pragma once
#include "GameObject.h"
#include "NetworkBase.h"
#include "NetworkState.h"
#include "ClientInputComponent.h"

namespace NCL::CSC8503 {
	class GameObject;

	struct FullPacket : public GamePacket {
		int		objectID = -1;
		NetworkState fullState;

		FullPacket() {
			type = Full_State;
			size = sizeof(FullPacket) - sizeof(GamePacket);
		}
	};

	struct DeltaPacket : public GamePacket {
		int		fullID		= -1;
		int		objectID	= -1;
		char	pos[3];
		char	orientation[4];

		DeltaPacket() {
			type = Delta_State;
			size = sizeof(DeltaPacket) - sizeof(GamePacket);
		}
	};

	struct ClientPacket : public GamePacket {
		int		objectID;
		char	buttonstates[8];
		//buttonstates[0] = W
		//buttonstates[1] = A
		//buttonstates[2] = S
		//buttonstates[3] = D
		//buttonstates[4] = Space
		//buttonstates[5] = Right Click
		//buttonstates[6] = Left Click
		//buttonstates[7] = E
		float camPitch;
		float camYaw;

		ClientPacket() {
			type = Client_State;
			size = sizeof(ClientPacket) - sizeof(GamePacket);
		}
	};

	class NetworkObject		{
	public:
		NetworkObject(GameObject& o, int id);
		virtual ~NetworkObject();

		//Called by clients
		virtual bool ReadPacket(GamePacket& p);
		//Called by servers
		virtual bool WritePacket(GamePacket** p, bool deltaFrame, int stateID);

		int GetNetworkID() { return networkID; }
		void UpdateStateHistory(int minID);
		virtual bool ReadDeltaPacket(DeltaPacket& p);
		virtual bool ReadFullPacket(FullPacket& p);
		virtual bool WriteDeltaPacket(GamePacket** p, int stateID);
		virtual bool WriteFullPacket(GamePacket** p);
		virtual bool WriteClientPacket(GamePacket** p,ClientInputComponent* c);

	protected:

		NetworkState& GetLatestNetworkState();

		bool GetNetworkState(int frameID, NetworkState& state);

		GameObject& object;

		NetworkState lastFullState;

		std::vector<NetworkState> stateHistory;

		int deltaErrors;
		int fullErrors;

		int networkID;
	};

}