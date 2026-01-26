#pragma once
#include <GLFW/glfw3.h>
#include "imgui_impl_wgpu.h"
#include "../States.h"
#include <functional>                    
#include <Events/CreateEntityEvent.h>
#include <Events/SaveEntityToConfigFileEvent.h>
#include "../States.h"
#include "../App.h"
namespace willeditor
{
	class UI
	{
		class States;
	public:
		UI(App* app);
		~UI();

		void Startup(GLFWwindow* window, WGPUDevice device, WGPUTextureFormat render_target_format);		
		void Update();
		void Render();
		void Shutdown();
		
		void ShowMainToolbar();
		void ShowEntityCreatorWindow(bool* open);

		PlayState GetPlayState() const { return playState; }
		void SetPlayState(PlayState state) { playState = state; }
	private:
		App* app;
		static willeditor::EntityEditorState g_entityEditor;
		bool showEntityCreatorWindow;
		PlayState playState = Stopped;
	};

}
