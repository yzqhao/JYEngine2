#pragma once

#include "PipelineContex.h"
#include "Math/3DMath.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class RenderTargetEntity;
class Scene;
class RenderContext;
class ShaderKeyWords;

class ENGINE_API RenderUntil
{
public:

	/**
		* Sort the cmmand data for shadow rendering.
		* @param data The command data to be drawn.
		*/
	static void SortShadowPassData(RenderCommandData& data);

	/**
	* Collect the cmmand data for shadow rendering.
	* @contex the pipe contex
	* @param data The command data to be drawn.
	*/
	static void CollectShadowPassData(PipelineContex& context, RenderCommandData& data);

	/**
	* Compute the key words for light and object.
	* @lightCom the light for drawing
	* @renderCom the render component for drawing
	* @keyWords the keyWords for drawing
	*/
	static void ComputeKeyWords(const LightComponent* lightCom, const RenderComponent* renderCom, ShaderKeyWords* keyWords);

	/**
	* Compute the key words for hardware.
	* @keyWords the keyWords for shader
	*/
	static void ComputeHardwareKeyWords(ShaderKeyWords* pKeyWords);

	/**
	* Draw objects using the forward rendering method(per object per light).
	* @context pipe contex
	* @pRenderTarget the render target for drawing
	* @param data The command data to be drawn.
	* @flag the clear flag
	* @color the clear color
	* @commandData commad data array for data
	* @count the count of array
	*/
	static int DrawObjects2RT(PipelineContex& context, RenderTargetEntity* pRenderTarget,
		RHIDefine::ClearFlag flag, const Math::FLinearColor& color, RenderCommandData** commandData, int count = 1);

	/**
	* Copy source render target to dest target by material
	* @context pipe contex
	* @srcRenderTarget the source render target
	* @dstRenderTarget the dest render target
	* @pCopymat copy material
	* @flag clear flag
	* @color clear color
	*/
	static void CopyRenderTarget(PipelineContex& pipeContext, RenderTargetEntity* srcRenderTarget,
		RenderTargetEntity* dstRenderTarget, MaterialEntity* pCopymat,
		RHIDefine::ClearFlag flag, const Math::FColor& color);

	/**
	* Create render target
	* @resolution resolution
	* @targetType hash type
	* @format the format of rendertagert
	* @hasDepthStencil whichi has depth stencil
	* @return the render target which is be created
	*/
	static RenderTargetEntity* CreateRenderTarget(const Math::IntVec2& resolution, 
		RHIDefine::PixelFormat format, bool hasDepthStencil);


	/**
	* Copy the rendertarget form srcRenderTarget to dstRenderTarget
	* @pipeContext the pipe contex
	* @srcRenderTarget the source rendertarget
	* @dstRenderTarget the dest rendertarget
	*/
	static void CopyRenderTarget(PipelineContex& pipeContext, RenderTargetEntity* srcRenderTarget, RenderTargetEntity* dstRenderTarget);

	/**
	* Draw Post effects
	* @pipeContext the pipe contex
	* @srcRenderTarget the src render target
	* @dstRenderTarget the dst render target
	*/
	static int DrawPostEffects(PipelineContex& pipeContext, RenderTargetEntity* srcRenderTarget, RenderTargetEntity* dstRenderTarget);

	/**
	* Collects all the RenderPassData that can be rendered in this pipeline.
	* @param ForceForwardRendering Mark all objects as forward rendering instead of basing on the material.
	* @backCommand back command data
	* @translucentData the translucent command data
	* @overlayerData the overlayer command data which is for ui drawing after posteffect
	* @forwardOpaqueData the forward opaque command data
	* @deferredOpaqueData the deferred opaque command data
	*/
	static void CollectRenderingData(PipelineContex& contex, bool ForceForwardRendering,
		RenderCommandData& backCommand,
		RenderCommandData& translucentData,
		RenderCommandData& overlayerData,
		RenderCommandData& forwardOpaqueData,
		RenderCommandData& deferredOpaqueData);

	/**
	* Sort lighting pass opaque data by material, distance, etc.
	*/
	static void SortLightingPassDataNonOpaque(RenderCommandData& commandData);

	//Sort lighting pass non opaque data by material, distance, etc.
	static void SortLightingPassDataOpaque(RenderCommandData& commandData);

	/**
	* Set Camera Param
	* @context Pipeline contex
	*/
	static void SetCameraParam(PipelineContex& context);

	/**
	* Set Node Param
	* @context Pipeline contex
	* @renderComponent the render component
	* @submeshIdx the submesh index
	*/
	static void SetNodeParam(PipelineContex& contex, RenderComponent* renderComponent, uint16 submeshIdx);

	/**
	* Set Ambient Light Param
	* @context Pipeline contex
	*/
	static void SetAmbientLightParam(PipelineContex& pipeContext);

	/**
	* Set System Shader Param
	* @context Pipeline contex
	*/
	static void SetSystemParam(PipelineContex& context);

	/**
	* Set Light Param
	* @context Pipeline contex
	* @light the light for drawing
	*/
	static void SetLightParam(PipelineContex& context, const LightComponent* light);

	/**
	* Compute Deferred Shader Keywords
	* @lightCom the light for drawing
	* @keyWords the out keywords
	*/
	static void ComputeDeferredKeyWords(const LightComponent* lightCom, ShaderKeyWords* keyWords);

	/**
	* Create Render Pass
	* @commandData the command data for drawing
	* @supportedPass the support Passes
	* @passLen the len of the array
	*/
	static void GenRenderPasses(RenderCommandData& commandData, int* supportedPass, int passLen);

	/**
	* Draw Render Pass by type
	* @context Pipeline contex
	* @ro the render object
	* @mat the material
	* @passType the pass type for drawing
	*/
	static void DrawByType(PipelineContex& context, RenderObjectEntity* ro, MaterialEntity* mat, int passType);
};

NS_JYE_END