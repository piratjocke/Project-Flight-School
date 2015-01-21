#ifndef GRAPHICS_H
#define GRAPHICS_H

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

#include "AssetManager.h"
#include "Effect.h"
#include "CbufferPerFrame.h"
#include "CbufferPerObject.h"
#include "Camera.h"
#include "Gbuffer.h"
#include "..\ProjectFlightSchool\RenderStructs.h"

#ifdef COMPILE_LIBRARY
	#define LIBRARY_EXPORT __declspec( dllexport )
#else
	#define LIBRARY_EXPORT __declspec( dllimport )
#endif

#define NUM_GBUFFERS 3
#define MAX_ANIM_INSTANCE_BATCH 32

#define MAX_STATIC3D_INSTANCE_BATCH 512

class LIBRARY_EXPORT Graphics
{
	//Class members
	private:
		HWND						mHWnd;
		UINT						mScreenWidth;
		UINT						mScreenHeight;	
		ID3D11Buffer*				mVertexBuffer2d;

		IDXGISwapChain*				mSwapChain;
		ID3D11Device*				mDevice;
		ID3D11DeviceContext*		mDeviceContext;

		ID3D11RenderTargetView*		mRenderTargetView;
		ID3D11DepthStencilView*		mDepthStencilView;
		ID3D11DepthStencilState*	mDepthDisabledStencilState;
		ID3D11DepthStencilState*	mDepthEnabledStencilState;
		D3D11_VIEWPORT				mStandardView;
		ID3D11Buffer*				mCbufferPerFrame;
		ID3D11Buffer*				mCbufferPerObject;
		ID3D11Buffer*				mCbufferPerObjectAnimated;
		ID3D11Buffer*				mCbufferPerInstancedAnimated;
		ID3D11SamplerState*			mPointSamplerState;
		ID3D11SamplerState*			mLinearSamplerState;


		AssetManager*				mAssetManager;
		Effect*						mStaticEffect;
		Effect*						mStaticInstancedEffect;
		Effect*						m2dEffect;
		Effect*						mAnimatedEffect;
		Effect*						mAnimInstancedEffect;
		Camera*						mCamera;
		Camera*						mDeveloperCamera;
		bool						mIsDeveloperCameraActive;
		Effect*						mDeferredPassEffect;
		Gbuffer*					mGbuffers[NUM_GBUFFERS];
		ID3D11Buffer*				mBufferPerInstanceObject;

		StaticInstance mStatic3dInstanced[MAX_STATIC3D_INSTANCE_BATCH];
		CbufferPerObjectAnimated mAnim3dInstanced[MAX_ANIM_INSTANCE_BATCH];


	protected:
	public:

	//Class functions
	private:
		Graphics();
		virtual	~Graphics();

		HRESULT MapBuffer( ID3D11Buffer* buffer, void* data, int size );

	protected:
	public:
		HRESULT LoadStatic2dAsset( std::string fileName, AssetID &assetId );
		HRESULT LoadStatic3dAsset( std::string filePath, std::string fileName, AssetID &assetId );
		HRESULT LoadStatic3dAssetIndexed( Indexed3DAssetInfo &info, AssetID &assetId );
		HRESULT LoadAnimated3dAsset( std::string filePath, std::string fileName, AssetID skeletonId, AssetID &assetId ); 
		HRESULT LoadSkeletonAsset( std::string filePath, std::string fileName, AssetID &assetId );
		HRESULT LoadAnimationAsset( std::string filePath, std::string fileName, AssetID &assetId );

		void Render( RenderLists& renderLists );
		void Render2dAsset( AssetID assetId, float x, float y, float width, float height );
		void RenderPlane2dAsset( AssetID assetId, DirectX::XMFLOAT3 x, DirectX::XMFLOAT3 y );
		void RenderPlane2dAsset( PlaneInfo* info, UINT sizeOfList );
		void RenderStatic3dAsset( AssetID assetId, DirectX::XMFLOAT4X4* world );
		void RenderStatic3dAsset( Object3dInfo* info, UINT sizeOfList );
		void RenderAnimated3dAsset( Anim3dInfo* info, UINT sizeOfList );

		DirectX::XMFLOAT4X4 GetRootMatrix( AssetID modelAssetId, AssetID animationAssetId, float animationTime );

		Camera* GetCamera() const;
		Camera* GetDeveloperCamera() const;
		void	ChangeCamera();
		void	ZoomInDeveloperCamera();
		void	ZoomOutDeveloperCamera();

		void SetNDCSpaceCoordinates( float &mousePositionX, float &mousePositionY );
		void SetInverseViewMatrix( DirectX::XMMATRIX &inverseViewMatrix );
		void SetInverseProjectionMatrix( DirectX::XMMATRIX &projectionViewMatrix );
		void SetEyePosition( DirectX::XMFLOAT3 &eyePosition );
		void SetFocus( DirectX::XMFLOAT3 &focusPoint );

		void	BeginScene();
		void	EndScene();

		void GetAnimationMatrices( AssetID modelAssetId, AssetID animationAssetId, float &animationTime, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, Anim3dInfo& info );

		UINT QueryMemoryUsed();

		static	Graphics* GetInstance();
		HRESULT Initialize( HWND hWnd, UINT screenWidth, UINT screenHeight );
		void	Release();
};
#endif