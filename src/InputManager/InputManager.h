#pragma once
#include <unordered_map>
namespace willengine
{
	class Engine;
	class InputManager
	{
		public:
			enum Key 
			{
				W = 87,
				A = 65,
				S = 83,
				D = 68,
				ESC = 256
			};
			InputManager(Engine* engine);
			~InputManager();
			void Update();
			bool KeyIsPressedInFrame(Key key);
			bool KeyJustReleased(Key key);
			bool KeyJustPressed(Key key);

		private:
			Engine* engine;
			std::unordered_map<Key, int> prevFrameKeyStates;
	};
}
