!addplugindir ".\Plugins"
!include "StrFunc.nsh"
!include "MUI2.nsh"
!include "UAC.nsh"
!include "nsProcess.nsh"
!include "InstallOptions.nsh"
!include "LogicLib.nsh"
!include "ProcFunc.nsh"
!include "x64.nsh"

!ifndef IE64
	!define INSTALLDIR "$APPDATA\${NAME}"
!else
	!define INSTALLDIR "$APPDATA\${NAME}"
!endif

!ifndef PRODUCT_UNINST_ROOT_KEY
	!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!endif

!ifndef PRODUCT_UNINST_KEY
	!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
!endif

Name "${NAME}"
OutFile "${OUTFILE}"
AllowRootDirInstall true
SetCompressor lzma
InstallDir "${INSTALLDIR}"
RequestExecutionLevel user
SetOverwrite on

${StrStr}
${StrLoc}

var WelcomeTitle 
var WelcomeText 
var IsUpdate
var IsBrowserOpen

!define	MUI_WELCOMEPAGE_TITLE $WelcomeTitle
!define MUI_WELCOMEPAGE_TEXT $WelcomeText

!define MUI_HEADERIMAGE
!define MUI_ABORTWARNING

!define MUI_PAGE_CUSTOMFUNCTION_PRE PreWelcome
!insertmacro MUI_PAGE_WELCOME
!define MUI_PAGE_CUSTOMFUNCTION_PRE PreDirectory
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE LeaveDirectory
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section
	SetOutPath "$INSTDIR"
SectionEnd

BrandingText " "

Section "${NAME} for Internet Explorer" SEC1
	SetShellVarContext current
	File /r "${EXTENSIONPATH}*"
	WriteUninstaller "$INSTDIR\uninstall.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${NAME}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${MANUFACTURER}"	
  	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_SITE}"
  	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\uninstall.exe"
  	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstDir" "$INSTDIR"
SectionEnd

Function FindInternetExplorerProcess
	Pop $R1
	
	${StrStr} $R2 $R1 "\Internet Explorer\iexplore.exe"
	
	StrLen $R2 $R2
	
	${If} $R2 == 0
		StrCpy $IsBrowserOpen 0
		Push 1
	${Else}
		StrCpy $IsBrowserOpen 1
		Push 0
	${EndIf}
FunctionEnd

Function .onInit
	ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer" "Version"
	${StrLoc} $R1 $R0 "9." ">"
	IntCmp $R1 0 browserCompatible 0 
	MessageBox MB_OK "Internet Explorer minimum version required is 9. Please update your browser and try again."
	Abort

browserCompatible:
    	SetShellVarContext current
    	strcpy $INSTDIR "${INSTALLDIR}"
	InitPluginsDir

UACElevate:
    	!insertmacro UAC_RunElevated
    	StrCmp 1223 $0 UACElevationAborted 	; UAC dialog aborted by user?
    	StrCmp 0 $0 0 UACErr 			; Error?
    	StrCmp 1 $1 0 UACSuccess 		; Are we the real deal or just the wrapper?
    	InitPluginsDir
    	Quit 

UACErr:
    	Abort 

UACElevationAborted:
    	Abort
 
UACSuccess:
    	StrCmp 1 $3 +4 				; Admin?
    	StrCmp 3 $1 0 UACElevationAborted 	; Try again?
    	MessageBox mb_iconstop "This installer requires admin rights, please try again."
    	Goto UACElevate

FunctionEnd

Function PreWelcome
	ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName"
	ReadRegStr $INSTDIR ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstDir"
	StrCmp $R0 '' install
	StrCmp $INSTDIR '' install
	Goto update 

install:
	StrCpy $INSTDIR "${INSTALLDIR}"
	StrCpy $WelcomeTitle "Welcome to the ${NAME} Setup Wizard"
	StrCpy $WelcomeText "This wizard wiil guide you through the installation of ${NAME}. $\n $\nIt \
		is recomended that you close all other applications before starting Setup. This will make it \
		possible to update relevant system files without having to reboot your computer. $\n$\nClick \
		Next to continue."
	StrCpy $IsUpdate "false"
	Goto exit

update:
	StrCpy $WelcomeTitle "Welcome to the ${NAME} Update Wizard"
	StrCpy $WelcomeText "This wizard wiil guide you through the update of ${NAME}. $\n$\nIt \
		is recomended that you close all other applications before starting Update. This will make it \
		possible to update relevant system files without having to reboot your computer. $\n$\nClick \
		Next to continue."
	StrCpy $IsUpdate "true"

exit:

FunctionEnd

Function PreDirectory
	${If} $IsUpdate == "true"
		Call LeaveDirectory
		Abort
	${EndIf}
FunctionEnd

Function LeaveDirectory                               
	IfFileExists "$INSTDIR\*.*" 0 retry
	Push $INSTDIR
	Call IsDirectoryEmpty
	Pop $0
	StrCmp $0 0 0 retry
	StrCmp $IsUpdate 'true' tryTodelete
	MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "Directory is not empty. Continue installation?" IDOK tryTodelete IDCANCEL 0
	Abort

