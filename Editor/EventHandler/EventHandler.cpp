#include "EventHandler.h"
#include <utility>
namespace willeditor
{
	EventHandler::EventHandler(App* app)
		:app(app)
	{

	}
	void EventHandler::BindEventCallbacks(EventCallbacks&& callbacks)
	{
		EngineEmitCreateEntityEventCallback = std::move(callbacks.onCreateEntity);
		EngineEmitSaveEntityCallback = std::move(callbacks.onSaveEntity);
		OnPlayClicked = std::move(callbacks.onPlay);
		OnPauseClicked = std::move(callbacks.onPause);
		OnStopClicked = std::move(callbacks.onStop);
	}
	void EventHandler::Startup(EventCallbacks&& callbacks)
	{
		BindEventCallbacks(std::move(callbacks));
	}

}