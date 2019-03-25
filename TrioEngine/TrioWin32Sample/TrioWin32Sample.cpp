// TrioWin32Sample.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TrioWin32Sample.h"

#include "Graphics/Effect.h"
#include "Graphics/EffectParameter.h"
#include "Graphics/EffectTechnique.h"
#include "Graphics/EffectPass.h"
#include "Graphics/GraphicsDeviceManager.h"
#include "Graphics/TextureCollection.h"

#include "Graphics/Texture2D.h"
#include "Utils/TextureLoader.h"

namespace TrioWin32
{
	DemoGame::DemoGame() :
		Game(), m_pIndexBuffer(nullptr)
	{
		mRadius = 5.0f;
		GraphicsDeviceManager* graphicsManager = new GraphicsDeviceManager(this);
	}

	void DemoGame::Initialize()
	{
		Game::Initialize();
		Magic();
	}

	void DemoGame::Magic() {
		m_pEffect = new Effect(GetGraphicsDevice());
		m_pEffect->CompileEffectFromFile("LineEffect.fx");

		m_pIndexBuffer = new IndexBuffer(GetGraphicsDevice(), IndexElementSize::ThirtyTwoBits, 36, ResourceUsage::Immutable);

		uint32_t indices[] = {
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 6, 5,
			4, 7, 6,

			// left face
			4, 5, 1,
			4, 1, 0,

			// right face
			3, 2, 6,
			3, 6, 7,

			// top face
			1, 5, 6,
			1, 6, 2,

			// bottom face
			4, 0, 3,
			4, 3, 7
		};

		m_pIndexBuffer->SetData<uint32_t>(indices, 36);

		Color whiteColor(1, 1, 1);
		Color blackColor(0, 0, 0);
		Color redColor(1, 0, 0);
		Color greenColor(0, 1, 0);
		Color blueColor(0, 0, 1);
		Color yellowColor(1, 0, 1);
		Color cyanColor(0, 1, 1);
		Color magentaColor(0, 0.5f, 1);

		VertexPositionColorTexture vertexs[] =
		{
			{ Vector3(-1.0f, -1.0f, -1.0f), whiteColor,		Vector2(0.0f, 0.0f) },
			{ Vector3(-1.0f, +1.0f, -1.0f), blackColor,		Vector2(0.0f, 1.0f) },
			{ Vector3(+1.0f, +1.0f, -1.0f), redColor,		Vector2(1.0f, 1.0f) },
			{ Vector3(+1.0f, -1.0f, -1.0f), greenColor,		Vector2(1.0f, 0.0f) },
			{ Vector3(-1.0f, -1.0f, +1.0f), blueColor,		Vector2(0.0f, 0.0f) },
			{ Vector3(-1.0f, +1.0f, +1.0f), yellowColor,	Vector2(0.0f, 1.0f) },
			{ Vector3(+1.0f, +1.0f, +1.0f), cyanColor,		Vector2(1.0f, 1.0f) },
			{ Vector3(+1.0f, -1.0f, +1.0f), magentaColor,	Vector2(1.0f, 0.0f) }
		};
/*
		VertexPositionColor vertexs[] =
		{
			{ Vector3(-1.0f, -1.0f, -1.0f), whiteColor },
			{ Vector3(-1.0f, +1.0f, -1.0f), blackColor },
			{ Vector3(+1.0f, +1.0f, -1.0f), redColor },
			{ Vector3(+1.0f, -1.0f, -1.0f), greenColor },
			{ Vector3(-1.0f, -1.0f, +1.0f), blueColor },
			{ Vector3(-1.0f, +1.0f, +1.0f), yellowColor },
			{ Vector3(+1.0f, +1.0f, +1.0f), cyanColor },
			{ Vector3(+1.0f, -1.0f, +1.0f), magentaColor }
		};*/

		//Vector3 tmpConst = vertexs[0].GetPosition();
		//tmpConst.x = 4.0f;
		int sizeVPC = sizeof(VertexPositionColorTexture);
		m_pVertexBuffer = new VertexBuffer(GetGraphicsDevice(), VertexPositionColorTexture::GetVertexDeclaration(), 8);
		m_pVertexBuffer->SetData<VertexPositionColorTexture>(vertexs, 8);

		//m_pVertexBuffer = new VertexBuffer(GetGraphicsDevice(), VertexPositionColor::GetVertexDeclaration(), 8);
		//m_pVertexBuffer->SetData<VertexPositionColor>(vertexs, 8);

		TextureLoader texLoader;
		TextureLoader::ImageInfo imgInfo = texLoader.GetTextureFromFile("edgar.jpg");

		Texture2D* tex2D = new Texture2D(GetGraphicsDevice(), imgInfo.Width, imgInfo.Height, SurfaceFormat::Color);
		tex2D->SetData<uint8_t>(imgInfo.Data, imgInfo.SizeOfData);

		GetGraphicsDevice()->GetTextures()->SetTexture(0, tex2D);
	}

