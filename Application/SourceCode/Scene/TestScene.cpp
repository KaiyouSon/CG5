#include "TestScene.h"
#include "ShaderObjectManager.h"

void TestScene::CreateGraphicsPipeline()
{
	D3D12_DEPTH_STENCIL_DESC  depthStencilDesc1{};
	depthStencilDesc1.DepthEnable = true; // 深度テストを行う
	depthStencilDesc1.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	// 書き込み許可
	depthStencilDesc1.DepthFunc = D3D12_COMPARISON_FUNC_LESS;	// 小さいほうを採用

	std::string path = "NewEngine/Shader/";
	// Phoneシェーダー
	ShaderObjectManager::Create("Phone");
	ShaderObjectManager::GetShaderObject("Phone")->AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	ShaderObjectManager::GetShaderObject("Phone")->AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	ShaderObjectManager::GetShaderObject("Phone")->AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	ShaderObjectManager::GetShaderObject("Phone")->CompileVertexShader(path + "Object3DVS.hlsl", "main");
	ShaderObjectManager::GetShaderObject("Phone")->CompilePixelShader(path + "PhonePS.hlsl", "main");

	// Phone用
	GraphicsPipelineManager::Create(
		ShaderObjectManager::GetShaderObject("Phone"),
		RenderBase::GetInstance()->GetObject3DRootSignature()->GetRootSignature(),
		CullMode::CullBack,
		depthStencilDesc1,
		TopologyType::TriangleTopology,
		2,
		"Phone");

	// Toonシェーダー
	ShaderObjectManager::Create("Toon");
	ShaderObjectManager::GetShaderObject("Toon")->AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	ShaderObjectManager::GetShaderObject("Toon")->AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	ShaderObjectManager::GetShaderObject("Toon")->AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	ShaderObjectManager::GetShaderObject("Toon")->CompileVertexShader(path + "Object3DVS.hlsl", "main");
	ShaderObjectManager::GetShaderObject("Toon")->CompilePixelShader(path + "ToonPS.hlsl", "main");

	// Toon用
	GraphicsPipelineManager::Create(
		ShaderObjectManager::GetShaderObject("Toon"),
		RenderBase::GetInstance()->GetObject3DRootSignature()->GetRootSignature(),
		CullMode::CullBack,
		depthStencilDesc1,
		TopologyType::TriangleTopology,
		2,
		"Toon");

	// Toonシェーダー
	ShaderObjectManager::Create("RimLight");
	ShaderObjectManager::GetShaderObject("RimLight")->AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	ShaderObjectManager::GetShaderObject("RimLight")->AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	ShaderObjectManager::GetShaderObject("RimLight")->AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	ShaderObjectManager::GetShaderObject("RimLight")->CompileVertexShader(path + "Object3DVS.hlsl", "main");
	ShaderObjectManager::GetShaderObject("RimLight")->CompilePixelShader(path + "RimLightPS.hlsl", "main");

	// Toon用
	GraphicsPipelineManager::Create(
		ShaderObjectManager::GetShaderObject("RimLight"),
		RenderBase::GetInstance()->GetObject3DRootSignature()->GetRootSignature(),
		CullMode::CullBack,
		depthStencilDesc1,
		TopologyType::TriangleTopology,
		2,
		"RimLight");
}

TestScene::TestScene()
{
}
TestScene::~TestScene()
{
}

void TestScene::Init()
{
	Camera::current.pos = { 0,1,-15 };
	Camera::current.rot = { Radian(0),0,0 };

	// ライティング有効
	Object3D::isAllLighting = true;

	LightManager::GetInstance()->directionalLights[0].isActive = true;
	LightManager::GetInstance()->directionalLights[0].dirVec = { 0,-1,0 };

	// パイプライン生成
	CreateGraphicsPipeline();

	phoneObj.pos.x = -1;
	phoneObj.model = ModelManager::GetModel("Sphere");
	phoneObj.graphicsPipeline = GraphicsPipelineManager::GetGraphicsPipeline("Phone");

	toonObj.pos.x = +1;
	toonObj.model = ModelManager::GetModel("Sphere");
	toonObj.graphicsPipeline = GraphicsPipelineManager::GetGraphicsPipeline("Toon");

	rimLightObj.pos.x = +3;
	rimLightObj.model = ModelManager::GetModel("Sphere");
	rimLightObj.graphicsPipeline = GraphicsPipelineManager::GetGraphicsPipeline("RimLight");

	//obj1.obj = &obj;
	//obj1.isOutLine = false;
}
void TestScene::Update()
{
	Camera::DebugCameraUpdate();

	phoneObj.Update();
	toonObj.Update();
	rimLightObj.Update();
}

