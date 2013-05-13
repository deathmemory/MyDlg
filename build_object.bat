@echo off
@set ObjectName="MyDlg.sln"
@set exeName="MyDlg.exe"

rem Get VS environment
call "%VS100COMNTOOLS%vsvars32.bat"
if errorlevel 1 goto flgErr

echo **************************************
echo ** Choose obj
echo ** [Debug]=1, [Release]=2
echo **************************************
set /p var=
if %var%==1 (set BuildObj="Debug|Win32")^
 else (set BuildObj="Release|Win32")
echo Current object is : %BuildObj% %var%
taskkill /f /im %exeName%
echo Start building ...

devenv %ObjectName% /build %BuildObj%
echo Finished !
goto flgEnd

:flgErr
@echo ********************************************
@echo **** Environment error !
@echo ********************************************
:flgEnd
@pause