#pragma once

#include "Engine/private/Define.h"
#include "Engine/render/material/ShaderKeyWords.h"
#include "Engine/render/material/BuiltinParams.h"
#include "Engine/render/material/ParameterArray.h"
#include "Engine/render/RenderObjectEntity.h"
#include "Engine/render/material/MaterialEntity.h"
#include "RenderContext.h"

NS_JYE_BEGIN

class RenderComponent;
class LightComponent;
class IAsyncSource;

struct InstanceData
{
	RenderComponent* renderCompoent;
	MaterialEntity* pMat;
	unsigned char passIndex;
	ShaderKeyWords keyWords;
	ParameterArray* geoParams;
	RenderObjectEntity* renderObj;
	uint16 subMeshIdx;
};

#define ConstBufferSize 4096

class InstanceBatcher
{
public:
	InstanceBatcher(RenderContext& renderContex)
		: pRenderContex(&renderContex)
		, m_instanceCount(0)
		, m_subMeshHash(0)
		, m_matHashCode(0)
	{
			
	}

	~InstanceBatcher()
	{

	}

	void AddBatch(InstanceData& instanceData);

	void Flush();
private:
	void _ApplyNodeParam(InstanceData& instanceData);
	void _ApplyInstanceNodeParam(InstanceData& instanceData, std::vector<RenderComponent*>& nodeList);
private:
	RenderContext * pRenderContex;
	BuiltinFlag m_builtFlag;
	PassInfo m_passInfo;
	uint16 m_instanceCount;
	size_t m_subMeshHash;
	size_t m_matHashCode;
	BuiltinFlag m_BuilinFlag;
	InstanceData m_InstanceData;

	std::vector<RenderComponent*> m_nodeList;
};

NS_JYE_END
