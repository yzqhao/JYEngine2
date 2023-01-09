
#include "MSceneExt.h"
#include "math/3DMath.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/resource/GeneralMetadata.h"
#include "Engine/resource/VertexMetadata.h"
#include "Engine/resource/IndicesMetadata.h"
#include "Engine/render/material/IMaterialSystem.h"
#include "Engine/ProjectSetting.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/resource/RenderTargetMetadata.h"
#include "Engine/render/texture/TextureEntity.h"
#include "Engine/resource/TextureMetadata.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Engine/component/transform/TransformComponent.h"

NS_JYE_BEGIN

void MSceneExt::SceneInit(Scene* sce)
{
	sce->SetSequence(HIGH);
	sce->CreateDefaultRenderTarget(Math::IntVec2(128, 128));
}

GObject* MSceneExt::SceneCreateGrid(Scene* sce)
{
	float cellLength = 1.0;
	int cellCnt = 100;
	String material = "comm:material/editor/unlight_color";
	float sidelength = cellLength;
	int size = cellCnt;

	GObject* gridNode = MSceneExt::SceneCreateGenericNode(sce, "Grid");
	gridNode->SetLayer(MC_MASK_EDITOR_SCENE_LAYER);

	RenderComponent* renderComponent = gridNode->CreateComponent<RenderComponent>();
	renderComponent->EraseRenderProperty(GraphicDefine::RP_CULL);
	renderComponent->SetRenderProperty(GraphicDefine::RP_IGNORE_PICK);
	//renderComponent->SetBindBox(mathfunction.Aabbox3d(mathfunction.vector3(0, 0, 0), mathfunction.vector3(0, 0, 0))); --让grid无法被拾取

	VertexStream* vertexStream = _NEW VertexStream();
	IndicesStream* indexStream = _NEW IndicesStream();
	vertexStream->SetVertexType(RHIDefine::PS_ATTRIBUTE_POSITION,
		RHIDefine::DT_FLOAT,
		RHIDefine::DT_HALF_FLOAT,
	4);
	indexStream->SetIndicesType(RHIDefine::IT_UINT16);

	vertexStream->ReserveBuffer(4 * size); //预先分配顶点的内存  4 * n
	indexStream->ReserveBuffer(4 * size + 4); //4 * n + 4
	int val = size / 2;

	for (int i = -val; i <= val; ++i)
	{
		vertexStream->PushVertexData(
			RHIDefine::PS_ATTRIBUTE_POSITION,
			Math::Vec4(i * sidelength, 0, -val * sidelength, 1).GetPtr());
		vertexStream->PushVertexData(
			RHIDefine::PS_ATTRIBUTE_POSITION,
			Math::Vec4(i * sidelength, 0, val * sidelength, 1).GetPtr());
	}

	for (int i = 1 - val; i <= val - 1; ++i)
	{
		vertexStream->PushVertexData(
			RHIDefine::PS_ATTRIBUTE_POSITION,
			Math::Vec4(-val * sidelength, 0, i * sidelength, 1).GetPtr());
		vertexStream->PushVertexData(
			RHIDefine::PS_ATTRIBUTE_POSITION,
			Math::Vec4(val * sidelength, 0, i * sidelength, 1).GetPtr());
	}

	for (int i = 0; i < 4 * size; ++i)
	{
		indexStream->PushIndicesData(i);
	}

	indexStream->PushIndicesData(0);
	indexStream->PushIndicesData(2 * size);
	indexStream->PushIndicesData(1);
	indexStream->PushIndicesData(2 * size + 1);

	MaterialEntity* mat = _NEW MaterialEntity();
	mat->PushMetadata(PathMetadata(material));
	mat->CreateResource();
	renderComponent->AddMaterialEntity(mat);

	ReferenceVertexMetadata vmeta = ReferenceVertexMetadata(RHIDefine::MU_STATIC, vertexStream);
	ReferenceIndicesMetadata imeta = ReferenceIndicesMetadata(RHIDefine::MU_STATIC, indexStream);
	renderComponent->PushMetadata(RenderObjectMeshMetadate(
		RHIDefine::RM_LINES,
		vmeta,
		imeta));
	renderComponent->CreateResource();

	auto gridColor = IMaterialSystem::Instance()->NewParameterSlot(RHIDefine::SU_UNIFORM, "POINT_COLOR");

	if (ProjectSetting::Instance()->IsCurrentAdvancedShading())
		renderComponent->SetParameter(gridColor, MakeMaterialParam(Math::Vec4(0.214, 0.214, 0.214, 1.0)));
	else
		renderComponent->SetParameter(gridColor, MakeMaterialParam(Math::Vec4(0.5, 0.5, 0.5, 1.0)));

	return gridNode;
}

