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
		EngineEmitSaveSceneCallback = std::move(callbacks.onSaveScene);
		OnPlayClicked = std::move(callbacks.onPlay);
		OnPauseClicked = std::move(callbacks.onPause);
		OnStopClicked = std::move(callbacks.onStop);
		OnModifyTransform = std::move(callbacks.onModifyTransform);
		OnModifySprite = std::move(callbacks.onModifySprite);
		OnModifyRigidbody = std::move(callbacks.onModifyRigidbody);
		OnModifyBoxCollider = std::move(callbacks.onModifyBoxCollider);
		OnModifyHealth = std::move(callbacks.onModifyHealth);
		OnModifyScript = std::move(callbacks.onModifyScript);


	}
	void EventHandler::Startup(EventCallbacks&& callbacks)
	{
		BindEventCallbacks(std::move(callbacks));
	}

}