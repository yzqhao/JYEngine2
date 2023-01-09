#include "private/Define.h"

NS_JYE_BEGIN

class MFileItem;
class MFileTree;

namespace MFileUtility
{
	bool Contain(const Vector<String>& strs, const String& findstr);
	void SearchFiles(const String& path, const Vector<String>& ext, Vector<String>& out);
	String getExtension(const String& filename);
	void GetParentDirectory(const String& spath, String& parentpath);
	void GetParentDirectoryName(const String& spath, String& parentpath);
	void getFileNameWithPosfix(const String& spath, String& outpath);
	void getFileNameNotPosfix(const String& spath, String& outpath);
	void MCreateDirectory(const String& spath);
	void MCreateDirectoryDefined(const String& spath);
	void ConverToRelativePath(const String& absolutepath, const String& targetPath, String& outpath);
	void GetCleanPathForDialog(const String& strpath, String& outpath);
	String GetParentDirectory(const String& strpath);
	String MGetCurrentDirectory(const String& strpath);
	String GetRelativePath(const String& strpath, const Vector<String>& targetPaths);
	void GetNoRepeatFileName(const String& targetPath, String& outPath);
	void GetFilesInDir(Vector<String>& out, const String& strpath, bool bRecursive, const Vector<String>& exts);
	MFileTree* GetFileSystemTree(const String& serarchpath);
	void AppendToFileTree(const String& newFilePath);
	void RemoveInFileTree(const String& newFilePath);
	MFileTree* GetDirectoryInFileTree(const String& targetPath, const Vector<String>& ext);
	String GetTextureMeta(const String& targetPath);
}

NS_JYE_END