#pragma once
#include "EventManager/EventManager.h"
#include "Engine.h"
namespace willengine
{
	class KeyPressedEvent : public Event
	{
	public:
		KeyPressedEvent(willengine::InputManager::Key key)
			:keyPressed(key)
		{
			
		}

		InputManager::Key keyPressed;
	private:
	};
}
