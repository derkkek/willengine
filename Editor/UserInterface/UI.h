#pragma once
#include <GLFW/glfw3.h>
#include "imgui_impl_wgpu.h"
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

		void ShowEntityCreatorWindow(bool* open);

	private:
		static willeditor::EntityEditorState g_entityEditor;
		bool showEntityCreatorWindow;
	};

}
