For build sources you need 
- MSVS 2013 
- Windows SDK 7.1
- NSIS 2.46+ (http://sourceforge.net/projects/nsis/)

Steps:
1. Open solution ie.sln
2. If needed increment version in Config.cpp and in resources (Content.rc and CommonInterface.rc)
3. Change version in makeinstaller.bat
4. Build in Release configuration for both platform win32 and x64
5. Run makeinstaller.bat (check path to NSIS directory in this file)

