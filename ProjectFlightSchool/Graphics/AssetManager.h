#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "Static2dAsset.h"
#include "Static3dAsset.h"
#include "Static3dAssetIndexed.h"
#include "Animated3dAsset.h"
#include "SkeletonAsset.h"
#include "AnimationAsset.h"
#include "Vertex.h"
//#include "ImporterAnim.h"
//#include "MapPathImportHandler.h"
#include <vector>
#include <fstream>
#include <iostream>
//#include <string>

struct MeshInfo
{
	UINT nrOfVertices;

	char meshName[30];
	char diffuseMapName[30];
	char normalMapName[30];
	char specularMapName[30];

	char skeletonName[30];
};
struct Indexed3DAssetInfo
{
	UINT vertexCount;
	UINT indexCount;

	StaticVertex* vertices;
	UINT* indices;
	char* assetName;
};
class AssetManager
{
	private:
		AssetID mAssetIdCounter;
	protected:
	public:
		std::vector<AssetBase*> mAssetContainer;

	private:		
		bool			AssetExist( std::string fileName, AssetID &assetId );	//Returns true and assigns the correct id to assetId if the asset exist.	
		void			AssignAssetId( AssetID &assetId );					//Assigns the asset an id and increase the mAssetIdCounter.
		HRESULT			PlaceholderAssets( ID3D11Device* device, ID3D11DeviceContext* dc );
		HRESULT			LoadTextureFromFile ( ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* fileName, ID3D11Resource** texture, ID3D11ShaderResourceView** srv, size_t size = 0 );
		std::wstring	StringToWstring( std::string fileName );
		HRESULT			CreateSRVAssets( ID3D11Device* device, ID3D11DeviceContext* dc, std::string filePath, MeshInfo &meshInfo, AssetID &assetId );

	protected:
	public:
		HRESULT			LoadStatic2dAsset( ID3D11Device* device, ID3D11DeviceContext* dc, std::string fileName, AssetID &assetId );
		HRESULT			LoadStatic3dAsset( ID3D11Device* device, ID3D11DeviceContext* dc, std::string filePath, std::string fileName, AssetID &assetId );
		HRESULT			LoadStatic3dAssetIndexed( ID3D11Device* device, Indexed3DAssetInfo &info, AssetID &assetId );
		HRESULT			LoadAnimated3dAsset( ID3D11Device* device, ID3D11DeviceContext* dc, std::string filePath, std::string fileName, AssetID skeletonId, AssetID &assetId );
		HRESULT			LoadSkeletonAsset( std::string filePath, std::string fileName, AssetID &assetId );
		HRESULT			LoadAnimationAsset( std::string filePath, std::string fileName, AssetID &assetId );
		HRESULT			Initialize( ID3D11Device* device, ID3D11DeviceContext* dc );

		void		Release();
					AssetManager();
		virtual		~AssetManager();

};
#endif