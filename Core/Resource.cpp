
#include "Resource.h"

NS_JYE_BEGIN

String Resource::ms_FileSuffix[] =
{
	String("TEXTURE"),
	String("SKMODEL"),
	String("STMODEL"),
	String("ACTION"),
	String("MATERIAL"),
	String("POSTEFFECT"),
	String("SHADER"),
	String("ANIMTREE"),
	String("MORPHTREE"),
	String("TERRAIN"),
	String("FSM"),
	String("ACTOR"),
	String("MAP"),
	String("FONT"),
	String("")
};

const String& Resource::GetFileSuffix(uint uiFileSuffix)
{
	if (uiFileSuffix > RT_MAX)
		return ms_FileSuffix[RT_MAX];
	return ms_FileSuffix[uiFileSuffix];
}

NS_JYE_END
