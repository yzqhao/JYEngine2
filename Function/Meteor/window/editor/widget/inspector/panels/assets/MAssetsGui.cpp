
#include "MAssetsGui.h"
#include "MTexture.h"
#include "util/MFileUtility.h"
#include "../../../../system/MDefined.h"


#include "imgui.h"

NS_JYE_BEGIN

void MAssetsGui::OnGui(const String& assetspath)
{
	static const String strhash = "Assets";
	static const String strlabelhash = "Assets##Assets";
	if (ImGui::CollapsingHeader(strlabelhash.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TreePush(strlabelhash.c_str());
		String extension = MFileUtility::getExtension(assetspath);
		std::transform(extension.begin(), extension.end(), extension.begin(),
			[](unsigned char c) { return std::tolower(c); });

		if (MFileUtility::Contain(MDefined::FileTypeList[MDefined::Texture], extension))
		{
			MTexture::OnGui(assetspath);
		}

		ImGui::TreePop();
	}
}

NS_JYE_END