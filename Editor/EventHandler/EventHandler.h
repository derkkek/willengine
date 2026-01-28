#pragma once
#include <functional>    
#include "../App.h"
#include "Types.h"
#include <Events/CreateEntityEvent.h>
#include <Events/SaveSceneEvent.h>
namespace willeditor
{
	class App;

	struct EventCallbacks
	{
		std::function<void(const willengine::EntityCreationData&)> onCreateEntity;
		std::function<void()> onSaveScene;
		std::function<void()> onPlay;
		std::function<void()> onPause;
		std::function<void()> onStop;
	};

	class EventHandler
	{
	public:
		EventHandler(App* app);
		~EventHandler()=default;

		void Startup(EventCallbacks&& callbacks);

		void BindEventCallbacks(EventCallbacks&& callbacks);

		std::function<void(const willengine::EntityCreationData&)> EngineEmitCreateEntityEventCallback;
		std::function<void()> EngineEmitSaveSceneCallback;
		std::function<void()> OnPlayClicked;
		std::function<void()> OnPauseClicked;
		std::function<void()> OnStopClicked;
	private:

		App* app;

	};

}