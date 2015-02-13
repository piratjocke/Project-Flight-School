#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include "LightManager.h"
#include "RenderStructs.h"
#include "ParticleManager.h"

class RenderManager
{
	private:
		Object3dInfo	mObject3dArray[MAX_AMOUNT_OF_OBJECT3D];
		Object2dInfo	mObject2dArray[MAX_AMOUNT_OF_OBJECT2D];
		Anim3dInfo		mAnim3dArray[MAX_AMOUNT_OF_ANIM3D];
		PlaneInfo		mPlaneArray[MAX_AMOUNT_OF_PLANES];
		BillboardInfo	mBillboardArray[MAX_AMOUNT_OF_OBJECT2D];
		ParticleInfo	mParticleInfoArray[MAX_NR_OF_PARTICLES];
		NodeGridInfo	mNodeGridArray[MAX_AMOUNT_OF_NODEGRIDS];
		BoxInfo			mBoxArray[MAX_AMOUNT_OF_BOXES];

		LightManager*		mLightManager;
		ParticleManager*	mParticleManager;

	protected:
	public:
		UINT mNrOfObject3d;
		UINT mNrOfAnim3d;
		UINT mNrOfObject2d;
		UINT mNrOfPlane;
		UINT mNrOfBillboard;
		UINT mNrOfParticles;
		UINT mNrOfNodeGrid;
		UINT mNrOfBoxes;

	private:
		void SetLightStructuredBuffer();
		void Clear();
		RenderManager();
		virtual~RenderManager();
	protected:
	public:
		void AddObject3dToList( AssetID assetId, DirectX::XMFLOAT3 position = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ), DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
		void AddObject3dToList( AssetID assetId, DirectX::XMFLOAT4X4 world );
		void AddObject2dToList( AssetID assetId, DirectX::XMFLOAT2 topLeftCorner, DirectX::XMFLOAT2 widthHeight, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
		void AddBoxToList( DirectX::XMFLOAT3 min, DirectX::XMFLOAT3 max );
		bool AddAnim3dToList( AnimationTrack &animTrack, int playType, DirectX::XMFLOAT3 position = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ), DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
		void AddPlaneToList( AssetID assetId, DirectX::XMFLOAT3 topTriangle, DirectX::XMFLOAT3 bottomTriangle );
		void AddBillboardToList( AssetID assetId, DirectX::XMFLOAT3 worldPosition, float width, float height );
		void AddParticleSystemToList( ParticleSystem*** particleSystem,  int* nrOfActiveParticleSystemsPerType );
		void AddNodeGridToList( StaticVertex* vertices, UINT nrOfVertices, DirectX::XMFLOAT4X4 world );

		void AnimationInitialize( AnimationTrack &animationTrack, AssetID model, AssetID defaultAnimation );
		void AnimationUpdate( AnimationTrack &animationTrack, float deltaTime );
		void AnimationStartNew( AnimationTrack &animationTrack, AssetID newAnimation );
		void AnimationReset( AnimationTrack &animationTrack, AssetID defaultAnimation );

		void RequestParticleSystem( size_t entityID, ParticleType particleType, XMFLOAT3 position, XMFLOAT3 direction );

		HRESULT Update( float deltaTime );	//Currently clearing the arrays to standard values and reseting number of active objects variable for them in update
		HRESULT Render();	
		HRESULT Initialize();
		void Release();
		static RenderManager* GetInstance();


};

#endif