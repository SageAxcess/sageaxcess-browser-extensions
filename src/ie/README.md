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
6. Open Internet Explorer
7. Go to Internet Options --> Security --> Trusted Sites --> Sites
8. Set "Require server verification (https:) for all sites in this zone" to false
9. Add http://localhost to trusted websites
10. Go back to Trusted Sites and click on Custom level button
11. Set "Display Mixed Content" to "Enable"
12. Go to Internet Options --> Security --> Internet --> Custom level
13. Set "Display Mixed Content" to "Enable"
14. Save and restart browser