void TestScene::DrawRenderTexture()
{
}
void TestScene::DrawBackSprite()
{
}
void TestScene::DrawModel()
{
	phoneObj.Draw();
	//toonObj.Draw();
	//rimLightObj.Draw();
}
void TestScene::DrawFrontSprite()
{

}
void TestScene::DrawDebugGui()
{
	GuiManager::BeginWindow("Lighting");
	GuiManager::DrawSlider3("lightVec", LightManager::GetInstance()->directionalLights[0].dirVec, 0.01f);
	GuiManager::EndWindow();

	GuiManager::BeginWindow("Camera");
	GuiManager::DrawSlider3("Camera Pos", Camera::current.pos, 0.01f);
	GuiManager::EndWindow();

	// フォン
	GuiManager::BeginWindow("Phone Object");

	GuiManager::DrawSlider3("Pos", phoneObj.pos, 0.01f);

	GuiManager::DrawColorEdit("PhoneObj BaseColor", phoneObj.color);

	Color ambient1;
	ambient1 = phoneObj.ambient * 255.f;
	GuiManager::DrawColorEdit("PhoneObj AmbientColor", ambient1);
	phoneObj.ambient = ambient1 / 255.f;

	Color diffuse1;
	diffuse1 = phoneObj.diffuse * 255.f;
	GuiManager::DrawColorEdit("PhoneObj DiffuseColor", diffuse1);
	phoneObj.diffuse = diffuse1 / 255.f;

	Color specular1;
	specular1 = phoneObj.specular * 255.f;
	GuiManager::DrawColorEdit("PhoneObj SpecularColor", specular1);
	phoneObj.specular = specular1 / 255.f;

	GuiManager::EndWindow();

	// トゥーン
	GuiManager::BeginWindow("Toon Object");

	GuiManager::DrawSlider3("Pos", toonObj.pos, 0.01f);

	GuiManager::DrawColorEdit("ToonObj BaseColor", toonObj.color);

	Color ambient2;
	ambient2 = toonObj.ambient * 255.f;
	GuiManager::DrawColorEdit("ToonObj AmbientColor", ambient2);
	toonObj.ambient = ambient2 / 255.f;

	Color diffuse2;
	diffuse2 = toonObj.diffuse * 255.f;
	GuiManager::DrawColorEdit("ToonObj DiffuseColor", diffuse2);
	toonObj.diffuse = diffuse2 / 255.f;

	//Color specular2;
	//specular2 = toonObj.specular * 255.f;
	//GuiManager::DrawColorEdit("ToonObj SpecularColor", specular2);
	//toonObj.specular = specular2 / 255.f;

	GuiManager::EndWindow();

	// リムライト
	GuiManager::BeginWindow("RimLight Object");

	GuiManager::DrawSlider3("Pos", rimLightObj.pos, 0.01f);

	GuiManager::DrawColorEdit("RimLightObj BaseColor", rimLightObj.color);

	Color ambient3;
	ambient3 = rimLightObj.ambient * 255.f;
	GuiManager::DrawColorEdit("RimLightObj AmbientColor", ambient3);
	rimLightObj.ambient = ambient3 / 255.f;

	Color diffuse3;
	diffuse3 = rimLightObj.diffuse * 255.f;
	GuiManager::DrawColorEdit("RimLightObj DiffuseColor", diffuse3);
	rimLightObj.diffuse = diffuse3 / 255.f;

	//Color specular2;
	//specular2 = toonObj.specular * 255.f;
	//GuiManager::DrawColorEdit("ToonObj SpecularColor", specular2);
	//toonObj.specular = specular2 / 255.f;

	GuiManager::EndWindow();

}