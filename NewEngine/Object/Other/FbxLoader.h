﻿//#pragma once
//#include "Singleton.h"
//#include "FbxModel.h"
//#include <fbxsdk.h>
//#include <string>
//
//#include <d3d12.h>
//#include <d3dx12.h>
//
//template<typename T> class Singleton;
//
//class FbxLoader : public Singleton<FbxLoader>
//{
//private:
//	FbxManager* fbxManager = nullptr;
//	FbxImporter* fbxImporter = nullptr;
//
//private:
//	static const std::string baseDirectory;
//	static const std::string defaultTextureFileName;
//
//private:
//	void ParseMesh(FbxModel* fbxModel, FbxNode* fbxNode);
//	void ParseMeshVertices(FbxModel* fbxModel, FbxMesh* fbxMesh);
//	void ParseMeshFaces(FbxModel* fbxModel, FbxMesh* fbxMesh);
//	void ParseMaterial(FbxModel* fbxModel, FbxNode* fbxNode);
//	void ParseSkin(FbxModel* fbxModel, FbxMesh* fbxMesh);
//
//public:
//	void Init();
//	void Destroy();
//
//	void ParseNodeRecursive(FbxModel* fbxModel, FbxNode* fbxNode, FbxModelNode* parent = nullptr);
//
//	// ディレクトリーを含んだファイルパスからファイル名を抽出する
//	std::string ExractFileName(const std::string& path);
//
//	inline FbxManager* GetFbxManager() { return fbxManager; }
//	inline FbxImporter* GetFbxImporter() { return fbxImporter; }
//private:
//	friend Singleton<FbxLoader>;
//};