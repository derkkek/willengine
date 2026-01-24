#pragma once
#include <GLFW/glfw3.h>
#include "imgui_impl_wgpu.h"
#include "../States.h"
#include <functional>                    
#include <Events/CreateEntityEvent.h>
#include <Events/SaveEntityToConfigFileEvent.h>
#include "../States.h"
namespace willeditor
{
	class UI
	{
		class States;
	public:
		UI();
		~UI();

		void Startup(GLFWwindow* window, WGPUDevice device, WGPUTextureFormat render_target_format);		
		void Update();
		void Render();
		void Shutdown();
		
		void ShowMainToolbar();
		void ShowEntityCreatorWindow(bool* open);

		std::function<void(const willengine::EntityCreationData&)> EngineEmitCreateEntityEventCallback;
		std::function<void(const willengine::EntitySaveData&)> EngineEmitSaveEntityCallback;
		std::function<void()> OnPlayClicked;
		std::function<void()> OnPauseClicked;
		std::function<void()> OnStopClicked;

		PlayState GetPlayState() const { return playState; }
		void SetPlayState(PlayState state) { playState = state; }
	private:
		static willeditor::EntityEditorState g_entityEditor;
		bool showEntityCreatorWindow;
		PlayState playState = Stopped;
	};

}
