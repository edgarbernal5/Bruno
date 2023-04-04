#pragma once

#include "TrioApiRequisites.h"

#include "ClearOptions.h"
#include "Event.h"
#include "GraphicsCapabilities.h"
#include "PresentationParameters.h"
#include "PrimitiveType.h"
#include "ShaderStage.h"
#include "VertexBufferBinding.h"
#include "RenderTargetBinding.h"
#include "IndexElementSize.h"

#include "DeviceErrorStatus.h"
#include "..\Math\MathVector.h"

#include <map>

namespace BrunoEngine
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
	class RenderTarget2D;
	class VertexDeclaration;
	class DynamicVertexBuffer;
	class DynamicIndexBuffer;
	
	class BRUNO_API_EXPORT GraphicsDevice
	{
	public:
		GraphicsDevice(GraphicsAdapter* adapter, PresentationParameters parameters);
		~GraphicsDevice();

		void Clear(ColorRGBA8 &color);
		void Clear(const float* color);
		void Clear(ClearOptions options, const float* color, float depth, uint8_t stencil);

		void DrawIndexedPrimitives(PrimitiveType primitiveType, uint32_t baseVertex, uint32_t startIndex, uint32_t primitiveCount);
		
		void DrawUserPrimitives(PrimitiveType primitiveType, const uint8_t* vertexData, int vertexSizeArray, int vertexOffset, int primitiveCount, VertexDeclaration* declaration);
		void DrawUserIndexedPrimitives(PrimitiveType primitiveType, const uint8_t* vertexData, int vertexSizeArray, int vertexOffset, int numVertices, uint16_t* indexData, int indexLength, IndexElementSize indexSize, int indexOffset, int primitiveCount, VertexDeclaration* declaration);

		GraphicsAdapter*           GetAdapter() const { return m_adapter; }

#ifdef BRUNO_DIRECTX
		uint32_t                GetBackBufferCount() const { return m_backBufferCount; }
		ID3D11RenderTargetView*	GetBackBufferRenderTargetView() const
		{
			return m_d3dDefaultRenderTargetView; 
		}

		inline ID3D11Device1*          GetD3DDevice() const noexcept { return m_d3dDevice.Get(); }
		inline ID3D11DeviceContext1*   GetD3DDeviceContext() const noexcept { return m_d3dContext.Get(); }
		auto                    GetDXGIFactory() const noexcept { return m_dxgiFactory.Get(); }
		D3D_FEATURE_LEVEL       GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
		//ID3D11DepthStencilView* GetDepthStencilView() const					{ return m_depthStencilBuffer->m_d3dDepthStencilView.Get(); }
#endif
		inline BlendState* GetBlendState() { return m_blendState; }
		inline DepthStencilState* GetDepthStencilState() { return m_depthStencilState; }
		inline RasterizerState* GetRasterizerState() { return m_rasterizerState; }

		inline GraphicsCapabilities GetGraphicsCapabilities() { return m_graphicsCapabilities; }
		inline PresentationParameters& GetPresentationParameters()
		{
			return m_currentPresentationParameters;
		}

		inline TextureCollection* GetTextures() { return m_textureCollection; }
		inline TextureCollection* GetVertexTextures() { return m_textureCollection; }
		inline Viewport GetViewport() { return m_viewport; }
		
		void PrepareRenderWindow(HWND hwnd);

		void Flush();
		void Present();
		void Present(HWND hostHwnd);
		void RemoveHwnd(HWND hostHwnd);
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

		void SetRenderTarget(RenderTarget2D* renderTarget);

		Event<> DeviceLost;
		Event<> DeviceRestored;

		friend class EffectPass;
	private:
		struct WindowResources {
			SwapChain* m_swapChain = nullptr;
			DepthStencilBuffer* m_depthStencilBuffer = nullptr;
			PresentationParameters m_presentationParameters;
#ifdef BRUNO_DIRECTX
			ID3D11Texture2D* m_defaultD3dRenderTarget = nullptr;
			ID3D11RenderTargetView* m_d3dDefaultRenderTargetView = nullptr;
#endif
		};

		void ReleaseWindowResources(WindowResources& windowResources);

		PresentationParameters m_emptyPP;
		std::unordered_map<HWND, WindowResources>	m_windowResources;
		//std::unique_ptr<SwapChain>					m_swapChain;
		GraphicsCapabilities								m_graphicsCapabilities;
		TextureCollection*									m_vertexTextureCollection;

		TextureCollection*									m_textureCollection;
		SamplerStateCollection*								m_samplerCollection;
		PresentationParameters								m_currentPresentationParameters;
		GraphicsAdapter*									m_adapter;
		std::map<uint64_t, DynamicVertexBuffer*>			m_userVertexBuffers;
		std::map<int, DynamicIndexBuffer*>					m_userIndexBuffers;

		SwapChain* m_currentSwapChain;
		DepthStencilBuffer* m_currentDepthStencilBuffer;

		IndexBuffer* m_indexBuffer;
		bool m_indexBufferDirty;

		VertexBuffer* m_vertexBuffer;
		bool m_vertexBufferDirty;
		VertexBufferBindings m_vertexBindings;

		RenderTargetBindings m_renderTargetBindings;

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
#ifdef BRUNO_DIRECTX
		Microsoft::WRL::ComPtr<IDXGIFactory2>               m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D11Device1>               m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1>        m_d3dContext;
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;

		//Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_defaultD3dRenderTarget;
		//Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_d3dDefaultRenderTargetView;
		ID3D11Texture2D*         m_defaultD3dRenderTarget;
		ID3D11RenderTargetView*  m_d3dDefaultRenderTargetView;

		ID3D11DepthStencilView*							m_defaultD3dDepthStencilView;
		std::vector<ID3D11RenderTargetView*>			m_currentD3dRenderTargets;
		ID3D11DepthStencilView*							m_currentD3dDepthStencilView;

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
#if BRUNO_DIRECTX
		void CreateFactory();
#endif
		void CreateWindowSizeDependentResources(PresentationParameters& parameters);
		void HandleDeviceLost();
		void SetConstantBuffer(ShaderStage stage, int slot, ConstantBuffer* buffer);

		void SetRenderTargets(RenderTargetBindings& bindings);

		int SetUserIndexBuffer(int baseSizeInBytes, const uint8_t* indexData, int length, int indexOffset, int indexCount);
		int SetUserVertexBuffer(const uint8_t* vertexData, int length, int vertexOffset, int vertexCount, VertexDeclaration* vertexDecl);
	};

}
