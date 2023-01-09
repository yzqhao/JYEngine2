#include "AdapterRender.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "Engine/resource/GeneralMetadata.h"
#include "Engine/resource/VertexMetadata.h"
#include "Engine/resource/IndicesMetadata.h"
#include "Engine/resource/TextureMetadata.h"

NS_JYE_BEGIN

namespace ScissorParams
{
	static const char* COORDINATE_X("_SCISSORX");
	static const char* COORDINATE_Y("_SCISSORY");
	static const char* SCISSOR_WIDTH("_SCISSORZ");
	static const char* SCISSOR_HEIGHT("_SCISSORW");
}

#define CAMERA_SEQUENCE_BEGIN 100000
#define BIND_MIN_BEGIN 10000
#define BIND_MIN_END (BIND_MIN_BEGIN+1)

uint AdapterRender::s_CameraSequence = CAMERA_SEQUENCE_BEGIN;

void ImGui_RenderDrawData(AdapterRender* render, ImDrawData* draw_data)
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0)
		return;

	// Will project scissor/clipping rectangles into framebuffer space
	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	CameraComponent* camera = render->GetCameraComponent();
	camera->CreateUICameraProjection(
		draw_data->DisplayPos.x,
		draw_data->DisplayPos.y,
		draw_data->DisplaySize.x,
		draw_data->DisplaySize.y);

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		size_t idx_buffer_offset = 0;
		// Upload vertex/index buffers
		RenderComponent* rc = render->GetRenderComponent(cmd_list->CmdBuffer.Size);
		VertexStream* vs = rc->GetVertexStream();
		vs->CopyVertexBuffer(cmd_list->VtxBuffer.Size, sizeof(ImDrawVert), (byte*)cmd_list->VtxBuffer.Data);
		rc->FlushVertexBuffer(*vs);
		IndicesStream* is = rc->GetIndexStream();
		is->CopyIndicesBuffer(cmd_list->IdxBuffer.Size, sizeof(ImDrawIdx), (byte*)cmd_list->IdxBuffer.Data);
		rc->FlushIndiesBuffer(*is);
		rc->DisableSubMeshs();

		uint cmdindex = 0;
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			// Project scissor/clipping rectangles into framebuffer space
			ImVec4 clip_rect;
			clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
			clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
			clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
			clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

			if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
			{
				SubMesh* dc = rc->CreateSubMesh();

				dc->SetDrawRange(idx_buffer_offset, pcmd->ElemCount);

				float xStart = clip_rect.x;
				float xLength = clip_rect.z - clip_rect.x;
				float yStart = clip_rect.y;
				float yLength = clip_rect.w - clip_rect.y;
				Math::Vec4 range(xStart, yStart, xLength, yLength);

				rc->SetParameter(cmd_i, ScissorParams::COORDINATE_X, MakeMaterialParam(xStart));
				rc->SetParameter(cmd_i, ScissorParams::COORDINATE_Y, MakeMaterialParam(yStart));
				rc->SetParameter(cmd_i, ScissorParams::SCISSOR_WIDTH, MakeMaterialParam(xLength));
				rc->SetParameter(cmd_i, ScissorParams::SCISSOR_HEIGHT, MakeMaterialParam(yLength));
				rc->SetParameter(cmd_i, "TEXTURE_DIFFUSE", MakeMaterialParam((TextureEntity*)pcmd->TextureId));
				idx_buffer_offset += pcmd->ElemCount;
			}
		}
	}
}


AdapterRender::AdapterRender(IWindow* win)
	: m_rpCameraComponent(NULL)
	, m_RootNode(NULL)
	, m_RenderIndex(0)
{
	Scene* scene = SceneManager::Instance()->GetScene("EditorUI");
	scene->SetSequence(CAMERA_SEQUENCE_BEGIN);
	m_RootNode = scene->CreateObject("node");
	m_RootNode->SetLayer(MaskConstant::MC_MASK_EDITOR_UI_LAYER);  //±à¼­Æ÷UI Layer
	m_rpCameraComponent = m_RootNode->CreateComponent<CameraComponent>();
	m_rpCameraComponent->SetPipelineType(GraphicDefine::PP_UI);
	m_rpCameraComponent->EraseLayerMask(MaskConstant::MC_MASK_DEFAULT_LAYER);
	m_rpCameraComponent->AddLayerMask(MaskConstant::MC_MASK_EDITOR_UI_LAYER);
	m_rpCameraComponent->SetRenderWindow(win);
	m_rpCameraComponent->SetSequenceCulling(true);
	m_rpCameraComponent->SetSequence(s_CameraSequence++);
	m_rpCameraComponent->SetActive(true);

	m_pUIMat = _NEW MaterialEntity;
	m_pUIMat->PushMetadata(PathMetadata("comm:/material/editor/editorui"));
	m_pUIMat->CreateResource();
}

