#include "LoadManager.h"
#include "NewEngine.h"
#include "FbxLoader.h"

void LoadManager::ModelLoad()
{
	ModelManager::LoadObjModel("Sphere", "Sphere", true);
	ModelManager::LoadObjModel("Block1", "Block1");
}

void LoadManager::TextureLoad()
{
	TextureManager::CreateTexture(Color::white, "White");

	TextureManager::ExcuteComandList();
}

void LoadManager::SoundLoad()
{
}

void LoadManager::Load()
{
	TextureLoad();
	ModelLoad();
	SoundLoad();
}
