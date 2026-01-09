#pragma once

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
			bool KeyIsPressed(Key key);

		private:
			Engine* engine;
	};
}
