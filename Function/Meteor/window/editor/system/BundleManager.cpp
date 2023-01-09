#include "BundleManager.h"
#include "util/MSceneManagerExt.h"

NS_JYE_BEGIN

void BundleManager::SaveScene(const String& file)
{
	Scene* scene = MSceneManagerExt::Instance()->GetEditScene(); // ��scene��ʼ���л�
	Stream stream ;
	stream.SetStreamFlag(Stream::AT_REGISTER);
	stream.ArchiveAll(scene);
	stream.Save(file.c_str());
}

NS_JYE_END