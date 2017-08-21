#pragma once

#include "TrioAPI.h"

#include "PresentationParameters.h"
#include "ResourceEnums.h"

#include "PrimitiveType.h"
#include "GraphicsCapabilities.h"

#include "Event.h"
#include "VertexBufferBinding.h"

#include "StatesEnums.h"
#include <vector>
#include <memory>

#include <wrl/client.h>
namespace Cuado
{

	class TRIOAPI_DLL GraphicsAdapter;
	class TRIOAPI_DLL BlendState;
	class TRIOAPI_DLL DepthStencilState;
	class TRIOAPI_DLL IndexBuffer;
	class TRIOAPI_DLL VertexBuffer;
	class TRIOAPI_DLL RasterizerState;
	class TRIOAPI_DLL SamplerState;
	class TRIOAPI_DLL VertexDeclaration;
	class TRIOAPI_DLL TextureCollection;
	class TRIOAPI_DLL SamplerStateCollection;
	class TRIOAPI_DLL DepthStencilBuffer;
	class TRIOAPI_DLL Shader;
	class TRIOAPI_DLL ConstantBufferCollection;
	class TRIOAPI_DLL ConstantBuffer;
	
	struct TRIOAPI_DLL VertexBufferBinding;

	class TRIOAPI_DLL GraphicsDevice
	{
	public:
		GraphicsDevice(GraphicsAdapter* adapter, PresentationParameters parameters);
		~GraphicsDevice();

		void Clear(DirectX::SimpleMath::Color &color);
		void Clear(ClearOptions options, DirectX::SimpleMath::Color &color, float depth, uint8_t stencil);
		
		void DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount);
		void DrawPrimitives(PrimitiveType primitiveType, int vertexStart, int primitiveCount);

		//template <class T>
		//void DrawUserIndexedPrimitives(PrimitiveType primitiveType, const T* vertexData, int lengthVertex, int vertexOffset, int numVertices, const uint32_t* indexData, int lengthIndex, int indexOffset, int primitiveCount, VertexDeclaration *vertexDeclaration);

		//template <class T>
		//void DrawUserIndexedPrimitives(PrimitiveType primitiveType, const T* vertexData, int lengthVertex, int vertexOffset, int numVertices, const uint16_t* indexData, int lengthIndex, int indexOffset, int primitiveCount, VertexDeclaration *vertexDeclaration);

		//template <class T>
		//void DrawUserPrimitives(PrimitiveType primitiveType, const T* vertexData, int vertexSizeArray, int vertexOffset, int primitiveCount); //where T: struct, IVertexType

		//template <class T>
		//void DrawUserPrimitives(PrimitiveType primitiveType, const T* vertexData, int vertexSizeArray, int vertexOffset, int primitiveCount, VertexDeclaration* declaration); //where T: struct, IVertexType

		GraphicsAdapter*           GetAdapter() const { return m_adapter; }
#ifdef TRIO_DIRECTX
		ID3D11Device*           GetD3DDevice() const						{ return m_d3dDevice.Get(); }
		ID3D11Device1*          GetD3DDevice1() const						{ return m_d3dDevice1.Get(); }
		ID3D11DeviceContext*    GetD3DDeviceContext() const					{ return m_d3dContext.Get(); }
		ID3D11DeviceContext1*   GetD3DDeviceContext1() const				{ return m_d3dContext1.Get(); }
		IDXGISwapChain*         GetSwapChain() const						{ return m_swapChain.Get(); }
		IDXGISwapChain1*        GetSwapChain1() const						{ return m_swapChain1.Get(); }
		D3D_FEATURE_LEVEL       GetDeviceFeatureLevel() const				{ return m_d3dFeatureLevel; }
		ID3D11RenderTargetView*	GetBackBufferRenderTargetView() const		{ return m_d3dRenderTargetView.Get(); }
		//ID3D11DepthStencilView* GetDepthStencilView() const					{ return m_depthStencilBuffer->m_d3dDepthStencilView.Get(); }

