#pragma once
#include <GLFW/glfw3.h>
#include "imgui_impl_wgpu.h"
#include "../States.h"
#include <functional>                    
#include <Events/CreateEntityEvent.h>
#include <Events/SaveSceneEvent.h>
#include "../States.h"
#include "../App.h"
namespace willeditor
{
	class UI
	{
		class States;
	public:
		// Entity data from engine (updated each frame)
// In UI.h - replace the EntityDisplayInfo struct

        struct EntityDisplayInfo {
            std::string name;
            long id;

            // Transform
            bool hasTransform = false;
            float transformX = 0.0f;
            float transformY = 0.0f;

            // Rigidbody
            bool hasRigidbody = false;
            float rbPosX = 0.0f;
            float rbPosY = 0.0f;
            float rbVelX = 0.0f;
            float rbVelY = 0.0f;

            // Sprite
            bool hasSprite = false;
            std::string spriteImage;
            float spriteAlpha = 1.0f;
            float spriteScaleX = 1.0f;
            float spriteScaleY = 1.0f;

            // BoxCollider
            bool hasBoxCollider = false;
            float colliderWidth = 1.0f;
            float colliderHeight = 1.0f;

            // Health
            bool hasHealth = false;
            float healthAmount = 100.0f;

            // Script
            bool hasScript = false;
            std::string scriptName;
        };

		UI(App* app);
		~UI();

		void Startup(GLFWwindow* window, WGPUDevice device, WGPUTextureFormat render_target_format);		
		void Update();
		void Render();
		void Shutdown();
		
		void ShowMainToolbar();
		void ShowEntityCreatorWindow(bool* open);
		void ShowEntitiesList(bool* open);
		void ShowEntityComponents(bool* open);

		PlayState GetPlayState() const { return playState; }
		void SetPlayState(PlayState state) { playState = state; }

		void SetEntityList(const std::vector<EntityDisplayInfo>& entityList) {
			entities = entityList;
		}

		const std::string& GetSelectedEntity() const { return selectedEntityName; }
	private:
		App* app;
		static willeditor::EntityEditorState g_entityEditor;
		bool showEntityCreatorWindow;
		PlayState playState = Stopped;

		// For hierarchy window
		bool showHierarchyWindow = true;
		bool showInspectorWindow = true;
		std::string selectedEntityName = "";  // Currently selected entity


		std::vector<EntityDisplayInfo> entities;
	};

}
