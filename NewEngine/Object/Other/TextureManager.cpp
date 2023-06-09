#include "TextureManager.h"
#include "RenderBase.h"
#include <DirectXTex.h>
#include <d3dx12.h>
using namespace DirectX;

UINT TextureManager::srvIncrementIndex = 1;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> TextureManager::srvDescHeap;
std::map<std::string, std::unique_ptr<Texture>> TextureManager::textureMap;
std::map<std::string, std::unique_ptr<RenderTexture>> TextureManager::renderTextureMap;

Texture* TextureManager::GetTexture(const std::string& textureTag)
{
	return textureMap[textureTag].get();
}

Texture TextureManager::CreateTexture(const Color& color)
{
	Texture tex;

	HRESULT result;

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = 1; // 幅
	textureResourceDesc.Height = 1; // 高さ
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	// テクスチャのサイズをセット
	tex.size = { (float)textureResourceDesc.Width, (float)textureResourceDesc.Height };

	// テクスチャバッファの生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&tex.buffer));
	assert(SUCCEEDED(result));

	CreateSRV(tex, tex.buffer.Get());

	Color col = Color(color.r / 255, color.g / 255, color.b / 255, color.a / 255);

	D3D12_SUBRESOURCE_DATA subResourcesData{};
	subResourcesData.pData = (void**)&col;
	subResourcesData.RowPitch = (LONG_PTR)(sizeof(Color) * tex.size.x);
	subResourcesData.SlicePitch = (LONG_PTR)(sizeof(Color) * tex.size.x * tex.size.y);

	uint64_t uploadSize = GetRequiredIntermediateSize(tex.buffer.Get(), 0, 1);

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC textureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// テクスチャバッファの生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer));
	assert(SUCCEEDED(result));

	UpdateSubresources(
		RenderBase::GetInstance()->GetCommandList(),
		tex.buffer.Get(),
		uploadBuffer.Get(),
		0,
		0,
		1,
		&subResourcesData);

	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = tex.buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	ExcuteComandList();

	return tex;
}
Texture* TextureManager::CreateTexture(const Color& color, const std::string& textureTag)
{
	textureMap.insert(std::make_pair(textureTag, std::move(std::make_unique<Texture>())));

	HRESULT result;

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = 1; // 幅
	textureResourceDesc.Height = 1; // 高さ
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	// テクスチャのサイズをセット
	textureMap[textureTag]->size = { (float)textureResourceDesc.Width, (float)textureResourceDesc.Height };

	// テクスチャバッファの生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&textureMap[textureTag]->buffer));
	assert(SUCCEEDED(result));

	CreateSRV(*textureMap[textureTag], textureMap[textureTag]->buffer.Get());


	Color col = Color(color.r / 255, color.g / 255, color.b / 255, color.a / 255);

	D3D12_SUBRESOURCE_DATA subResourcesData{};
	subResourcesData.pData = (void**)&col;
	subResourcesData.RowPitch = (LONG_PTR)(sizeof(Color) * textureMap[textureTag]->size.x);
	subResourcesData.SlicePitch = (LONG_PTR)(sizeof(Color) * textureMap[textureTag]->size.x * textureMap[textureTag]->size.y);

	uint64_t uploadSize = GetRequiredIntermediateSize(textureMap[textureTag]->buffer.Get(), 0, 1);

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC textureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	// テクスチャバッファの生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureMap[textureTag]->uploadBuffer));
	assert(SUCCEEDED(result));

	UpdateSubresources(
		RenderBase::GetInstance()->GetCommandList(),
		textureMap[textureTag]->buffer.Get(),
		textureMap[textureTag]->uploadBuffer.Get(),
		0,
		0,
		1,
		&subResourcesData);

	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = textureMap[textureTag]->buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	return textureMap[textureTag].get();
}
Texture TextureManager::LoadTexture(const std::string& filePath)
{
	Texture tex;

	std::string path = "Application/Resources/Texture/" + filePath;

	TexMetadata metadata{};
	ScratchImage scratchImg{};
	std::wstring wfilePath(path.begin(), path.end());

	HRESULT result;

	// WICテクスチャのロード
	result = LoadFromWICFile(
		wfilePath.c_str(),
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	if (result != S_OK)
	{
		assert(0 && "テクスチャーの読み込みが失敗しました");
	}

	// ミップマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(),
		scratchImg.GetImageCount(),
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// ヒープの設定
	CD3DX12_HEAP_PROPERTIES textureHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// リソース設定
	CD3DX12_RESOURCE_DESC textureResourceDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			(UINT64)metadata.width,
			(UINT)metadata.height,
			(UINT16)metadata.arraySize,
			(UINT16)metadata.mipLevels,
			1);

	// テクスチャのサイズをセット
	tex.size = { (float)textureResourceDesc.Width, (float)textureResourceDesc.Height };

	// テクスチャバッファの生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&tex.buffer));
	assert(SUCCEEDED(result));

	CreateSRV(tex, tex.buffer.Get());

	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	for (size_t i = 0; i < subResourcesDatas.size(); i++)
	{
		// 全ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);

		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = img->rowPitch;
		subResourcesDatas[i].SlicePitch = img->slicePitch;
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex.buffer.Get(), 0, (UINT)metadata.mipLevels);

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC textureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// テクスチャバッファの生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer));
	assert(SUCCEEDED(result));

	UpdateSubresources(
		RenderBase::GetInstance()->GetCommandList(),
		tex.buffer.Get(),
		uploadBuffer.Get(),
		0,
		0,
		(UINT)metadata.mipLevels,
		subResourcesDatas.data());

	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = tex.buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	ExcuteComandList();

	return tex;
}
Texture* TextureManager::LoadTexture(const std::string& filePath, const std::string& textureTag)
{
	textureMap.insert(std::make_pair(textureTag, std::move(std::make_unique<Texture>())));

	std::string path = "Application/Resources/Texture/" + filePath;

	TexMetadata metadata{};
	ScratchImage scratchImg{};
	std::wstring wfilePath(path.begin(), path.end());

	HRESULT result;

	// WICテクスチャのロード
	result = LoadFromWICFile(
		wfilePath.c_str(),
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	if (result != S_OK)
	{
		assert(0 && "テクスチャーの読み込みが失敗しました");
	}

	// ミップマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(),
		scratchImg.GetImageCount(),
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// ヒープの設定
	CD3DX12_HEAP_PROPERTIES textureHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// リソース設定
	CD3DX12_RESOURCE_DESC textureResourceDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			(UINT64)metadata.width,
			(UINT)metadata.height,
			(UINT16)metadata.arraySize,
			(UINT16)metadata.mipLevels,
			1);

	// テクスチャのサイズをセット
	textureMap[textureTag]->size = { (float)textureResourceDesc.Width, (float)textureResourceDesc.Height };

	// テクスチャバッファの生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&textureMap[textureTag]->buffer));
	assert(SUCCEEDED(result));

	CreateSRV(*textureMap[textureTag], textureMap[textureTag]->buffer.Get());

	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	for (size_t i = 0; i < subResourcesDatas.size(); i++)
	{
		// 全ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);

		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = img->rowPitch;
		subResourcesDatas[i].SlicePitch = img->slicePitch;
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(textureMap[textureTag]->buffer.Get(), 0, (UINT)metadata.mipLevels);

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC textureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	// テクスチャバッファの生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureMap[textureTag]->uploadBuffer));
	assert(SUCCEEDED(result));

	UpdateSubresources(
		RenderBase::GetInstance()->GetCommandList(),
		textureMap[textureTag]->buffer.Get(),
		textureMap[textureTag]->uploadBuffer.Get(),
		0,
		0,
		(UINT)metadata.mipLevels,
		subResourcesDatas.data());

	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = textureMap[textureTag]->buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	return textureMap[textureTag].get();
}
Texture TextureManager::LoadMaterialTexture(const std::string& filePath)
{
	Texture tex;

	TexMetadata metadata{};
	ScratchImage scratchImg{};
	std::wstring wfilePath(filePath.begin(), filePath.end());

	HRESULT result;

	// WICテクスチャのロード
	result = LoadFromWICFile(
		wfilePath.c_str(),
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	if (result != S_OK)
	{
		assert(0 && "テクスチャーの読み込みが失敗しました");
	}

	// ミップマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(),
		scratchImg.GetImageCount(),
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// ヒープの設定
	CD3DX12_HEAP_PROPERTIES textureHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// リソース設定
	CD3DX12_RESOURCE_DESC textureResourceDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			(UINT64)metadata.width,
			(UINT)metadata.height,
			(UINT16)metadata.arraySize,
			(UINT16)metadata.mipLevels,
			1);

	// テクスチャのサイズをセット
	tex.size = { (float)textureResourceDesc.Width, (float)textureResourceDesc.Height };

	// テクスチャバッファの生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&tex.buffer));
	assert(SUCCEEDED(result));

	CreateSRV(tex, tex.buffer.Get());

	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	for (size_t i = 0; i < subResourcesDatas.size(); i++)
	{
		// 全ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);

		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = img->rowPitch;
		subResourcesDatas[i].SlicePitch = img->slicePitch;
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex.buffer.Get(), 0, (UINT)metadata.mipLevels);

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC textureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	// テクスチャバッファの生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&tex.uploadBuffer));
	assert(SUCCEEDED(result));

	UpdateSubresources(
		RenderBase::GetInstance()->GetCommandList(),
		tex.buffer.Get(),
		tex.uploadBuffer.Get(),
		0,
		0,
		(UINT)metadata.mipLevels,
		subResourcesDatas.data());

	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = tex.buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	return tex;
}

RenderTexture* TextureManager::GetRenderTexture(const std::string& textureTag)
{
	return renderTextureMap[textureTag].get();
}
RenderTexture* TextureManager::CreateRenderTexture(const Vec2& size, const size_t num, const std::string& textureTag)
{
	std::unique_ptr<RenderTexture> renderTex = std::make_unique<RenderTexture>();
	renderTex->buffers.resize(num);

	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };

	HRESULT result;
	RenderBase* renderBase = RenderBase::GetInstance();

	// ヒープ設定
	CD3DX12_HEAP_PROPERTIES texturenResourceHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// リソース設定
	CD3DX12_RESOURCE_DESC texturenResourceDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			(UINT64)size.x, (UINT)size.y,
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	CD3DX12_CLEAR_VALUE textureResourceClearValue =
		CD3DX12_CLEAR_VALUE(
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			clearColor);

	renderTex->size = size;

	for (int i = 0; i < num; i++)
	{
		result = renderBase->GetDevice()->
			CreateCommittedResource(
				&texturenResourceHeapProp,
				D3D12_HEAP_FLAG_NONE,
				&texturenResourceDesc,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				&textureResourceClearValue,
				IID_PPV_ARGS(&renderTex->buffers[i]));
		assert(SUCCEEDED(result));
	}

	// レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	// シェーダーの計算結果をSRGBに変換して書き込む
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	renderTex->renderTargets.resize(num);
	for (int i = 0; i < num; i++)
	{
		// SRV作成
		CreateSRV(*renderTex, renderTex->buffers[i].Get());

		// RTV作成
		renderTex->renderTargets[i].buffer = renderTex->buffers[i];
		renderBase->CreateRTV(renderTex->renderTargets[i], &rtvDesc);
	}

	// DSV作成
	renderTex->depthBuffer.Create();
	renderBase->CreateDSV(renderTex->depthBuffer);

	renderTextureMap.insert(std::make_pair(textureTag, std::move(renderTex)));

	return renderTextureMap[textureTag].get();
}

