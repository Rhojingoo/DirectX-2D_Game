#pragma once
#include "..\Engine_SOURCE\jkSceneManager.h"
#include "jkPlayScene.h"
#include "jkTitleScene.h"
#include "jkStage1.h"
#include "jkToolScene.h"


#ifdef _DEBUG
#pragma comment(lib, "..\\x64\\Debug\\JKEngine.lib")
#else
#pragma comment(lib, "..\\x64\\Release\\JKEngine.lib")
#endif




namespace jk
{
	void InitializeScenes(HWND hWnd)
	{		
		//Tool 
		SceneManager::CreateScene<ToolScene>(L"ToolScene", hWnd);
		//Scene
		SceneManager::CreateScene<jkStage1>(L"Stage1", hWnd);
		SceneManager::CreateScene<PlayScene>(L"PlayScene", hWnd);
		SceneManager::CreateScene<jkTitleScene>(L"TitleScene", hWnd);

	}
}