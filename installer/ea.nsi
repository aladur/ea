;--------------------------------
; ea.nsi
;--------------------------------
;
; This installer requires NSIS EnVar plug-in

!include LogicLib.nsh
!include FileFunc.nsh
!include SplitFirstStrPart.nsh
!include MUI2.nsh

!addplugindir /x86-ansi "Plugins\x86-ansi"
!addplugindir /x86-unicode "Plugins\x86-unicode"
!addplugindir /amd64-unicode "Plugins\amd64-unicode"

!define APPNAME    "EncodingAnalyzer"
; The variable APPVERSION have to be set as command line parameters:
; //DEA_VERSION=<version>
;!define APPVERSION "0.1.0"
; Refreshing Windows Defines
!define SHCNE_ASSOCCHANGED 0x8000000
!define SHCNF_IDLIST 0
!define ERROR_ALREADY_EXISTS 183
!define ARP "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
!define BASEDIR ".."

CRCCheck on
SetDateSave on
SetDatablockOptimize on
BGGradient 080820 5455FF 5455FF
SetCompressor /SOLID lzma
LicenseBkColor /windows

; file info
VIAddVersionKey ProductName     "${APPNAME}"
VIAddVersionKey LegalCopyright  "(C) 2026 W. Schwotzer"
VIAddVersionKey Comment         "A Character Encoding Analyzer"
VIAddVersionKey ProductVersion  "${APPVERSION}"
VIAddVersionKey FileDescription "Character Encoding Analyzer"
VIAddVersionKey FileVersion     "${APPVERSION}.0"
VIProductVersion "${APPVERSION}.0"

; The name of the installer
Name "${APPNAME} ${APPVERSION}"

; The installation file to create
OutFile "..\${APPNAME}-Setup-${APPVERSION}.exe"

; Have Windows Vista, 2008, 7, etc. trust us to not be a "legacy" installer
;RequestExecutionLevel admin

!macro VerifyUserIsAdmin
  UserInfo::GetAccountType
  Pop $0
  ${If} $0 != "admin" ;Require admin rights on NT4+
    MessageBox MB_ICONSTOP "Administrator rights required!"
    SetErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
    Quit
  ${EndIf}
!macroend

Function .onInit

  ; Check if installer is already running. If open a user dialog and abort installation.
  System::Call 'kernel32::CreateMutex(i 0, i 0, t "${APPNAME}_Mutex") ?e'
  Pop $R0
  StrCmp $R0 ${ERROR_ALREADY_EXISTS} 0 init.verifyadmin
  MessageBox MB_OK "${APPNAME} installer is already running."
  Abort
init.verifyadmin:
  ; Depending on admin status install application for all or current user.
  UserInfo::GetAccountType
  Pop $0
  ${If} $0 == "admin"
    SetShellVarContext all
  ${Else}
    SetShellVarContext current
  ${EndIf}

  StrCpy $INSTDIR $PROGRAMFILES64\${APPNAME}

  ; Estimate by Registry access if Application is already installed
  ; If so open a user dialog and evtl. uninstall it or abort.
  SetRegView Default
  ReadRegStr $R0 HKLM "${ARP}" "UninstallString"
  IfFileExists "$R0" init.askuninst init.done
init.askuninst:
  MessageBox MB_YESNO|MB_ICONQUESTION "${APPNAME} is already installed. Uninstall the existing version?" IDYES init.uninstall IDNO init.quit
init.quit:
  Quit
init.uninstall:
  ReadRegStr $R1 HKLM "${ARP}" "InstallLocation"
  ClearErrors
  ExecWait '"$R0" /S _?=$R1'
  IfErrors +1 init.done  
  MessageBox MB_OK "Error during uninstallation"
  Quit  
init.done:

FunctionEnd

;-------------------------------
; Pages
!define MUI_COMPONENTSPAGE_SMALLDESC  
!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
  
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; The install sections
Section "Binary Files" BinaryFiles

  SectionIn RO  
  SetOutPath $INSTDIR ; Set output path to the installation directory.
  ; Add files to be extracted to the current $OUTDIR path
  File /a "${BASEDIR}\install\bin\ea.exe"
  File /a "${BASEDIR}\install\bin\icudt78.dll"
  File /a "${BASEDIR}\install\bin\icuuc78.dll"
  File /a /oname=LICENSE.txt "${BASEDIR}\LICENSE"

SectionEnd

Section "Example Files" ExampleFiles

  SectionIn RO
  SetOutPath $INSTDIR\Examples
  File /a "${BASEDIR}\install\share\doc\ea\examples\all_types.txt"
  File /a "${BASEDIR}\install\share\doc\ea\examples\example_utf8.txt"
  File /a "${BASEDIR}\install\share\doc\ea\examples\example_windows1252.txt"

SectionEnd

Section "Start Menu Shortcuts" StartMenu

  SectionIn RO
  SetOutPath $INSTDIR ; for working directory
  CreateDirectory "$SMPROGRAMS\${APPNAME}"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall ${APPNAME}.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0 "" "" "Uninstall ${APPNAME} Installation"  

