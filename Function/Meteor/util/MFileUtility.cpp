#include "MFileUtility.h"
#include "Core/Interface/IFileSystem.h"
#include "window/editor/system/MDefined.h"
#include "System/Utility.hpp"
#include "Engine/resource/TextureMetadata.h"
#include "MFileTree.h"

#include <algorithm>
#include <filesystem>

NS_JYE_BEGIN

Map<String, String> MDefined::SystemDirectories;

using namespace std;
using namespace std::filesystem;

static bool s_isEqualPathExt(const String& str, const String& ext)
{
	if (str.size() == ext.size())
	{
		return str == ext;
	}
	else if (str.size() == ext.size() - 1)
	{
		bool bcheck = true;
		for (int i = 0; i < str.size(); ++i)
		{
			if (str[i] != ext[i + 1])
			{
				bcheck = false;
				break;
			}
		}
		return bcheck;
	}

	return false;
}

bool MFileUtility::Contain(const Vector<String>& strs, const String& findstr)
{
	for (int i = 0; i < strs.size(); ++i)
	{
		if (s_isEqualPathExt(strs[i], findstr))
		{
			return true;
		}
	}

	return false;
}

void MFileUtility::SearchFiles(const String& serachpath, const Vector<String>& ext, Vector<String>& out)
{
	String& rootpath = IFileSystem::Instance()->PathAssembly(serachpath);
	path ph(rootpath);
	directory_entry entry(ph);
	if (entry.status().type() == file_type::directory)
	{
		directory_iterator list(ph);	        //文件入口容器
		for (auto& it : list)
		{
			if (it.status().type() == std::filesystem::file_type::directory) //is folder
			{
				SearchFiles(it.path().string(), ext, out);
			}
			else if (it.status().type() == std::filesystem::file_type::regular) //is file
			{
				if (Contain(ext, it.path().extension().string()))
				{
					String sp = it.path().string();
					Utility::ReplaceAllString(sp, "\\", "/");
					out.push_back(sp);
				}
			}
		}
	}
}

String MFileUtility::getExtension(const String& filename)
{
	String ext;
	auto pos = filename.find_last_of('.');
	if (pos != String::npos)
	{
		ext = filename.substr(pos + 1);
	}
	return std::move(ext);
}

void MFileUtility::GetParentDirectory(const String& spath, String& parentpath)
{
	path p(spath);
	parentpath = p.parent_path().string();
}

void MFileUtility::GetParentDirectoryName(const String& spath, String& parentpath)
{
	path p(spath);
	parentpath = p.parent_path().filename().string();
}

void MFileUtility::getFileNameWithPosfix(const String& spath, String& outpath)
{
	path p(spath);
	outpath = p.filename().string();
}

void MFileUtility::getFileNameNotPosfix(const String& spath, String& outpath)
{
	path p(spath);
	outpath = p.stem().string();
}

void MFileUtility::MCreateDirectory(const String& spath)
{
	String absolutepath = IFileSystem::Instance()->PathAssembly(spath);
	if (!IFileSystem::Instance()->isFileExist(spath))
	{
		IFileSystem::Instance()->MakeFolder(absolutepath);
	}
}

void MFileUtility::MCreateDirectoryDefined(const String& projectpath)
{
	MDefined::SystemDirectories.clear();
	for (auto& it : MDefined::FirstDirectories)
	{
		String newpath = projectpath + it.second;
		MDefined::SystemDirectories.insert({ it.first, newpath });
		MFileUtility::MCreateDirectory(newpath);
	}
	for (auto& it : MDefined::SecondDirectories)
	{
		String newpath = projectpath + it.second;
		MDefined::SystemDirectories.insert({ it.first, newpath });
		MFileUtility::MCreateDirectory(newpath);
	}
}

void MFileUtility::ConverToRelativePath(const String& absolutepath, const String& targetPath, String& relativepath)
{
	relativepath = absolutepath;
	String replacestr = targetPath;
	String projectpath = IFileSystem::Instance()->PathAssembly(replacestr);
	auto sidx = absolutepath.find(projectpath);
	if (sidx != String::npos)
	{
		relativepath = absolutepath.substr(projectpath.size());
	}
	relativepath = replacestr + relativepath;
}

