#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <Windows.h>
#include <DirectXMath.h>
#include <string>
#include <chrono>

namespace HelperFunctions
{
	bool Inside2DTriangle( DirectX::XMFLOAT2 p, DirectX::XMFLOAT2 p0, DirectX::XMFLOAT2 p1, DirectX::XMFLOAT2 p2 );
	float DistSquared( DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2 );
	DirectX::XMFLOAT3 GetCenter( DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 p3 );

	extern double PCFreq;
	extern __int64 CounterStart;
	void StartCounter();
	void PrintCounter( std::string text = "Result:" );
}
#endif