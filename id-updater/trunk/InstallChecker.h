
#include <string>

class InstallChecker
{
public:
	InstallChecker(void);
	~InstallChecker(void);
	static void getInstalledVersion(std::wstring upgradeCode,std::wstring &version);
	static bool verifyPackage(std::wstring filePath,bool withUI = true);
	static bool installPackage(std::wstring filePath,bool reducedUI = false);
};
