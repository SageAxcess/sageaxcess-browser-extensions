

if exist ".\\Temp" rd /q /s ".\\Temp"
mkdir ".\\Temp"

pushd ".\Binaries\"
for %%i in (*.dll) do copy "%%i" "..\Temp\%%i"
popd

pushd ".\Binaries\"
for %%i in (*.js) do copy "%%i" "..\Temp\%%i"
popd

pushd ".\Binaries\"
for %%i in (*.html) do copy "%%i" "..\Temp\%%i"
popd


"C:\Program Files (x86)\NSIS\makensis.exe" /DEXTENSIONPATH="..\\Temp\\" /DPRODUCT_VERSION="1.0.0" /DNAME="SageAxcess" /DMANUFACTURER="SageAxcess" /DOUTFILE="..\\SageAxcess.${PRODUCT_VERSION}.exe" "Installer\\config.nsi" /V2

rmdir ".\\Temp" /s /q


