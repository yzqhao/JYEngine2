
#include "MEditorService.h"

NS_JYE_BEGIN

MEditorService* g_editorSvc = nullptr;

void InitService(MEditor* editor)
{
	g_editorSvc = new MEditorService(editor);
}

MEditorService::MEditorService(MEditor* svc)
	: m_editor(svc)
{
	
}

MEditorService::~MEditorService()
{

}

void MEditorService::CallbackFunction(DelegateHandle& hd)
{
	m_editorCallEvents.Signal(hd);
}

NS_JYE_END