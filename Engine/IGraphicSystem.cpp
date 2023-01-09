#include "IGraphicSystem.h"
#include "Core/Interface/IApplication.h"
#include "RHI/RHI.h"
#include "pipeline/PipelineSystem.h"
#include "pipeline/RenderContext.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(IGraphicSystem);

IGraphicSystem::IGraphicSystem()
	: m_pMainWindow(NULL)
	, m_pMainRenderContex(NULL)
{
}

IGraphicSystem::~IGraphicSystem()
{
}

void IGraphicSystem::Update()
{

}

void IGraphicSystem::Initialize()
{
	m_pMainWindow = IApplication::Instance()->GetMainWindow();
	InsertRenderWindow(m_pMainWindow);
}

void IGraphicSystem::Render()
{
	_RenderCheck();

	RHIResetViewID();

	m_pMainRenderContex->Begin();
	for (auto wcpair : m_RenderContextList)
	{
		PipelineSystem::Instance()->RenderWindow(wcpair.second, *m_pMainRenderContex);
	}
	m_pMainRenderContex->End();

	RHIFrame();
}

RenderContext* IGraphicSystem::CreateRenderContex()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	RenderContext* pRender = _NEW RenderContext();
	m_renderContexs.push_back(pRender);
	return pRender;
}

void IGraphicSystem::ReleaseRenderContex(RenderContext* pRenderContex)
{
	auto size = m_renderContexs.size();
	for (int i = 0; i < size; i++)
	{
		if (m_renderContexs[i] == pRenderContex)
		{
			SAFE_DELETE(pRenderContex);
			m_renderContexs.erase(m_renderContexs.begin() + i);
			break;
		}
	}
}

void IGraphicSystem::_RenderCheck()
{
	if (!m_pMainRenderContex)
	{
		m_pMainRenderContex = CreateRenderContex();
	}
}

NS_JYE_END