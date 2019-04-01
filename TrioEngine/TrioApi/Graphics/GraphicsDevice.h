#pragma once

#include "TrioApiDecl.h"

#include "ClearOptions.h"
#include "Event.h"
#include "GraphicsCapabilities.h"
#include "PresentationParameters.h"
#include "PrimitiveType.h"
#include "ShaderStage.h"
#include "VertexBufferBinding.h"

#include "..\Math\MathVector.h"

namespace Vago
{
	class ConstantBuffer;
	class ConstantBufferCollection;
	struct BlendState;
	class DepthStencilBuffer;
	struct DepthStencilState;
	class GraphicsAdapter;
	class IndexBuffer;
	class TRIOAPI_DECL RasterizerState;
	struct SamplerState;
	class SamplerStateCollection;
	class Shader;
	class TextureCollection;
	class VertexBuffer;
	
	class TRIOAPI_DECL GraphicsDevice
	{
	public:
		GraphicsDevice(GraphicsAdapter* adapter, PresentationParameters parameters);
		~GraphicsDevice();

		void Clear(Color &color);
		void Clear(ClearOptions options, Color &color, float depth, uint8_t stencil);

		void DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount);
		void DrawPrimitives(PrimitiveType primitiveType, int vertexStart, int primitiveCount);

		GraphicsAdapter*           GetAdapter() const { return m_pAdapter; }

#ifdef TRIO_DIRECTX
		UINT                    GetBackBufferCount() const { return m_uiBackBufferCount; }
		ID3D11RenderTargetView*	GetBackBufferRenderTargetView() const { return m_d3dRenderTargetView.Get(); }
		ID3D11Device*           GetD3DDevice() const { return m_d3dDevice.Get(); }
		ID3D11Device1*          GetD3DDevice1() const { return m_d3dDevice1.Get(); }
		ID3D11DeviceContext*    GetD3DDeviceContext() const { return m_d3dContext.Get(); }
		ID3D11DeviceContext1*   GetD3DDeviceContext1() const { return m_d3dContext1.Get(); }
		D3D_FEATURE_LEVEL       GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
		IDXGISwapChain*         GetSwapChain() const { return m_swapChain.Get(); }
		IDXGISwapChain1*        GetSwapChain1() const { return m_swapChain1.Get(); }
		//ID3D11DepthStencilView* GetDepthStencilView() const					{ return m_depthStencilBuffer->m_d3dDepthStencilView.Get(); }

		D3D11_VIEWPORT          GetScreenViewport() const { return m_stScreenViewport; }
#endif

		inline GraphicsCapabilities GetGraphicsCapabilities() { return m_graphicsCapabilities; }
		inline PresentationParameters& GetPresentationParameters() { return m_presentationParameters; }
		inline TextureCollection* GetTextures() { return m_pTextureCollection; }
		inline TextureCollection* GetVertexTextures() { return m_pTextureCollection; }
		
		void Present();
		void Reset(PresentationParameters presentationParameters);
		void Reset(PresentationParameters presentationParameters, GraphicsAdapter* graphicsAdapter);

		void SetBlendState(BlendState* state);
		void SetDepthStencilState(DepthStencilState* state);
		void SetIndexBuffer(IndexBuffer *indexBuffer);
		void SetRasterizerState(RasterizerState* state);
		void SetSamplerState(int index, SamplerState* state);
		void SetVertexBuffer(VertexBuffer *buffer);
		void SetVertexBuffer(VertexBufferBindings &bindings);
		void SetVertexShader(Shader* shader);
		void SetPixelShader(Shader* shader);

		Event<> DeviceLost;
		Event<> DeviceRestored;

		friend class EffectPass;
	private:
		GraphicsCapabilities						m_graphicsCapabilities;
		TextureCollection*							m_pVertexTextureCollection;

		TextureCollection*							m_pTextureCollection;
		SamplerStateCollection*						m_pSamplerCollection;
		PresentationParameters						m_presentationParameters;
		GraphicsAdapter*							m_pAdapter;

		std::unique_ptr<DepthStencilBuffer> m_depthStencilBuffer;
		DepthStencilBuffer* m_pCurrentDepthStencilBuffer;

		IndexBuffer* m_pIndexBuffer;
		bool m_bIndexBufferDirty;

		VertexBuffer* m_pVertexBuffer;
		bool m_bVertexBufferDirty;
		VertexBufferBindings m_vVertexBindings;

		Shader* m_pVertexShader;
		bool m_bVertexShaderDirty;

		Shader* m_pPixelShader;
		bool m_bPixelShaderDirty;

		DepthStencilState* m_pDepthStencilState;
		bool m_bDepthStencilStateDirty;

		BlendState* m_pBlendState;
		bool m_bBlendStateDirty;

		RasterizerState* m_pRasterizerState;
		bool m_bRasterizerStateDirty;
		
		ConstantBufferCollection* m_pVertexConstantBuffers;
		ConstantBufferCollection* m_pPixelConstantBuffers;

		UINT                                            m_uiBackBufferCount;
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
		D3D11_VIEWPORT                                  m_stScreenViewport;

		std::vector<ID3D11RenderTargetView *>			m_vCurrentRenderTargets;

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
		void HandleDeviceLost();
		void SetConstantBuffer(ShaderStage stage, int slot, ConstantBuffer* buffer);
	};

}