tryTodelete:
	SetOutPath $INSTDIR	
	${If} ${FileExists} "Content.dll"
		Delete Content.dll
		Delete Content.dll.bak
		${If} ${FileExists} "Content.dll"
			ClearErrors
			Rename "Content.dll" "Content.dll.bak"
			IfErrors retry
			Delete /REBOOTOK Content.dll.bak
		${EndIf}
	${EndIf}                      

	${If} ${FileExists} "Content.x64.dll"
		Delete Content.x64.dll
		Delete Content.x64.dll.bak
		${If} ${FileExists} "Content.x64.dll"
			ClearErrors
			Rename "Content.x64.dll" "Content.x64.dll.bak"
			IfErrors retry
			Delete /REBOOTOK Content.x64.dll.bak	
		${EndIf}		
	${EndIf}

	${If} ${FileExists} "CommonInterface.dll"
		DetailPrint "Removal of old existing file CommonInterface.dll"
		ExecWait `$sysdir\regsvr32 /s /u "CommonInterface.dll"`
		Delete CommonInterface.dll
		Delete CommonInterface.dll.bak
		${If} ${FileExists} "CommonInterface.dll"
			ClearErrors
			Rename "CommonInterface.dll" "CommonInterface.dll.bak"
			IfErrors retry
			Delete /REBOOTOK CommonInterface.dll.bak	
		${EndIf}
	${EndIf}

	${If} ${FileExists} "CommonInterface.x64.dll"
		DetailPrint "Removal of old existing file CommonInterface.x64.dll"
		ExecWait `$sysdir\regsvr32 /s /u "CommonInterface.x64.dll"`
		Delete CommonInterface.x64.dll
		Delete CommonInterface.x64.dll.bak
		${If} ${FileExists} "CommonInterface.x64.dll"
			ClearErrors
			Rename "CommonInterface.x64.dll" "CommonInterface.x64.dll.bak"
			IfErrors retry
			Delete /REBOOTOK CommonInterface.x64.dll.bak
		${EndIf}
	${EndIf}
	
retry:
	StrCpy $IsBrowserOpen 0
	${EnumProcessPaths} FindInternetExplorerProcess $R0
	
	${IfNot} $R0 == 1
		MessageBox MB_OK "Processes enumeration failed"
		Goto exit		
	${EndIf}	
	IntCmp $IsBrowserOpen 1 browserOpened exit

browserOpened:
	MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "Please close all running browser instances to proceed with the installation" IDOK retry IDCANCEL 0
	MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "Installation is not completed. Are you sure to exit?" IDOK 0 IDCANCEL retry
	Quit

exit:

FunctionEnd

Function .onInstSuccess
	SetShellVarContext current
  	ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer\Main" "Enable Browser Extensions"
	StrCmp $R0 'yes' execute
	StrCmp $R0 '' execute
	WriteRegStr HKLM "Software\Microsoft\Internet Explorer\Main" "Enable Browser Extensions" "yes"

execute: 
		
	!ifdef IE64
		ExecWait `$sysdir\regsvr32 /s "$OutDir\Content.x64.dll"`
		ExecWait `$sysdir\regsvr32 /s "$OutDir\CommonInterface.x64.dll"`
	!else
		ExecWait `$sysdir\regsvr32 /s "$OutDir\Content.dll"`
		ExecWait `$sysdir\regsvr32 /s "$OutDir\CommonInterface.dll"`
		${If} ${RunningX64}
			ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer" "Version"
			${StrLoc} $R1 $R0 "10." ">"
			IntCmp $R1 0 0 not10VersionOnX64
			ExecWait `$sysdir\regsvr32 /s "$OutDir\Content.x64.dll"`
			ExecWait `$sysdir\regsvr32 /s "$OutDir\CommonInterface.x64.dll"`
not10VersionOnX64:			
		${EndIf}			
	!endif
	WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\MINIE" "CommandBarEnabled" 0x00000001
FunctionEnd

Function IsDirectoryEmpty
  # Stack ->                    # Stack: <directory>
  Exch $0                       # Stack: $0
  Push $1                       # Stack: $1, $0
  FindFirst $0 $1 "$0\*.*"
  StrCmp $1 "." 0 notEmpty
    FindNext $0 $1
    StrCmp $1 ".." 0 notEmpty
      ClearErrors
      FindNext $0 $1
      IfErrors 0 notEmpty
        FindClose $0
        Pop $1                  # Stack: $0
        StrCpy $0 1
        Exch $0                 # Stack: 1 (true)
        Goto end
     notEmpty:
       FindClose $0
       ClearErrors
       Pop $1                   # Stack: $0
       StrCpy $0 0
       Exch $0                  # Stack: 0 (false)
  end:
FunctionEnd

Section "Uninstall"
	!insertmacro UAC_RunElevated
	${Switch} $0
	${Case} 0
		${IfThen} $1 = 1 ${|} Quit ${|} 
		${IfThen} $3 <> 0 ${|} ${Break} ${|} 
	${Case} 1223
		Quit
	${Case} 1062
		Quit
	${Default}
		Quit
	${EndSwitch}  
  	ExecWait `$sysdir\regsvr32 /s /u "$INSTDIR\Content.dll"`  
  	ExecWait `$sysdir\regsvr32 /s /u "$INSTDIR\Content.x64.dll"`  
  	ExecWait `$sysdir\regsvr32 /s /u "$INSTDIR\CommonInterface.dll"`
  	ExecWait `$sysdir\regsvr32 /s /u "$INSTDIR\CommonInterface.x64.dll"`   
  	SetShellVarContext current 
  	RMDir /REBOOTOK /r "$INSTDIR\*"
  	RMDir /REBOOTOK "$INSTDIR"   
  	DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"         
SectionEnd