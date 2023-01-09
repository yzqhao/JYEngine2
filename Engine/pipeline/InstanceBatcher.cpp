
#include "InstanceBatcher.h"
#include "Engine/component/light/LightComponent.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/component/transform/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/GObject.h"

#include <algorithm>

NS_JYE_BEGIN

void InstanceBatcher::_ApplyNodeParam(InstanceData& instanceData)
{
	const BuiltinFlag& buildFlag = m_passInfo.pShaderEntity->GetBuiltFlagNotBatch();
	BuiltinParams* instance = pRenderContex->GetBuiltinParams();
	const GObject* pNode = instanceData.renderCompoent->GetParentObject();

	Math::Mat4 identity_mat;
	// PS_LOCALWORLD_TRANSFORM
	Math::Mat4* local2WorldMat = nullptr;
	if (false)		// sin
	{
		SetBuiltParam(instance, RHIDefine::PS_LOCALWORLD_TRANSFORM, MakeMaterialParam(Math::Mat4()));
	}
	else
	{
		local2WorldMat = (Math::Mat4*)pNode->Attribute<Math::Mat4>(GraphicDefine::GA_WORLD_TRANSFORM);
		if (local2WorldMat)
		{
			SetBuiltParam(instance, RHIDefine::PS_LOCALWORLD_TRANSFORM, MakeMaterialParam(Math::Mat4(*local2WorldMat)));
		}
	}

	static int bindWorld2Local = instance->GetBindIndex(RHIDefine::PS_WORLDLOCAL_TRANSFROM);

	if (buildFlag.test(bindWorld2Local))
	{
		Math::Mat4* world2localMat = (Math::Mat4*)pNode->Attribute<Math::Mat4>(GraphicDefine::GA_WORLD_TRANSFORM_INVERS);
		if (world2localMat)
		{
			SetBuiltParam(instance, RHIDefine::PS_WORLDLOCAL_TRANSFROM, MakeMaterialParam(Math::Mat4(*world2localMat)));
		}
		else
		{
			SetBuiltParam(instance, RHIDefine::PS_WORLDLOCAL_TRANSFROM, MakeMaterialParam(Math::Mat4()));
		}
	}

	// PS_LOCALSCREEN_TRANSVIEWPROJ
	static int bindWVPIndex = instance->GetBindIndex(RHIDefine::PS_LOCALSCREEN_TRANSVIEWPROJ);
	if (buildFlag.test(bindWVPIndex))
	{
		static int bindVPIndex = instance->GetBindIndex(RHIDefine::PS_CAMERA_VIEWPROJ);
		const Object* p_vpmatrixVC = instance->GetValue(bindVPIndex).pObj;
		const Math::Mat4* p_vpmatrix = dynamic_cast<const Math::Mat4*>(p_vpmatrixVC);
		Math::Mat4 wvpmatrix = *p_vpmatrix;

		if (local2WorldMat != nullptr)
		{
			wvpmatrix = *local2WorldMat * wvpmatrix;
		}

		SetBuiltParam(instance, RHIDefine::PS_LOCALSCREEN_TRANSVIEWPROJ, MakeMaterialParam(Math::Mat4(*local2WorldMat)));
	}

	// PS_LOCALWORLD_ROTATION
	static int bind_localrIndex = instance->GetBindIndex(RHIDefine::PS_LOCALWORLD_ROTATION);
	if (buildFlag.test(bind_localrIndex))
	{
		Math::Mat3* local2WorldRot = (Math::Mat3*)(pNode->Attribute<Math::Mat3>(GraphicDefine::GA_NORMAL_TRANSFORM));
		if (local2WorldRot != nullptr)
		{
			SetBuiltParam(instance, RHIDefine::PS_LOCALWORLD_ROTATION, MakeMaterialParam(Math::Mat3(*local2WorldRot)));
		}
	}

	// PS_WORLD_POSITION
	static int bind_world_pos = instance->GetBindIndex(RHIDefine::PS_WORLD_POSITION);
	if (buildFlag.test(bind_world_pos))
	{
		Math::Vec3* worldPos = (Math::Vec3*)pNode->Attribute<Math::Vec3>(GraphicDefine::GA_WORLD_POSITION);
		if (worldPos != nullptr)
		{
			SetBuiltParam(instance, RHIDefine::PS_WORLD_POSITION, MakeMaterialParam(Math::Vec3(*worldPos)));
		}
	}

	// PS_WORLD_SCALE
	static int bind_world_scale = instance->GetBindIndex(RHIDefine::PS_WORLD_SCALE);
	if (buildFlag.test(bind_world_scale))
	{
		Math::Vec3* worldScale = (Math::Vec3*)pNode->Attribute<Math::Vec3>(GraphicDefine::GA_WORLD_SCALE);
		if (worldScale != nullptr)
		{
			SetBuiltParam(instance, RHIDefine::PS_WORLD_SCALE, MakeMaterialParam(Math::Vec3(*worldScale)));
		}
	}
}

#define FLOAT4X4SIZE 64

/**
* a normal member write data to constBuffer.
* @param pBuffer which is dest buffer to write in.
* @param uniform which is description of const buffer.
* @param nodeList which is the node list to draw.
* @param alignBytes which is align bytes to write.
* @param offsetBuffer which is offset bytes of the last write.
*/

