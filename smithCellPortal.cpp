#include "smith.h"

void PerformClip();

extern "C"
{
	SMITHCALLS* smith = nullptr;

	int __cdecl SmithQueryPlugin(PLUGININFO& p)
	{
		strcpy(p.name, "Cell and Portal");
		strcpy(p.author, "bahstrike");
		strcpy(p.attributions, "shinyquagsire23&OpenJKDF2 decompilation project: https://github.com/shinyquagsire23/OpenJKDF2");
		strcpy(p.desc, "Attempts to implement original JK cell&portal algorithm.");
		strcpy(p.homepageURL, "https://github.com/bahstrike/smithCellPortal");
		p.smithRequiredVer = SMITHVERSION;
		p.ver = 100;
		//p.purpose = PP_TEXTUREUPSCALE;

		//p.authoritykey

		return 1337;
	}

	int __cdecl InitializePlugin(SMITHCALLS* _smith)
	{
		smith = _smith;


		return true;
	}

	void __cdecl ShutdownPlugin()
	{
		

		smith = nullptr;
	}

	void __cdecl whatever()
	{
		PerformClip();
	}
}