#include "./App.h"
#include "UserInterface/UI.h"
#include "EventHandler/EventHandler.h"
namespace willeditor
{
	App::App()
		:ui(new UI(this)), eventHandler(new EventHandler(this))
	{

	}
	void App::Startup(GLFWwindow* window, WGPUDevice device, WGPUTextureFormat render_target_format)
	{
		ui->Startup(window, device, render_target_format);
		ui->Update();

	}

	void App::Update()
	{
		ui->Update();
	}
	void App::Shutdown()
	{
		delete eventHandler;
		delete ui;
	}

}