	void DemoGame::Draw(StepTimer const & timer)
	{
		//using namespace DirectX::SimpleMath;
		Color LightSteelBlue(0.69f, 0.77f, 0.87f);

		auto device = GetGraphicsDevice();
		device->Clear(LightSteelBlue);

		device->SetIndexBuffer(m_pIndexBuffer);
		device->SetVertexBuffer(m_pVertexBuffer);

		////device->SetDepthStencilState(DepthStencilState::Default);
		////device->SetBlendState(BlendState::Opaque);
		device->SetRasterizerState(RasterizerState::CullClockwise);
		device->SetSamplerState(0, SamplerState::LinearWrap);

		Matrix view;
		Matrix proj;
		Matrix viewProj;

		float ratio = (float)device->GetPresentationParameters().GetBackBufferWidth() / device->GetPresentationParameters().GetBackBufferHeight();
		proj = Matrix::CreatePerspectiveFieldOfView(0.25f * 3.1415926535f, ratio, 0.5f, 100.0f);

		Vector3 zero(0, 0, 0);
		Vector3 up(0, 1, 0);

		view = Matrix::CreateLookAt(m_position, zero, up);

		viewProj = view * proj;

		m_pEffect->GetParameters()[0]->SetValue(viewProj);
		m_pEffect->GetTechniques()[0]->GetPasses()[0]->Apply();

		device->DrawIndexedPrimitives(PrimitiveType::TriangleList, 0, 0, 8, 0, 12);
	}

	void DemoGame::Update(StepTimer const& timer)
	{
		//std::cout << "Update: dt = " << timer.GetElapsedSeconds() << "\t Total seconds: " << timer.GetTotalSeconds() << endl;

		float mTheta = 1.5f * 3.141592654f;
		float mPhi = 0.25f * 3.141592654f;
		float x = mRadius * (float)sinf(mPhi) * (float)cosf(mTheta);
		float z = mRadius * (float)sinf(mPhi) * (float)sinf(mTheta);
		float y = mRadius * (float)cosf(mPhi);

		m_position = Vector3(x, y, z);
	}
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	std::unique_ptr<TrioWin32::DemoGame> game(new TrioWin32::DemoGame());
	game->Run();

	return 0;
}

//#define MAX_LOADSTRING 100
//
//// Global Variables:
//HINSTANCE hInst;                                // current instance
//WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
//WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
//
//// Forward declarations of functions included in this code module:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//                     _In_opt_ HINSTANCE hPrevInstance,
//                     _In_ LPWSTR    lpCmdLine,
//                     _In_ int       nCmdShow)
//{
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//
//    // TODO: Place code here.
//
//    // Initialize global strings
//    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
//    LoadStringW(hInstance, IDC_TRIOWIN32SAMPLE, szWindowClass, MAX_LOADSTRING);
//    MyRegisterClass(hInstance);
//
//    // Perform application initialization:
//    if (!InitInstance (hInstance, nCmdShow))
//    {
//        return FALSE;
//    }
//
//    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRIOWIN32SAMPLE));
//
//    MSG msg;
//
//    // Main message loop:
//    while (GetMessage(&msg, nullptr, 0, 0))
//    {
//        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//    }
//
//    return (int) msg.wParam;
//}
//
//
//
////
////  FUNCTION: MyRegisterClass()
////
////  PURPOSE: Registers the window class.
////
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//    WNDCLASSEXW wcex;
//
//    wcex.cbSize = sizeof(WNDCLASSEX);
//
//    wcex.style          = CS_HREDRAW | CS_VREDRAW;
//    wcex.lpfnWndProc    = WndProc;
//    wcex.cbClsExtra     = 0;
//    wcex.cbWndExtra     = 0;
//    wcex.hInstance      = hInstance;
//    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRIOWIN32SAMPLE));
//    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
//    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
//    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TRIOWIN32SAMPLE);
//    wcex.lpszClassName  = szWindowClass;
//    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
//    return RegisterClassExW(&wcex);
//}
//
////
////   FUNCTION: InitInstance(HINSTANCE, int)
////
////   PURPOSE: Saves instance handle and creates main window
////
////   COMMENTS:
////
////        In this function, we save the instance handle in a global variable and
////        create and display the main program window.
////
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   hInst = hInstance; // Store instance handle in our global variable
//
//   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
//
//   if (!hWnd)
//   {
//      return FALSE;
//   }
//
//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   return TRUE;
//}
//
////
////  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
////
////  PURPOSE: Processes messages for the main window.
////
////  WM_COMMAND  - process the application menu
////  WM_PAINT    - Paint the main window
////  WM_DESTROY  - post a quit message and return
////
////
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message)
//    {
//    case WM_COMMAND:
//        {
//            int wmId = LOWORD(wParam);
//            // Parse the menu selections:
//            switch (wmId)
//            {
//            case IDM_ABOUT:
//                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//                break;
//            case IDM_EXIT:
//                DestroyWindow(hWnd);
//                break;
//            default:
//                return DefWindowProc(hWnd, message, wParam, lParam);
//            }
//        }
//        break;
//    case WM_PAINT:
//        {
//            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hWnd, &ps);
//            // TODO: Add any drawing code that uses hdc here...
//            EndPaint(hWnd, &ps);
//        }
//        break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}
//
//// Message handler for about box.
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(lParam);
//    switch (message)
//    {
//    case WM_INITDIALOG:
//        return (INT_PTR)TRUE;
//
//    case WM_COMMAND:
//        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//        {
//            EndDialog(hDlg, LOWORD(wParam));
//            return (INT_PTR)TRUE;
//        }
//        break;
//    }
//    return (INT_PTR)FALSE;
//}
