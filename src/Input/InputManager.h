#pragma once

namespace willengine
{
	class Engine;
	class InputManager
	{
		public:
			InputManager(Engine* engine);
			~InputManager();

		private:

			Engine* engine;
	};
}