void WriteConstBuffer(unsigned char* pBuffer, ShaderUniform& uniform, std::vector<RenderComponent*>& nodeList, unsigned short alignBytes, unsigned short & offsetBuffer)
{
	/*
	int size = nodeList.size();
	uint16 offset = uniform.regIndex;
	unsigned char* pOffsetBuffer = pBuffer + offset;

	// 目前只支持一个矩阵的设置
	if (uniform.slot == RHIDefine::ParameterSlot::PS_LOCALWORLD_TRANSFORM)
	{
		uint16 align = Max(alignBytes, (uint16)FLOAT4X4SIZE);

		for (int i = 0; i < size; i++)
		{
			GObject* pNode = nodeList[i]->GetHostNode();

			Math::Mat4* local2WorldMat = (Math::Mat4*)(pNode->Attribute(GraphicDefine::GA_WORLD_TRANSFORM));
			void* pDest = pOffsetBuffer;
			memcpy(pDest, local2WorldMat->data, sizeof(local2WorldMat->data));

			pOffsetBuffer += align;
		}

		offsetBuffer = pOffsetBuffer - pBuffer - align + FLOAT4X4SIZE;
	}
	*/
}

void InstanceBatcher::_ApplyInstanceNodeParam(InstanceData& instanceData, std::vector<RenderComponent*>& nodeList)
{
	/*
	UnifromFunctionList& unfiromList = m_passInfo.pShaderEntity->GetBatchUniformList();

	for (auto& it : unfiromList)
	{
		ShaderUniform* uniform = it;
		std::vector<ShaderUniform*>& subVars = uniform->subUniforms;
		uint16 constBufferSize = uniform->regCount;
		uint16 alignBytes = ((uniform->num > 1) ? uniform->regCount / uniform->num : 0);
		uint16 writeBytes = 0;

		unsigned char * pBuffer = _NEW unsigned char[constBufferSize];
		for (int i = 0; i < subVars.size(); i++)
		{
			uint16 bufOffset = 0;
			WriteConstBuffer(pBuffer, *subVars[i], nodeList, alignBytes, bufOffset);
			writeBytes = std::max(writeBytes, bufOffset);
		}

		handle curHandle = pRenderContex->GetConstBufferMgr()->RequestBuffer(constBufferSize);
		pRenderContex->GetConstBufferMgr()->UpdateData(curHandle, pBuffer, writeBytes);
		Math::vector1ud constBufferid(curHandle);
		pRenderContex->GetBuiltinParams()->SetValue(uniform->bindIndex, &constBufferid);
		SAFE_DELETE(pBuffer);
	}
	*/
}

void InstanceBatcher::AddBatch(InstanceData& instanceData)
{
	bool canBatch = false;	// 暂时写死

	if (!canBatch)
	{
		Flush();
		pRenderContex->GenShaderPass(instanceData.pMat, instanceData.passIndex, instanceData.keyWords, instanceData.geoParams, m_passInfo);
		_ApplyNodeParam(instanceData);
		pRenderContex->DrawShaderPass(m_passInfo, instanceData.renderObj, instanceData.subMeshIdx, 1);
	}
	else
	{
		bool canBatchNode = false;

		if (m_instanceCount > 0)
		{
			size_t matHashCode = instanceData.pMat->GetHashCode();
			size_t subMeshHashCode = instanceData.renderObj->GetSubMeshHashByIndex(instanceData.subMeshIdx);
			BuiltinFlag builinFlag = pRenderContex->GetBuiltinParams()->EndRecordFlag();

			canBatchNode = (m_matHashCode == matHashCode
				&& m_InstanceData.keyWords == instanceData.keyWords
				&& m_InstanceData.passIndex == instanceData.passIndex
				&& m_InstanceData.geoParams == m_InstanceData.geoParams
				&& m_subMeshHash == subMeshHashCode
				&& (m_BuilinFlag&builinFlag) == 0);
		}

		if (canBatchNode)
		{
			m_nodeList.push_back(instanceData.renderCompoent);
			m_instanceCount++;
		}
		else
		{
			Flush();

			instanceData.keyWords.Enable(KeyWordUtil::GPUINSTANCING);
			pRenderContex->GenShaderPass(instanceData.pMat, instanceData.passIndex, instanceData.keyWords, instanceData.geoParams, m_passInfo);
			instanceData.keyWords.Disable(KeyWordUtil::GPUINSTANCING);

			pRenderContex->GetBuiltinParams()->BeginRecordFlag();
			m_InstanceData = instanceData;
			m_subMeshHash = m_InstanceData.renderObj->GetSubMeshHashByIndex(m_InstanceData.subMeshIdx);
			m_matHashCode = m_InstanceData.pMat->GetHashCode();
			m_BuilinFlag = m_passInfo.pShaderEntity->GetBuiltFlagNotBatch();
			m_nodeList.push_back(instanceData.renderCompoent);
			m_instanceCount = 1;
		}
	}
}

void InstanceBatcher::Flush()
{
	if (m_instanceCount)
	{
		_ApplyInstanceNodeParam(m_InstanceData, m_nodeList);
		pRenderContex->DrawShaderPass(m_passInfo, m_InstanceData.renderObj, m_InstanceData.subMeshIdx, m_instanceCount);
		m_nodeList.clear();
		m_instanceCount = 0;
	}
}

NS_JYE_END