void MFileUtility::GetCleanPathForDialog(const String& strpath, String& outpath)
{
	if (strpath.empty()) return;
	bool slashFirst = strpath[0] == '/';
	String temppath = IFileSystem::Instance()->PathAssembly(strpath);
	Utility::ReplaceAllString(temppath, "\\", "/");
	Vector<String> temp = Utility::split(temppath, "/");
	while (Contain(temp, ".."))
	{
		Array<int, 2> removelist;
		for (int i = 0; i < temp.size(); ++i)
		{
			if (temp[i] == "..")
			{
				removelist[0] = i;
				removelist[1] = i - 1;
				break;
			}
		}
		temp.erase(temp.begin() + removelist[0]);
		temp.erase(temp.begin() + removelist[1]);
	}
	temppath = "";
	for (int i = 0; i < temp.size(); ++i)
	{
		temppath += temp[i] + "/";
	}
	outpath = slashFirst ? ("/" + temppath) : temppath;
}

String MFileUtility::GetParentDirectory(const String& strpath)
{
	auto pos = strpath.find_last_of('/');
	JY_ASSERT(pos != String::npos);
	return std::move(strpath.substr(0, pos));
}

String MFileUtility::MGetCurrentDirectory(const String& strpath)
{
	directory_entry entry(strpath);
	if (entry.status().type() == file_type::directory)
	{
		return std::move(strpath);
	}
	return std::move(GetParentDirectory(strpath));
}

String MFileUtility::GetRelativePath(const String& absolutepath, const Vector<String>& targetPaths)
{
	String relativepath = absolutepath;
	String replacestr = MDefined::ProjectPath;
	String projectpath = IFileSystem::Instance()->PathAssembly(replacestr);
	auto pos = absolutepath.find(projectpath);
	if (pos != String::npos)
	{
		JY_ASSERT(pos == 0);	// 一旦在proj路径找到，肯定是最开始就匹配
		relativepath = absolutepath.substr(pos + projectpath.size());
		relativepath = replacestr + relativepath;
		return relativepath;
	}

	for (int i = 0; i < targetPaths.size(); ++i)
	{
		const String& itpath = targetPaths[i];

		String targetpath = IFileSystem::Instance()->PathAssembly(itpath);
		std::replace(targetpath.begin(), targetpath.end(), '\\', '/');
		auto tarpos = absolutepath.find(targetpath);
		if (tarpos != String::npos)
		{
			JY_ASSERT(tarpos == 0);	// 一旦在proj路径找到，肯定是最开始就匹配
			relativepath = absolutepath.substr(tarpos + targetpath.size());
			relativepath = itpath + relativepath;
			return relativepath;
		}
	}

	{	// 在尝试下Internal Path
		auto& internalDir = MDefined::Internal;
		for (int i = 0; i < internalDir.size(); ++i)
		{
			String internalPath = IFileSystem::Instance()->PathAssembly(internalDir[i]);
			std::replace(internalPath.begin(), internalPath.end(), '\\', '/');
			auto posInter = absolutepath.find(internalPath);
			if (posInter != String::npos)
			{
				JY_ASSERT(posInter == 0);	
				relativepath = absolutepath.substr(posInter + internalPath.size());
				relativepath = internalDir[i] + relativepath;
			}
		}
	}
	return relativepath;
}

void MFileUtility::GetNoRepeatFileName(const String& targetPath, String& outPath)
{
	outPath = targetPath;
	int idx = 1;
	while (IFileSystem::Instance()->isFileExist(outPath))
	{
		String parentdir;
		MFileUtility::GetParentDirectory(outPath, parentdir);
		String name;
		MFileUtility::getFileNameNotPosfix(outPath, name);
		outPath = parentdir + "/" + name + "(" + std::to_string(idx) + ")" + "." + MFileUtility::getExtension(outPath);
	}
}

