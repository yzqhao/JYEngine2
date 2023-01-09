#include "IRenderQueue.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/object/GObject.h"

NS_JYE_BEGIN

IRenderQueue::IRenderQueue() 
{

}

IRenderQueue::~IRenderQueue()
{

}

void IRenderQueue::Update()
{
	for (auto& it : m_pAttachedComponentMap)
	{
		if (it.second->isSuccess() && it.second->isActiveHierarchy())
		{
			it.second->TryRecaculateBindBox();
		}

		// ¼ÆËã³¡¾°°üÎ§ºÐ
		m_SceneBoundBox.Reset();
		for (auto& it : m_pAttachedComponentMap)
		{
			if (it.second->isRenderProperty(GraphicDefine::RP_SHOW)
				&& it.second->isActiveHierarchy())
			{
				if (it.second->isRenderProperty(GraphicDefine::RP_SHADOW_RECEIVER) ||
					it.second->isRenderProperty(GraphicDefine::RP_SHADOW_CASTER))
				{
					const std::vector<MaterialEntity*>& materials = it.second->GetMaterialEntities();
					bool supportShadow = false;
					for (int i = 0; i < materials.size(); i++)
					{
						supportShadow = materials[i]->IsSupportPass(RHIDefine::PT_DEPTHPASS);

						if (supportShadow)
						{
							break;
						}
					}

					if (supportShadow)
					{
						m_SceneBoundBox.Merge(it.second->GetBindBox());
					}
				}
			}
		}
	}
}

void IRenderQueue::AttachRenderComponent(RenderComponent* rc)
{
	m_pAttachedComponentMap.insert({ rc->GetStaticID(), rc });
}

void IRenderQueue::DetachRenderComponent(RenderComponent* rc)
{
	auto it = m_pAttachedComponentMap.find(rc->GetStaticID());
	if (m_pAttachedComponentMap.end() != it)
	{
		m_pAttachedComponentMap.erase(it);
	}
}

void IRenderQueue::OnCulling(uint64 mask, int seq, bool scull, GraphicDefine::PiplineType pp, CameraComponent* cam)
{
	m_rpRendering.clear();
	switch (pp)
	{
	case GraphicDefine::PP_FORWARD_LIGHTING:
	case GraphicDefine::PP_DEFERRED_LIGHTING:
	case GraphicDefine::PP_UI:
	{
		for (auto& it : m_pAttachedComponentMap)
		{
			if (it.second->isRenderProperty(GraphicDefine::RP_SHOW)
				&& it.second->isSuccess()
				&& (!scull || seq == it.second->GetSequence())
				&& it.second->GetParentObject()->isLayer(mask)
				&& it.second->isActiveHierarchy()
				)
			{
				m_rpRendering.push_back(it.second);
			}
		}
		break;
	}
	case GraphicDefine::PP_DEPTH:
	{
		for (auto& it : m_pAttachedComponentMap)
		{
			if (it.second->isRenderProperty(GraphicDefine::RP_SHOW)
				&& it.second->isSuccess()
				&& (!scull || seq == it.second->GetSequence())
				&& it.second->GetParentObject()->isLayer(mask)
				&& it.second->isActiveHierarchy()
				&& (it.second->isRenderProperty(GraphicDefine::RP_SHADOW_CASTER))
				)
			{
				m_rpRendering.push_back(it.second);
			}
		}
		break;
	}
	default:
	{
		JYERROR("list render queue: unkown pipeline pass");
		break;
	}
	}
}

Vector<GObject*> IRenderQueue::PickNodes(uint64 mask, int seq, bool scull, const Math::Ray& rhs)
{
	std::multimap<float, GObject*> List_dist_Node;
	for (auto itr = m_pAttachedComponentMap.begin(); itr != m_pAttachedComponentMap.end(); itr++)
	{
		RenderComponent* p = itr->second;
		GObject* a = p->GetParentObject();
		if (p->isRenderProperty(GraphicDefine::RP_SHOW)
			&& p->isSuccess()
			&& (!p->isRenderProperty(GraphicDefine::RP_IGNORE_PICK))
			&& (!scull || seq == p->GetSequence())
			&& a->isLayer(mask)
			&& a->isActiveHierarchy())
		{
			const Math::AABB  lhs = p->GetBindBox();	//AABBOx
			Math::Vec3 intersectpoint;
			if (Math::MathUtil::intersects(rhs, lhs, intersectpoint))
			{
				Math::Vec3 rayorigin = rhs.GetRayOrigin();
				float dist = (intersectpoint - rayorigin).Length();
				List_dist_Node.insert({ dist, a });
			}
		}
	}

	Vector<GObject*> res;
	res.reserve(List_dist_Node.size());
	for (auto& it : List_dist_Node)
	{
		res.push_back(it.second);
	}
	return std::move(res);
}

NS_JYE_END