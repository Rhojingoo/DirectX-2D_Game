#include "jkScene.h"

namespace jk
{
	Scene::Scene()
	{
		mLayers.resize((int)jk::enums::eLayerType::End);
	}
	Scene::~Scene()
	{

	}

	void Scene::Initialize()
	{
		// ���⼭ �ʱ� ���� �ʵ����͸� ��������� �Ѵ�.
	}

	void Scene::Update()
	{
		for (Layer& layer : mLayers)
		{
			layer.Update();
		}
	}

	void Scene::LateUpdate()
	{
		for (Layer& layer : mLayers)
		{
			layer.LateUpdate();
		}
	}
	void Scene::Render()
	{
		for (Layer& layer : mLayers)
		{
			layer.Render();
		}
	}
	void Scene::AddGameObject(eLayerType type, GameObject* gameObj)
	{
		mLayers[(int)type].AddGameObject(gameObj);
	}
}
