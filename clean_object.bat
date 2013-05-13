@echo off
@echo ************************************************
@echo ** Clean object bat.
@echo **                    by DeathMemory
@echo ** clean object program is running ...
@echo ************************************************
del MyDlg.sdf
del Debug\*.* /q
del Release\*.* /q
del *.aps /s
@echo y|rd ipch /s
set mainpath="MyDlg"
@echo y|rd %mainpath%\Debug /s
@echo y|rd %mainpath%\release /s
set mainpath="UIlib"
@echo y|rd %mainpath%\Debug /s
@echo y|rd %mainpath%\release /s
set mainpath="SharedMemory"
@echo y|rd %mainpath%\Debug /s
@echo y|rd %mainpath%\release /s
set mainpath="Config"
@echo y|rd %mainpath%\Debug /s
@echo y|rd %mainpath%\release /s
set mainpath="DMMon"
@echo y|rd %mainpath%\Debug /s
@echo y|rd %mainpath%\release /s
@echo ************************************************
@echo ** Finished !
@echo ************************************************
@pause