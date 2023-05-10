#pragma once
#include "NewEngine.h"

class TestScene : public IScene
{
private:
	Object3D phoneObj;
	Object3D toonObj;
	Object3D rimLightObj;

private:
	void CreateGraphicsPipeline();

public:
	TestScene();
	~TestScene();
	void Init() override;
	void Update() override;
	void DrawRenderTexture() override;
	void DrawBackSprite() override;
	void DrawModel() override;
	void DrawFrontSprite() override;
	void DrawDebugGui() override;
};

