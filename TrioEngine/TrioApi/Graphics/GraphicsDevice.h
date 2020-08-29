#pragma once

#include "TrioApiRequisites.h"

#include "ClearOptions.h"
#include "Event.h"
#include "GraphicsCapabilities.h"
#include "PresentationParameters.h"
#include "PrimitiveType.h"
#include "ShaderStage.h"
#include "VertexBufferBinding.h"

#include "DeviceErrorStatus.h"
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
	class SwapChain;
	
	class TRIO_API_EXPORT GraphicsDevice
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
		ID3D11Device1*          GetD3DDevice() const noexcept { return m_d3dDevice.Get(); }
		ID3D11DeviceContext1*   GetD3DDeviceContext() const noexcept { return m_d3dContext.Get(); }
		auto                    GetDXGIFactory() const noexcept { return m_dxgiFactory.Get(); }
		D3D_FEATURE_LEVEL       GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
		//ID3D11DepthStencilView* GetDepthStencilView() const					{ return m_depthStencilBuffer->m_d3dDepthStencilView.Get(); }
#endif
		inline BlendState* GetBlendState() { return m_blendState; }
		inline DepthStencilState* GetDepthStencilState() { return m_depthStencilState; }
		inline RasterizerState* GetRasterizerState() { return m_rasterizerState; }

		inline GraphicsCapabilities GetGraphicsCapabilities() { return m_graphicsCapabilities; }
		inline PresentationParameters& GetPresentationParameters() { return m_presentationParameters; }
		inline TextureCollection* GetTextures() { return m_textureCollection; }
		inline TextureCollection* GetVertexTextures() { return m_textureCollection; }
		inline Viewport GetViewport() { return m_viewport; }
		
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
		void SetViewport(Viewport viewport);
		void SetPixelShader(Shader* shader);

		Event<> DeviceLost;
		Event<> DeviceRestored;

		friend class EffectPass;
	private:
		std::unique_ptr<SwapChain>					m_swapChain;
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

		Viewport				m_viewport;

		uint32_t                                            m_backBufferCount;
#ifdef TRIO_DIRECTX
		Microsoft::WRL::ComPtr<IDXGIFactory2>               m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D11Device1>               m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1>        m_d3dContext;
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_renderTarget;

		// Direct3D rendering objects. Required for 3D.
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_d3dRenderTargetView;
		
		std::vector<ID3D11RenderTargetView*>			m_currentRenderTargets;

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
#if TRIO_DIRECTX
		void CreateFactory();
#endif
		void CreateWindowSizeDependentResources();
		void HandleDeviceLost();
		void SetConstantBuffer(ShaderStage stage, int slot, ConstantBuffer* buffer);
	};

}