SectionEnd

Section "Microsoft Visual C++ Redistributables" VC_Redist

  SectionIn RO
  SetOutPath $TEMP
  ReadRegDword $R1 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Installed"
  ; Only istall VC_Redist if it is not already installed.
  ; Do not check for version, only if it is already installed.
  ; 14.0 supports VS2015 up to incl. VS2026.
  ${If} $R1 != "1"
    File ..\vc_redist.x64.exe  
    ExecWait '"$TEMP\vc_redist.x64.exe" /install /passive /norestart' 
    Delete $TEMP\vc_redist.x64.exe     
  ${EndIf}

SectionEnd   

Section "Update PATH environment variable" UpdatePath

  EnVar::SetHKLM
  EnVar::AddValue "PATH" "$INSTDIR"

SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${BinaryFiles} "Install Binary files."
  !insertmacro MUI_DESCRIPTION_TEXT ${ExampleFiles} "Install Example Files."
  !insertmacro MUI_DESCRIPTION_TEXT ${StartMenu} "Create Start Menu."
  !insertmacro MUI_DESCRIPTION_TEXT ${VC_Redist} "Install Microsoft Visual C++ Redistributable package."
  !insertmacro MUI_DESCRIPTION_TEXT ${UpdatePath} "Add ${APPNAME} installation path to PATH environment variable to use ea from command prompt."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section "-Registry update"

  Var /GLOBAL EstimatedSize
  Var /GLOBAL VersionMajor
  Var /GLOBAL VersionMinor
  Var /GLOBAL DisplayName

  ; Write the uninstall keys for Windows
  ; Calculate estimate size of installation directory
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  StrCpy $EstimatedSize "$0"
  ; Extract major and minor version
  Push "."
  Push "${APPVERSION}"
  Call SplitFirstStrPart
  Pop $VersionMajor
  Pop $VersionMinor
  StrCpy $DisplayName "${APPNAME} ${APPVERSION}"
  
  WriteRegStr   HKLM "${ARP}" "DisplayName"     "$DisplayName"
  WriteRegStr   HKLM "${ARP}" "DisplayVersion"  "${APPVERSION}"
  WriteRegStr   HKLM "${ARP}" "DisplayIcon"     "$INSTDIR\ea.exe"
  WriteRegStr   HKLM "${ARP}" "HelpLink"        "https://github.com/aladur/ea"
  WriteRegStr   HKLM "${ARP}" "InstallLocation" "$INSTDIR"
  WriteRegStr   HKLM "${ARP}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${ARP}" "Publisher"       "Wolfgang Schwotzer"
  WriteRegDWORD HKLM "${ARP}" "EstimatedSize"   "$EstimatedSize"
  WriteRegDWORD HKLM "${ARP}" "VersionMajor"    "$VersionMajor"
  WriteRegDWORD HKLM "${ARP}" "VersionMinor"    "$VersionMinor"
  WriteRegDWORD HKLM "${ARP}" "NoModify"        1
  WriteRegDWORD HKLM "${ARP}" "NoRepair"        1
  WriteUninstaller "uninstall.exe"

  ; Add Application paths
  WriteRegStr   HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\${APPNAME}.exe" "" "$INSTDIR\${APPNAME}.exe"

  System::Call 'Shell32::SHChangeNotify(i ${SHCNE_ASSOCCHANGED}, i ${SHCNF_IDLIST}, p0, p0)'

SectionEnd

;--------------------------------
; Uninstaller

Function un.onInit

  ; Depending on admin status uninstall application for all or current user.
  UserInfo::GetAccountType
  Pop $0
  ${If} $0 == "admin"
    SetShellVarContext all
  ${Else}
    SetShellVarContext current
  ${EndIf}
  SetRegView Default
  ReadRegStr $R0 HKLM "${ARP}" "InstallLocation"
  IfErrors uninit.dlgabort uninit.done
uninit.dlgabort:
    MessageBox MB_OK "Installation is inconsistent. Aborting Uninstallation."
    Abort ; Immediately Quit uninstallation
uninit.done:
  StrCpy $INSTDIR $R0

FunctionEnd

Section "Uninstall" Uninstall
  
  ; Remove registry keys
  DeleteRegKey HKLM "${ARP}"
  DeleteRegKey HKLM SOFTWARE\${APPNAME}
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\${APPNAME}.exe"

  ; Remove files and uninstaller
  Delete $INSTDIR\Examples\*.*
  Delete $INSTDIR\*.*

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${APPNAME}\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\${APPNAME}"
  RMDir "$INSTDIR\Examples"
  RMDir "$INSTDIR"

  ; Remove install directory from PATH environment variable
  EnVar::SetHKLM
  EnVar::DeleteValue "PATH" "$INSTDIR"

  System::Call 'Shell32::SHChangeNotify(i ${SHCNE_ASSOCCHANGED}, i ${SHCNF_IDLIST}, p0, p0)'

SectionEnd

