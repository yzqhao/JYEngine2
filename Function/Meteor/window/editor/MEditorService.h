
#pragma once

#include "window/MWindowBase.h"
#include "System/Delegate.h"

NS_JYE_BEGIN

class MEditor;

class MEditorService
{
	DECLARE_MULTICAST_DELEGATE(EditorCallbackList);
public:
	MEditorService(MEditor* svc);
	virtual ~MEditorService();

	FORCEINLINE DelegateHandle RegisterCallback(EditorCallbackListDelegate& evt) { return m_editorCallEvents.Add(std::move(evt)); }

	void CallbackFunction(DelegateHandle& hd);

private:
	MEditor* m_editor;

	EditorCallbackList m_editorCallEvents;
};

void InitService(MEditor* editor);

extern MEditorService* g_editorSvc;

NS_JYE_END