GObject* MSceneExt::SceneCreateEditorCamera(Scene* sce)
{
	Math::IntVec2 size(128, 128);
	float fnear = 0.1;
	float ffar = 100;
	Math::Vec3 pos(0, 1, 4);
	Math::Vec3 lookat(0, 0, 0);
	Math::Vec3 up(0, 1, 0);
	String EditorCameraName = "EditorCamera";

	RenderTargetEntity* rendertarget = _NEW RenderTargetEntity(); //创建一个FBO
	rendertarget->PushMetadata(	// 设置FBO格式
		RenderTargetMetadata(
			RHIDefine::RT_RENDER_TARGET_2D,
			Math::IntVec4(0, 0, size.x, size.y), // 视口大小
			size)); // 分辨率
	TextureEntity* depth = rendertarget->MakeTextureAttachment(RHIDefine::TA_DEPTH_STENCIL); // 增加深度纹理
		depth->PushMetadata(
			DepthRenderBufferMetadata(
				size,
				RHIDefine::PF_DEPTH24_STENCIL8
			)); // 增加深度
	TextureEntity* outputtexture = rendertarget->MakeTextureAttachment(RHIDefine::TA_COLOR_0); // 增加color0纹理
		outputtexture->PushMetadata(	// 创建纹理
			TextureBufferMetadata(size));
	rendertarget->CreateResource();

	GObject* luacamera3D = SceneCreateGenericNode(sce, "EditorCamera");
	CameraComponent* cameraComponent = luacamera3D->CreateComponent<CameraComponent>();
	GObject* camera3D = luacamera3D;
	camera3D->SetLayer(MC_MASK_EDITOR_SCENE_LAYER);

	cameraComponent->ChangeResolution(size);
	cameraComponent->SetLayerMaskEverything(); // 导演的视角 有什么画什么
	cameraComponent->CreatePerspectiveProjection(fnear, ffar);
	cameraComponent->LookAt(pos, lookat, up);
	cameraComponent->AttachRenderTarget(rendertarget);
	cameraComponent->Recalculate();
	cameraComponent->SetActive(true);

	if (ProjectSetting::Instance()->IsCurrentAdvancedShading())
		cameraComponent->SetClearColor(Math::FLinearColor(0.032, 0.032, 0.032, 1.0));
	else
		cameraComponent->SetClearColor(Math::FLinearColor(0.2, 0.2, 0.2, 1.0));
	
	return luacamera3D;
}

GObject* MSceneExt::SceneCreateGenericNode(Scene* sce, const String& name)
{
	GObject* node = sce->CreateObject(name);
	TransformComponent* trans = node->CreateComponent<TransformComponent>();
	trans->SetLocalPosition(Math::Vec3(0.0, 0.0, 0.0));
	return node;
}

String MSceneExt::GenerateNonRepeatName(GObject* node)
{
	String outName = node->GetName();
	int idx = 1;
	while (!CheckLegalName(node, outName))
	{
		outName = node->GetName() + "(" + std::to_string(idx) + ")";
	}
	return outName;
}

bool MSceneExt::CheckLegalName(GObject* node, const String& name)
{
	if (!node || name.empty())
	{
		return false;
	}

	GObject* parent = node->GetRoot();
	if (parent)
	{
		auto& children = parent->GetChildren();
		for (auto& it : children)
		{
			bool nodeShow = !it->isLayer(MC_MASK_EDITOR_SCENE_LAYER)
				&& !it->isLayer(MC_MASK_EDITOR_UI_LAYER)
				&& !it->isLayer(MC_MASK_EDITOR_CLOTHDEBUG_LAYER);
			if (it != node && nodeShow && it->GetName() == name)
			{
				return false;
			}
		}
	}

	return true;
}

void MSceneExt::SetPrefabPathInEdSc(GObject* node, const String& name)
{
	node->SetPrefabPath(name);
	node->SetHostPrefabPath(name);
	auto& children = node->GetChildren();
	for (auto& it : children)
	{
		SetHostPrefabPathInEdSc(it, name);
	}
}

void MSceneExt::SetHostPrefabPathInEdSc(GObject* node, const String& name)
{
	node->SetPrefabPath("");
	node->SetHostPrefabPath(name);
	auto& children = node->GetChildren();
	for (auto& it : children)
	{
		SetHostPrefabPathInEdSc(it, name);
	}
}

void MSceneExt::GetNoClothDebugChildrens(GObject* node, Vector<GObject*>& outChildren)
{
	const auto& nodes = node->GetChildren();
	for (auto bonepiar : nodes)
	{
		GObject* childnode = bonepiar;
		if (!childnode->isLayer(MC_MASK_EDITOR_CLOTHDEBUG_LAYER))
			outChildren.push_back(childnode);
	}
}

NS_JYE_END
