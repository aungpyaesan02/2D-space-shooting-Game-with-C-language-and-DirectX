//=============================================================================
//

//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"


D3DXMATRIX		cameraViewMatrix;
D3DXMATRIX		cameraInvViewMatrix;
D3DXMATRIX		cameraProjectionMatrix;

D3DXVECTOR3		cameraPos;
D3DXVECTOR3		cameraTarget;
D3DXVECTOR3		cameraRot;


D3DXMATRIX GetCameraViewMatrix()
{
	return cameraViewMatrix;
}

D3DXMATRIX GetCameraInvViewMatrix()
{
	return cameraInvViewMatrix;
}

D3DXMATRIX GetCameraProjectionMatrix()
{
	return cameraProjectionMatrix;
}

D3DXVECTOR3 GetCameraPosition()
{
	return cameraPos;
}

//=============================================================================
// 初期化処理
//=============================================================================
void InitCamera(void)
{
	cameraPos		= D3DXVECTOR3( 0.0f, 50.0f, -100.0f );
	cameraTarget	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	cameraRot		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitCamera(void)
{


}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateCamera(void)
{
	//if( GetKeyState( VK_LEFT ) & 0x80 )
	//{
	//	cameraRot.y -= 0.02f;
	//}
	//else if( GetKeyState( VK_RIGHT ) & 0x80 )
	//{
	//	cameraRot.y += 0.02f;
	//}


	D3DXVECTOR3 playerPosition;

	playerPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// GetPlayerPosition();

	cameraTarget.x = playerPosition.x * 0.5f;
	cameraTarget.z = playerPosition.z * 0.5f;

	cameraPos.x = cameraTarget.x + sinf( cameraRot.y ) * 100.0f;
	cameraPos.z = cameraTarget.z - cosf( cameraRot.y ) * 100.0f;

}


//=============================================================================
// 描画処理
//=============================================================================
void SetCamera(void)
{
	// ビューマトリックス設定
	D3DXMatrixLookAtLH( &cameraViewMatrix, &cameraPos, &cameraTarget, &D3DXVECTOR3(0.0f, 1.0f, 0.0f) );

	SetViewMatrix( &cameraViewMatrix );


	float det;
	D3DXMatrixInverse(&cameraInvViewMatrix, &det, &cameraViewMatrix);


	// プロジェクションマトリックス設定
	D3DXMatrixPerspectiveFovLH( &cameraProjectionMatrix, 1.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);

	SetProjectionMatrix( &cameraProjectionMatrix );
}