void TextureManager::CreateDescriptorHeap()
{
	HRESULT result;

	// --- SRV ------------------------------------------------------ //
	const size_t maxSRVCount = 2056;	// SRVの最大個数

	// SRV用デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// シェーダから見えるように
	srvHeapDesc.NumDescriptors = maxSRVCount;

	// SRV用デスクリプタヒープを生成
	result = RenderBase::GetInstance()->GetDevice()->
		CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvDescHeap));
	assert(SUCCEEDED(result));

}
void TextureManager::CreateSRV(Texture& texture, ID3D12Resource* buffer)
{
	// SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = srvDescHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvDescHeap->GetGPUDescriptorHandleForHeapStart();

	UINT descriptorSize = RenderBase::GetInstance()->GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvCpuHandle.ptr += (SIZE_T)(descriptorSize * srvIncrementIndex);
	srvGpuHandle.ptr += (SIZE_T)(descriptorSize * srvIncrementIndex);

	texture.SetCpuHandle(srvCpuHandle);
	texture.SetGpuHandle(srvGpuHandle);

	// シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	// srv設定構造体
	srvDesc.Format = buffer->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = buffer->GetDesc().MipLevels;

	// ハンドルの指す位置にシェーダーリソースビュー作成
	RenderBase::GetInstance()->GetDevice()->
		CreateShaderResourceView(buffer, &srvDesc, srvCpuHandle);

	srvIncrementIndex++;
}

void TextureManager::ExcuteComandList()
{
	ID3D12GraphicsCommandList* iCommandList = RenderBase::GetInstance()->GetCommandList();

	iCommandList->Close();

	ID3D12CommandQueue* iCommandQueue = RenderBase::GetInstance()->GetCommandQueue();

	ID3D12CommandList* list[] = { iCommandList };
	iCommandQueue->ExecuteCommandLists(1, list);

	RenderBase::GetInstance()->PreIncrimentFenceValue();

	// コマンドの実行完了を待つ
	iCommandQueue->Signal(RenderBase::GetInstance()->GetFence(), RenderBase::GetInstance()->GetFenceValue());

	auto test = RenderBase::GetInstance()->GetFence()->GetCompletedValue();
	if (RenderBase::GetInstance()->GetFence()->GetCompletedValue() != RenderBase::GetInstance()->GetFenceValue())
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		RenderBase::GetInstance()->GetFence()->SetEventOnCompletion(RenderBase::GetInstance()->GetFenceValue(), event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	HRESULT result;

	// キューをクリア
	result = RenderBase::GetInstance()->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(result));
	// 再びコマンドリストを貯める準備
	result = iCommandList->Reset(RenderBase::GetInstance()->GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(result));
}
