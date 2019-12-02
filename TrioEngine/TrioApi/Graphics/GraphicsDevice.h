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

namespace TrioEngine
{
	class ConstantBuffer;
	class ConstantBufferCollection;
	struct BlendState;
	class DepthStencilBuffer;
	struct DepthStencilState;
	class GraphicsAdapter;
	class IndexBuffer;
	class RasterizerState;
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

		GraphicsAdapter*           GetAdapter() const { return m_adapter; }

#ifdef TRIO_DIRECTX
		UINT                    GetBackBufferCount() const { return m_backBufferCount; }
		ID3D11RenderTargetView*	GetBackBufferRenderTargetView() const { return m_d3dRenderTargetView.Get(); }
		ID3D11Device*           GetD3DDevice() const { return m_d3dDevice.Get(); }
		ID3D11Device1*          GetD3DDevice1() const { return m_d3dDevice1.Get(); }
		ID3D11DeviceContext*    GetD3DDeviceContext() const { return m_d3dContext.Get(); }
		ID3D11DeviceContext1*   GetD3DDeviceContext1() const { return m_d3dContext1.Get(); }
		D3D_FEATURE_LEVEL       GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
		IDXGISwapChain*         GetSwapChain() const { return m_swapChain.Get(); }
		IDXGISwapChain1*        GetSwapChain1() const { return m_swapChain1.Get(); }
		//ID3D11DepthStencilView* GetDepthStencilView() const					{ return m_depthStencilBuffer->m_d3dDepthStencilView.Get(); }

		D3D11_VIEWPORT          GetScreenViewport() const { return m_screenViewport; }
#endif

		inline GraphicsCapabilities GetGraphicsCapabilities() { return m_graphicsCapabilities; }
		inline PresentationParameters& GetPresentationParameters() { return m_presentationParameters; }
		inline TextureCollection* GetTextures() { return m_textureCollection; }
		inline TextureCollection* GetVertexTextures() { return m_textureCollection; }
		
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
		TextureCollection*							m_vertexTextureCollection;

		TextureCollection*							m_textureCollection;
		SamplerStateCollection*						m_samplerCollection;
		PresentationParameters						m_presentationParameters;
		GraphicsAdapter*							m_adapter;

		std::unique_ptr<DepthStencilBuffer> m_depthStencilBuffer;
		DepthStencilBuffer* m_currentDepthStencilBuffer;

		IndexBuffer* m_indexBuffer;
		bool m_indexBufferDirty;

		VertexBuffer* m_vertexBuffer;
		bool m_vertexBufferDirty;
		VertexBufferBindings m_vertexBindings;

		Shader* m_vertexShader;
		bool m_vertexShaderDirty;

		Shader* m_pixelShader;
		bool m_pixelShaderDirty;

		DepthStencilState* m_depthStencilState;
		bool m_depthStencilStateDirty;

		BlendState* m_blendState;
		bool m_blendStateDirty;

		RasterizerState* m_rasterizerState;
		bool m_rasterizerStateDirty;
		
		ConstantBufferCollection* m_vertexConstantBuffers;
		ConstantBufferCollection* m_pixelConstantBuffers;

		uint32_t                                            m_backBufferCount;
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
		ID3D11Buffer*	m_vertexBuffers[MaxVertexBuffers];
		uint32_t		m_vertexOffsets[MaxVertexBuffers];
		uint32_t		m_vertexStrides[MaxVertexBuffers];

#endif
		void ApplyState(bool applyShaders);
		bool AreSameVertexBindings(VertexBufferBindings &bindings);
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		void HandleDeviceLost();
		void SetConstantBuffer(ShaderStage stage, int slot, ConstantBuffer* buffer);
	};

}