		D3D11_VIEWPORT          GetScreenViewport() const					{ return m_screenViewport; }
		UINT                    GetBackBufferCount() const					{ return m_backBufferCount; }
#endif

		inline GraphicsCapabilities GetGraphicsCapabilities()				{ return m_graphicsCapabilities; }
		inline TextureCollection* GetTextures()								{ return m_pTextureCollection; }
		inline PresentationParameters& GetPresentationParameters()			{ return m_presentationParameters; }

		void Present();

		void SetBlendState(BlendState* state);
		void SetDepthStencilState(DepthStencilState* state);
		void SetIndexBuffer(IndexBuffer *indexBuffer);
		void SetVertexBuffer(VertexBuffer *buffer);
		void SetVertexBuffer(VertexBufferBindings &bindings);

		void SetRasterizerState(RasterizerState* state);
		//void SetSamplerState(int index, SamplerState* state);

		void SetVertexShader(Shader* shader);
		void SetPixelShader(Shader* shader);
		//void SetViewport(DirectX::SimpleMath::Viewport viewport);

		void Reset(PresentationParameters presentationParameters);
		void Reset(PresentationParameters presentationParameters, GraphicsAdapter* graphicsAdapter);

		Event<> DeviceLost;

		friend class GraphicsDeviceManager;
		friend class EffectPass;
		friend class VertexDeclaration;
		friend class Game;

	private:
		GraphicsCapabilities						m_graphicsCapabilities;
		TextureCollection*							m_pTextureCollection;
		SamplerStateCollection*						m_pSamplerCollection;
		PresentationParameters						m_presentationParameters;
		GraphicsAdapter*							m_adapter;

		std::unique_ptr<DepthStencilBuffer> m_depthStencilBuffer;
		DepthStencilBuffer* m_currentDepthStencilBuffer;
		
		IndexBuffer* m_pIndexBuffer;
		bool m_bIndexBufferDirty;

		VertexBuffer* m_pVertexBuffer;
		bool m_bVertexBufferDirty;
		VertexBufferBindings m_vVertexBindings;

		Shader* m_pVertexShader;
		bool m_bVertexShaderDirty;

		Shader* m_pPixelShader;
		bool m_bPixelShaderDirty;

		DepthStencilState* m_DepthStencilState;
		bool m_DepthStencilStateDirty;

		BlendState* m_BlendState;
		bool m_BlendStateDirty;

		RasterizerState* m_RasterizerState;
		bool m_RasterizerStateDirty;

		ConstantBufferCollection* m_pVertexConstantBuffers;
		ConstantBufferCollection* m_pPixelConstantBuffers;

		UINT                                            m_backBufferCount;
#ifdef TRIO_DIRECTX
		Microsoft::WRL::ComPtr<ID3D11Device>				m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11Device1>				m_d3dDevice1;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_d3dContext;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1>		m_d3dContext1;
		Microsoft::WRL::ComPtr<IDXGISwapChain>				m_swapChain;
		Microsoft::WRL::ComPtr<IDXGISwapChain1>				m_swapChain1;
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;

		// Direct3D rendering objects. Required for 3D.
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_d3dRenderTargetView;
		D3D11_VIEWPORT                                  m_screenViewport;

		std::vector<ID3D11RenderTargetView *>			m_currentRenderTargets;

		D3D_FEATURE_LEVEL                               m_d3dMinFeatureLevel;
		D3D_FEATURE_LEVEL                               m_d3dFeatureLevel;

		static const int MaxVertexBuffers = 16;
		ID3D11Buffer*	m_aVertexBuffers[MaxVertexBuffers];
		uint32_t		m_aVertexOffsets[MaxVertexBuffers];
		uint32_t		m_aVertexStrides[MaxVertexBuffers];

#endif
		void ApplyState(bool applyShaders);
		bool AreSameVertexBindings(VertexBufferBindings &bindings);
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		void SetConstantBuffer(ShaderStage stage, int slot, ConstantBuffer* buffer);
	};
}