void MFileUtility::GetFilesInDir(Vector<String>& out, const String& strpath, bool bRecursive, const Vector<String>& exts)
{
	String& rootpath = IFileSystem::Instance()->PathAssembly(strpath);
	path ph(rootpath);
	directory_entry entry(ph);
	if (entry.status().type() == file_type::directory)
	{
		directory_iterator list(ph);	        //文件入口容器
		for (auto& it : list)
		{
			if (bRecursive && it.status().type() == std::filesystem::file_type::directory) //is folder
			{
				GetFilesInDir(out, it.path().string(), bRecursive, exts);
			}
			else if (it.status().type() == std::filesystem::file_type::regular) //is file
			{
				if (Contain(exts, it.path().extension().string()))
				{
					String sp = it.path().string();
					Utility::ReplaceAllString(sp, "\\", "/");
					out.push_back(sp);
				}
			}
		}
	}
}

static bool _DoFileSystemTree(MFileTree* pTree, MFileItem* root, const Vector<String>& exts = {})
{
	bool bfilter = !exts.empty();
	bool bfind = false;

	String& rootpath = IFileSystem::Instance()->PathAssembly(root->GetName());
	path ph(rootpath);
	directory_entry entry(ph);
	if (entry.status().type() == file_type::directory)
	{
		directory_iterator list(ph);	        //文件入口容器
		for (auto& it : list)
		{
			if (it.status().type() == std::filesystem::file_type::directory) //is folder
			{
				String realpath = it.path().string();
				std::replace(realpath.begin(), realpath.end(), '\\', '/');
				MFileItem* dir = pTree->CreateFileItem(realpath, true);
				if (_DoFileSystemTree(pTree, dir, exts))
				{
					bfind = true;
					root->AddItem(dir);
				}
				else
				{
					pTree->DeleteNode(dir);
				}
			}
			else if (it.status().type() == std::filesystem::file_type::regular) //is file
			{
				String realpath = it.path().string();
				std::replace(realpath.begin(), realpath.end(), '\\', '/');
				
				bool bToFind = bfilter && MFileUtility::Contain(exts, MFileUtility::getExtension(realpath));
				if (bToFind || !bfilter)
				{
					MFileItem* fileitem = pTree->CreateFileItem(realpath, false);
					root->AddItem(fileitem);
					bfind = true;
				}
			}
		}
	}

	return !bfilter || bfind;
}

MFileTree* MFileUtility::GetFileSystemTree(const String& serachpath)
{
	String rootpath = serachpath;
	if (rootpath.back() == '/')
	{
		rootpath = rootpath.substr(0, rootpath.size() - 1);
	}
	MFileTree* tree = _NEW MFileTree(rootpath);
	MFileItem* root = tree->GetRootNode();

	_DoFileSystemTree(tree, root);

	return tree;
}

void MFileUtility::AppendToFileTree(const String& newFilePath)
{
	if (!g_FileTree)	return;

	String realpath = newFilePath;
	std::replace(realpath.begin(), realpath.end(), '\\', '/');
	MFileItem* parent = g_FileTree->GetNodeByID(Utility::HashCode(realpath));
	if (parent)
	{
		MFileItem* fileitem = g_FileTree->CreateFileItem(realpath, false);
		parent->AddItem(fileitem);
	}
}

void MFileUtility::RemoveInFileTree(const String& newFilePath)
{
	if (!g_FileTree)	return;

	String realpath = newFilePath;
	std::replace(realpath.begin(), realpath.end(), '\\', '/');
	g_FileTree->DeleteNode(Utility::HashCode(realpath));
}

MFileTree* MFileUtility::GetDirectoryInFileTree(const String& targetPath, const Vector<String>& ext)
{
	String abpath = IFileSystem::Instance()->PathAssembly(targetPath);
	MFileTree* tree = _NEW MFileTree(abpath);
	MFileItem* root = tree->GetRootNode();
	_DoFileSystemTree(tree, root, ext);
	return tree;
}

String MFileUtility::GetTextureMeta(const String& targetPath)
{
	String metaPath = IFileSystem::Instance()->PathAssembly(targetPath + ".meta");
	if (!IFileSystem::Instance()->isFileExist(metaPath))
	{
		TextureDescribeDataPtr pTexDes = _NEW TextureDescribeData(); 
		Stream stream;
		stream.SetStreamFlag(Stream::AT_REGISTER);
		stream.ArchiveAll(pTexDes);
		stream.Save(metaPath.c_str());
	}
	return metaPath;
}

NS_JYE_END