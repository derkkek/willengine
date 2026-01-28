#pragma once
#include <functional>    
#include "../App.h"
#include "Types.h"
#include <Events/CreateEntityEvent.h>
#include <Events/SaveSceneEvent.h>
namespace willeditor
{
	class App;

	using ModifyTransformCallback = std::function<void(const std::string& entityName, float x, float y)>;
	using ModifyRigidbodyCallback = std::function<void(const std::string& entityName, float posX, float posY, float velX, float velY)>;
	using ModifySpriteCallback = std::function<void(const std::string& entityName, const std::string& image, float alpha, float scaleX, float scaleY)>;
	using ModifyBoxColliderCallback = std::function<void(const std::string& entityName, float width, float height)>;
	using ModifyHealthCallback = std::function<void(const std::string& entityName, float amount)>;
	using ModifyScriptCallback = std::function<void(const std::string& entityName, const std::string& scriptName)>;


	struct EventCallbacks
	{
		std::function<void(const willengine::EntityCreationData&)> onCreateEntity;
		std::function<void()> onSaveScene;
		std::function<void()> onPlay;
		std::function<void()> onPause;
		std::function<void()> onStop;

		ModifyTransformCallback onModifyTransform;
		ModifyRigidbodyCallback onModifyRigidbody;
		ModifySpriteCallback onModifySprite;
		ModifyBoxColliderCallback onModifyBoxCollider;
		ModifyHealthCallback onModifyHealth;
		ModifyScriptCallback onModifyScript;
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

		ModifyTransformCallback OnModifyTransform;
		ModifyRigidbodyCallback OnModifyRigidbody;
		ModifySpriteCallback OnModifySprite;
		ModifyBoxColliderCallback OnModifyBoxCollider;
		ModifyHealthCallback OnModifyHealth;
		ModifyScriptCallback OnModifyScript;
	private:

		App* app;

	};

}