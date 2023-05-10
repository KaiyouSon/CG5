#pragma once
#include "NewEngine.h"

class TitleScene : public IScene
{
private:

public:
	void Init() override;
	void Update() override;
	void DrawRenderTexture() override;
	void DrawBackSprite() override;
	void DrawModel() override;
	void DrawFrontSprite() override;
	void DrawDebugGui() override;
};

