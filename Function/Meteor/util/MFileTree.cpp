#include "MFileTree.h"
#include "Core/Interface/ILogSystem.h"
#include "Engine/object/ObjectIdAllocator.h"
#include "System/Utility.hpp"

NS_JYE_BEGIN

MFileTree* g_FileTree = nullptr;

MFileItem::MFileItem(const String& name, bool isDir)
	: m_name(name)
	, m_isDirectory(isDir)
	, m_visible(true)
	, m_uuid(Utility::HashCode(name))
	, m_RootNode(nullptr)
{
	
}

MFileItem::~MFileItem()
{

}

bool MFileItem::AddItem(MFileItem* node)
{
	if (NULL != node
		&& std::find(m_children.begin(), m_children.end(), node) == m_children.end()
		&& m_RootNode != node)
	{
		if (NULL != node->m_RootNode)
		{
			node->m_RootNode->_DetachNode(node);
		}

		node->_SetFather(this);
		m_children.push_back(node);
		return true;
	}
	JYERROR("Can't AddItem !!");
	return false;
}

bool MFileItem::RemoveItem(MFileItem* actor)
{
	if (_DetachNode(actor))
	{
		actor->_SetFather(NULL);
		return true;
	}
	return false;
}

void MFileItem::_SetFather(MFileItem* root)
{
	m_RootNode = root;
}

bool MFileItem::_DetachNode(MFileItem* actor)
{
	auto it = std::find(m_children.begin(), m_children.end(), actor);
	if (m_children.end() != it)
	{
		m_children.erase(it);
		return true;
	}
	return false;
}


MFileTree::MFileTree(const String& rootpath)
{
	m_rootObject = CreateFileItem(rootpath, true);
}

MFileTree::~MFileTree()
{
	DeleteNode(m_rootObject);
}

MFileItem* MFileTree::CreateFileItem(const String& name, bool bdir)
{
	MFileItem* node = _NEW MFileItem(name, bdir);
	if (m_rootObject)
	{
		m_rootObject->AddItem(node);
	}
	m_allFiles[node->GetUUID()] = node;

	return node;
}

MFileItem* MFileTree::GetNodeByID(uint id)
{
	auto itr = m_allFiles.find(id);
	if (itr != m_allFiles.end())
	{
		return (itr->second);
	}
	return nullptr;
}

void MFileTree::DeleteNode(uint id)
{
	auto itr = m_allFiles.find(id);
	if (itr != m_allFiles.end())
	{
		DeleteNode(itr->second);
	}
}

void MFileTree::DeleteNode(MFileItem* node)
{
	if (node == NULL)  return;

	if (m_rootObject && m_rootObject->GetUUID() == node->GetUUID())
		m_rootObject = NULL;

	MFileItem* root = node->GetParent();
	if (root)
	{
		root->RemoveItem(node);
	}

	_DoDeleteNode(node);
}

void MFileTree::_DoDeleteNode(MFileItem* item)
{
	auto itr = m_allFiles.find(item->GetUUID());
	if (itr != m_allFiles.end())
	{
		const auto& nodes = item->GetChildren();
		for (auto it = nodes.begin(); nodes.end() != it;)
		{
			MFileItem* current = *it;
			++it;
			_DoDeleteNode(current);
		}
		SAFE_DELETE(itr->second);
		m_allFiles.erase(itr);
	}
	else
	{
		JYERROR("error:MFileTree can't find node by Id!");
	}
}

NS_JYE_END