AdapterRender::~AdapterRender()
{
	Scene* scene = SceneManager::Instance()->GetScene("EditorUI");
	for (auto render : m_RenderNodeArray)
	{
		scene->DeleteGObject(render.first);
	}
	scene->DeleteGObject(m_RootNode);
	SAFE_DELETE(m_pUIMat);
}

void AdapterRender::_BeginRender()
{
	m_RenderIndex = 0;
	for (auto render : m_RenderNodeArray)
	{
		render.second->EraseRenderProperty(GraphicDefine::RP_SHOW);
	}
}

RenderComponent* AdapterRender::GetRenderComponent(int subCount)
{
	RenderComponent* rc;
	if (m_RenderIndex >= m_RenderNodeArray.size())
	{
		m_RenderNodeArray.resize(m_RenderIndex + 1);
		Scene* scene = SceneManager::Instance()->GetScene("EditorUI");
		GObject* node = scene->CreateObject("node");
		node->SetLayer(MaskConstant::MC_MASK_EDITOR_UI_LAYER);
		rc = node->CreateComponent<RenderComponent>();
		rc->SetSequence(m_rpCameraComponent->GetSequence());
		VertexStream vs;
		IndicesStream is;
		is.SetIndicesType(sizeof(ImDrawIdx) == 2 ? RHIDefine::IT_UINT16 : RHIDefine::IT_UINT32);
		vs.SetVertexType<Math::Vec2>(RHIDefine::PS_ATTRIBUTE_POSITION, RHIDefine::DT_FLOAT, RHIDefine::DT_FLOAT);
		vs.SetVertexType<Math::Vec2>(RHIDefine::PS_ATTRIBUTE_COORDNATE0, RHIDefine::DT_FLOAT, RHIDefine::DT_FLOAT);
		vs.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_COLOR0, RHIDefine::DT_FLOAT, RHIDefine::DT_UINT_8_8_8_8);

		rc->PushMetadata(RenderObjectMeshMetadate(RHIDefine::RenderMode::RM_TRIANGLES,
			ReferenceVertexMetadata(RHIDefine::MU_DYNAMIC, &vs),
			ReferenceIndicesMetadata(RHIDefine::MU_DYNAMIC, &is)));

		std::vector<MaterialEntity*> mats;
		mats.resize(subCount, m_pUIMat);

		for (int i = 0; i < subCount; i++)
		{
			mats[i] = m_pUIMat->Clone();
			rc->AddMaterialEntity(mats[i]);
		}

		for (int i = 0; i < subCount; i++)
		{
			SAFE_DELETE(mats[i]);
		}

		rc->CreateResource();
		rc->EraseRenderProperty(GraphicDefine::RP_CULL);
		m_RenderNodeArray[m_RenderIndex] = { node, rc };
	}
	else
	{
		rc = m_RenderNodeArray[m_RenderIndex].second;

		size_t nMats = rc->GetMaterialCount();
		if (nMats < subCount)
		{
			for (size_t i = 0; i < subCount - nMats; ++i)
			{
				MaterialEntity* tmpMat = m_pUIMat->Clone();
				rc->ChangeMaterialEntity(nMats + i, tmpMat);
				SAFE_DELETE(tmpMat);
			}
		}
		else if (nMats > subCount)
		{
			rc->ResizeMaterials(subCount);
		}

	}
	rc->SetRenderProperty(GraphicDefine::RP_SHOW);
	++m_RenderIndex;
	return rc;
}

void AdapterRender::Render(ImDrawData* draw_data)
{
	_BeginRender();
	ImGui_RenderDrawData(this, draw_data);
}

void AdapterRender::SetupRender()
{
	//setup font
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

	TextureStream ts;
	ts.SetStreamType(Math::IntVec2(width, height), RHIDefine::PF_R8G8B8A8);
	ts.SetBuffer(pixels);

	TextureEntity* font = _NEW TextureEntity();
	font->PushMetadata(
		TextureBufferMetadata(
			ts.GetSize(),
			RHIDefine::TextureType::TEXTURE_2D,
			RHIDefine::TextureUseage::TU_STATIC,
			ts.GetPixelFormat(),
			0, false, 0,
			RHIDefine::TextureWarp::TW_CLAMP_TO_EDGE,
			RHIDefine::TextureWarp::TW_CLAMP_TO_EDGE,
			RHIDefine::TextureFilter::TF_LINEAR,
			RHIDefine::TextureFilter::TF_LINEAR_MIPMAP_LINEAR,
			&ts));
	font->CreateResource();
	io.Fonts->TexID = font;
}

void AdapterRender::OnWindowDestroy()
{
	m_rpCameraComponent->SetRenderWindow(NULL);
}

NS_JYE_END