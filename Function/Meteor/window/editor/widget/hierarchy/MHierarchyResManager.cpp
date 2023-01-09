#include "MHierarchyResManager.h"
#include "Engine/resource/TextureMetadata.h"
#include "Engine/render/texture/TextureEntity.h"
#include "Engine/object/GObject.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Engine/component/light/LightComponent.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(MHierarchyResManager);

MHierarchyResManager::MHierarchyResManager()
{
	m_res = 
	{
		{"comm:documents/icons/new.png", "New object", nullptr},
		{"comm:documents/icons/layer.png", "Layer", nullptr},
		{"comm:documents/icons/quad.png", "Quad", nullptr},
		{"comm:documents/icons/light.png", "Light", nullptr},
		{"comm:documents/icons/camera.png", "Camera", nullptr},
		{"comm:documents/icons/sphere.png", "Sphere", nullptr},
		{"comm:documents/icons/box.png", "Box", nullptr},
		{"comm:documents/icons/particular.png", "Particular", nullptr},
		{"comm:documents/icons/effect.png", "Effect", nullptr},
		{"comm:documents/icons/face.png", "Face", nullptr},
	};
}

MHierarchyResManager::~MHierarchyResManager()
{

}

void MHierarchyResManager::_OnCreate()
{
}

void MHierarchyResManager::_OnDestroy()
{
}

void MHierarchyResManager::LoadResources()
{
	for (auto& it : m_res)
	{
		TextureEntity * tex = _NEW TextureEntity();
		tex->PushMetadata(
			TextureFileMetadata(
				RHIDefine::TEXTURE_2D,
				RHIDefine::TU_STATIC,
				RHIDefine::PixelFormat::PF_AUTO,
				1,
				true,
				0,
				RHIDefine::TextureWarp::TW_REPEAT,
				RHIDefine::TextureWarp::TW_REPEAT,
				RHIDefine::TextureFilter::TF_LINEAR,
				RHIDefine::TextureFilter::TF_LINEAR,
				it.m_path, true, false));
		tex->CreateResource();
		it.m_tex = tex;
	}
}

TextureEntity* MHierarchyResManager::GetNodeIcon(GObject* node)
{
	if (!node)	return m_res[M_Hierarchy_Effect].m_tex;

	auto& coms = node->GetComponents();

	// 优先判断是否是相机
	for (auto& it : coms)
	{
		if (it->IsSameType(CameraComponent::RTTI()))
			return m_res[M_Hierarchy_Camera].m_tex;
	}

	// 再判断是否是灯光
	for (auto& it : coms)
	{
		if (it->IsSameType(LightComponent::RTTI()))
			return m_res[M_Hierarchy_Light].m_tex;
	}


	// 最后判断mesh
	for (auto& it : coms)
	{
		if (it->IsSameType(RenderComponent::RTTI()))
		{
			RenderComponent* render = static_cast<RenderComponent*>(it);
			String& name = render->GetRenderObjectEntity()->GetMeshName();
			if (name == "box")
			{
				return m_res[M_Hierarchy_Box].m_tex;
			}
			else if (name == "sphere")
			{
				return m_res[M_Hierarchy_Sphere].m_tex;
			}
			else if (name == "face")
			{
				return m_res[M_Hierarchy_Face].m_tex;
			}
			else if (name == "quad")
			{
				return m_res[M_Hierarchy_Quad].m_tex;
			}
		}
	}

	return m_res[M_Hierarchy_Effect].m_tex;
}

NS_JYE_END