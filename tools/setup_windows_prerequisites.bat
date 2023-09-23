:<<"::WINDOWS_ONLY"
@echo off
:: Copyright 2023 The Pigweed Authors
::
:: Licensed under the Apache License, Version 2.0 (the "License"); you may not
:: use this file except in compliance with the License. You may obtain a copy of
:: the License at
::
::     https://www.apache.org/licenses/LICENSE-2.0
::
:: Unless required by applicable law or agreed to in writing, software
:: distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
:: WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
:: License for the specific language governing permissions and limitations under
:: the License.
::WINDOWS_ONLY
:; echo "ERROR: Attempting to run Windows .bat from a Unix/POSIX shell!"
:; echo "Instead, run the following command."
:; echo ""
:; echo "    source ./bootstrap.sh"
:; echo ""
:<<"::WINDOWS_ONLY"

:: Utility for bootstrapping Windows dependencies.
::
:: This tool does the following:
::   - Ensures `winget` is available and ready to use.
::   - Ensures developer mode is enabled so symlinks work.
::   - Ensures git is set up and ready to go.
::   - Ensures a working version of Python is installed.
::   - Ensures git is configured to allow symlinks.
::
:: Usage:
::   setup_windows_prerequisites.bat [mode]
::
::   [mode]: Either "check" or "setup" (defaults to "setup" if not specified).
::           If set to "check", the script will terminate when it finds
::           something to be missing or misconfigured, and emit a helpful
::           error message rather than trying to automatically fix the issue.

:: WARNING: Multi-line "if" statements can be dangerous!
::
:: Example:
::  call do_foo
::  if [expression] (
::    call cmd_a
::    set my_var = %ERRORLEVEL%
::    call final_script --flag %my_var%
::  )
:: Batch evaluates these expressions in a way that will produce unexpected
:: behavior. It appears that when each line is executed, it does not affect
:: local context until the entire expression is complete. In this example,
:: ERRORLEVEL does not reflect `call cmd_a`, but whatever residual state was
:: present from `do_foo`. Similarly, in the call to `final_script`, `my_var`
:: will NOT be valid as the variable `set` doesn't apply until the entire `if`
:: expression completes.
:: This script only uses multi-line if statements to `goto` after an operation.

SETLOCAL

SET _COLORED_TEXT_OK=[92mOK![0m
SET _COLORED_TEXT_MISSING=[93mMISSING![0m
SET _COLORED_TEXT_FAIL=[91mFAIL![0m

SET _COLORED_TEXT_INSTALL_SUCCEEDED=[92mInstall succeeded![0m
SET _COLORED_TEXT_INSTALL_FAILED=[91mInstall failed![0m

SET _COLORED_TEXT_DONE="[92mDONE![0m "
SET _COLORED_TEXT_NOTE="[93mNOTE:[0m "
SET _COLORED_TEXT_ERROR="[91mERROR:[0m "

SET "_DEV_MODE_PATH=Settings->Update & Security->For developers->Developer Mode"

reg query HKLM\Software\Policies\Microsoft\Windows\Appx /v BlockNonAdminUserInstall | find "0x1" >NUL 2>&1
SET /a "_BLOCK_NON_ADMIN_USER_INSTALL_ENABLED=%ERRORLEVEL%" >NUL 2>&1

:ensure_minimum_requirements
SET mode=INVALID
IF "%1"=="" set mode=SETUP
IF "%1"=="check" set mode=CHECK
IF "%1"=="setup" set mode=SETUP

CALL :ensure_winget_permissions %mode%
IF %ERRORLEVEL% NEQ 0 GOTO abort

CALL :ensure_winget %mode%
IF %ERRORLEVEL% NEQ 0 GOTO abort

CALL :ensure_symlinks  %mode%
IF %ERRORLEVEL% NEQ 0 GOTO abort

CALL :ensure_git %mode%
IF %ERRORLEVEL% NEQ 0 GOTO abort

CALL :ensure_python %mode%
IF %ERRORLEVEL% NEQ 0 GOTO abort

CALL :ensure_git_symlinks %mode%
IF %ERRORLEVEL% NEQ 0 GOTO abort

CALL :ensure_long_filenames %mode%
IF %ERRORLEVEL% NEQ 0 GOTO abort

echo | SET /p=%_COLORED_TEXT_DONE%

:: Workaround until winget properly updates PATH after installations.
::   https://github.com/microsoft/winget-cli/issues/222
:: Note: This "start" doesn't work, it just looks nice for now. You actually
:: need to restart the command prompt by hand.
SET "local_path=%PATH%"
IF "%mode%"=="SETUP" (
	echo Everything is ready to go!
	echo.
	echo %_COLORED_TEXT_NOTE% We advise opening a fresh shell before running bootstrap.bat!
	endlocal & set "PATH=%local_path%"
)

IF "%mode%"=="CHECK" (
	echo Everything is ready to go!
)

EXIT /B 0

:abort
:: TODO: add flag to restore previous permissions after.
rem CALL :restore_winget_permissions %mode%, %_BLOCK_NON_ADMIN_USER_INSTALL_ENABLED%
echo Failed to bootstrap development environment. Exiting...
EXIT /B %ERRORLEVEL%

:: Checks if winget has been bootstrapped, setting it up if not.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP"
::
:: Returns:
::   0 if winget is available and ready to use.
:ensure_winget
:: First call is to see if we need winget.
CALL :check_winget %~1
IF %ERRORLEVEL% EQU 0 EXIT /B 0

:: If CHECK mode, return here.
IF "%~1"=="CHECK" EXIT /B %ERRORLEVEL%

:: winget check failed, try to bootstrap.
echo %_COLORED_TEXT_MISSING%
CALL :bootstrap_winget
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

EXIT /B %ERRORLEVEL%

:: Checks if winget is present.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP", controls soft vs hard failures.
::
:: Returns:
::   0 if a functional system Python is available.
:check_winget
echo | SET /p="Checking for winget..........."
where winget >NUL 2>&1
IF "%~1"=="CHECK" CALL :pretty_print_failure %ERRORLEVEL%, "winget does not exist."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

winget --version >NUL 2>&1
IF "%~1"=="CHECK" CALL :pretty_print_failure %ERRORLEVEL%, "winget isn't working as expected, check your internet connection or firewall settings."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

echo %_COLORED_TEXT_OK%
EXIT /B 0

:: Try to bootstrap winget.
::
:: Arguments:
::   None.
::
:: Returns:
::   0 if winget is properly set up.
:bootstrap_winget
echo | SET /p="Bootstrapping winget.........."
powershell -command "Add-AppxPackage -RegisterByFamilyName -MainPackage Microsoft.DesktopAppInstaller_8wekyb3d8bbwe" >NUL 2>&1
CALL :pretty_print_failure %ERRORLEVEL%, "Failed to bootstrap winget."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

CALL :run_as_admin "winget", "source reset --force"
CALL :pretty_print_failure %ERRORLEVEL%, "Failed to refresh winget sources."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

echo %_COLORED_TEXT_OK%
EXIT /B 0

:: Checks if winget sufficient permissions, and tries to enable them if not.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP"
::
:: Returns:
::   0 if winget is available and ready to use.
:ensure_winget_permissions
:: First call is to see if we need winget.
CALL :check_winget_permissions %~1
IF %ERRORLEVEL% EQU 0 EXIT /B 0

:: If CHECK mode, return here.
IF "%~1"=="CHECK" EXIT /B %ERRORLEVEL%

:: winget check failed, try to bootstrap.
echo %_COLORED_TEXT_MISSING%
CALL :enable_winget_permissions
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

EXIT /B %ERRORLEVEL%

:: Checks if winget installs have permission to succeed.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP", controls soft vs hard failures.
::
:: Returns:
::   0 if sufficient permissions.
:check_winget_permissions
echo | SET /p="Checking winget install permissions.........."
:: This entry must NOT be set.
reg query HKLM\Software\Policies\Microsoft\Windows\Appx /v BlockNonAdminUserInstall | find "0x1" >NUL 2>&1
SET /a "retval=!%ERRORLEVEL%" >NUL 2>&1
IF "%~1"=="CHECK" CALL :pretty_print_failure %retval%, "The registry value `BlockNonAdminUserInstall` at HKLM\Software\Policies\Microsoft\Windows\Appx is set to 1, which will prevent winget installs."
IF %retval% NEQ 0 EXIT /B %retval%

echo %_COLORED_TEXT_OK%
EXIT /B 0

:: Enables permissions required to do winget installs.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP", controls soft vs hard failures.
::
:: Returns:
::   0 if succesful.
:enable_winget_permissions
echo | SET /p="Enabling winget install permissions.........."

CALL :run_as_admin "reg", "add HKLM\Software\Policies\Microsoft\Windows\Appx /t REG_DWORD /v BlockNonAdminUserInstall /d 0 /f"
CALL :pretty_print_failure %ERRORLEVEL%, "Failed to add registry entry"
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

echo %_COLORED_TEXT_OK%
EXIT /B 0

:: Restores previous state of winget install permissions.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP", controls soft vs hard failures.
::   %2: 1 if permissions should be reenabled.
::
:: Returns:
::   0 if succesful.
:restore_winget_permissions
IF %2 EQU 0 EXIT /B 0
IF "%~1"=="CHECK" EXIT /B 0

echo | SET /p="Restoring winget install permissions.........."
powershell -command "Start -Verb RunAs -Wait -File "reg" -ArgumentList \"add HKLM\Software\Policies\Microsoft\Windows\Appx /t REG_DWORD /v BlockNonAdminUserInstall /d 1 /f\"" >NUL 2>&1
CALL :pretty_print_failure %ERRORLEVEL%, "Failed to restore registry entry"
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

echo %_COLORED_TEXT_OK%
EXIT /B 0


:: Checks if symlinks work, prompting the user to enable developer mode if not.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP"
::
:: Returns:
::   0 if symlinks are properly set up.
:ensure_symlinks

:: First call is to see if we need to enable symlinks.
CALL :check_symlinks %~1
IF %ERRORLEVEL% EQU 0 EXIT /B 0

:: If CHECK mode, return here.
IF "%~1"=="CHECK" EXIT /B %ERRORLEVEL%

:: symlink check failed, try to enable.
echo %_COLORED_TEXT_MISSING%
CALL :enable_developer_mode
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

EXIT /B %ERRORLEVEL%

:: Checks if symlinks work.
::
:: Arguments:
::   None
::
:: Returns:
::   0 if symlinks are properly set up.
:check_symlinks
echo | SET /p="Checking if symlinks are enabled..."
echo Test file contents >test_file.txt 2>&1
mklink test_file_link.txt test_file.txt >NUL 2>&1
set retval=%ERRORLEVEL%
del test_file.txt
IF "%~1"=="CHECK" CALL :pretty_print_failure %retval%, "Symlinks are not enabled, please enable Developer Mode in %_DEV_MODE_PATH%"
IF %retval% NEQ 0 EXIT /B %retval%
:: Symlink was created, delete it.
del test_file_link.txt

echo %_COLORED_TEXT_OK%
EXIT /B 0

:: Prompt the user to enable developer mode.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP"
::
:: Returns:
::   0 if symlinks are properly set up.
:enable_developer_mode
echo | SET /p="Enabling symlinks.........."

powershell -command "Start -Verb RunAs -Wait -File "reg" -ArgumentList \"add HKLM\Software\Microsoft\Windows\CurrentVersion\AppModelUnlock /t REG_DWORD /v AllowDevelopmentWithoutDevLicense /d 1 /f\"" >NUL 2>&1
CALL :pretty_print_failure %ERRORLEVEL%, "Failed to add registry entry"
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

echo %_COLORED_TEXT_OK%

echo Waiting for change to apply.
:: TODO: This probably was caused by a race in the powershell command above and
:: has probably been fixed since `-Wait` was added.
timeout /NOBREAK 5

:: Check again to make sure symlinks now work.
CALL :check_symlinks CHECK
EXIT /B %ERRORLEVEL%

:: Checks if git is present, installing one if it is not.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP"
::
:: Returns:
::   0 if a functional system Python is available.
:ensure_git
:: First call is to see if we need python.
CALL :check_git %~1
IF %ERRORLEVEL% EQU 0 EXIT /B 0

:: If CHECK mode, return here.
IF "%~1"=="CHECK" EXIT /B %ERRORLEVEL%

:: Git check failed, try to install.
echo %_COLORED_TEXT_MISSING%
CALL :get_package "git", Git.Git
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

:: Oddly, github is never added to the PATH after installing using wingit.
::   https://github.com/microsoft/winget-cli/issues/2815
SET "PATH=%ProgramFiles%\Git\cmd;%PATH%"

EXIT /B %ERRORLEVEL%

:: Checks if a git is present.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP", controls soft vs hard failures.
::
:: Returns:
::   0 if a functional system Python is available.
:check_git
echo | SET /p="Checking for git.........."
where git >NUL 2>&1
IF "%~1"=="CHECK" CALL :pretty_print_failure %ERRORLEVEL%, "git does not exist."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

git --version >NUL 2>&1
IF "%~1"=="CHECK" CALL :pretty_print_failure %ERRORLEVEL%, "git isn't working as expected."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

echo %_COLORED_TEXT_OK%
EXIT /B 0

:: Checks if a functional system Python is present, installing one if it is not.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP"
::
:: Returns:
::   0 if a functional system Python is available.
:ensure_python
:: First call is to see if we need python.
CALL :check_python %~1
IF %ERRORLEVEL% EQU 0 EXIT /B 0

:: If CHECK mode, return here.
IF "%~1"=="CHECK" EXIT /B %ERRORLEVEL%

:: Python check failed, try to install.
echo %_COLORED_TEXT_MISSING%
CALL :get_package "Python", Python.Python.3.8
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

SET "PATH=%LOCALAPPDATA%\Programs\Python\Python38;%PATH%"

EXIT /B %ERRORLEVEL%

:: Checks if a functional system Python is present.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP", controls soft vs hard failures.
::
:: Returns:
::   0 if a functional system Python is available.
:check_python
echo | SET /p="Checking for Python.........."
where python >NUL 2>&1
IF "%~1"=="CHECK" CALL :pretty_print_failure %ERRORLEVEL%, "System python does not exist."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

python -c 'print("Hello, World!")' >NUL 2>&1
IF "%~1"=="CHECK" CALL :pretty_print_failure %ERRORLEVEL%, "System python is a Windows Store link."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

echo %_COLORED_TEXT_OK%
EXIT /B 0

:: Checks if git symlinks are enabled, and tries to enable them.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP"
::
:: Returns:
::   0 if a git symlinks are enabled.
:ensure_git_symlinks
:: First call is to see if we need to enable symlinks.
CALL :check_git_symlinks %~1
IF %ERRORLEVEL% EQU 0 EXIT /B 0

:: If CHECK mode, return here.
IF "%~1"=="CHECK" EXIT /B %ERRORLEVEL%

:: git symlink check failed, try to set.
echo %_COLORED_TEXT_MISSING%
CALL :enable_git_symlinks
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

EXIT /B %ERRORLEVEL%

:: Checks if git symlinks are enabled.
::
:: Arguments:
::   %~1: "CHECK" or "SETUP", controls soft vs hard failures.
::
:: Returns:
::   0 if git symlinks are enabled.
:check_git_symlinks
echo | SET /p="Checking git symlinks.........."
git --version >NUL 2>&1
:: Unconditionally fail if we fail here.
CALL :pretty_print_failure %ERRORLEVEL%, "git not found, if you just installed it try re-running this script."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

CALL :check_git_setting "core.symlinks", "true"
IF "%~1"=="CHECK" CALL :pretty_print_failure %ERRORLEVEL%, "Git's core.symlinks is not enabled."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

echo %_COLORED_TEXT_OK%
EXIT /B 0

:: Enable git symlinks.
::
:: Arguments:
::   None.
::
:: Returns:
::   0 if symlinks are properly enabled.
:enable_git_symlinks
echo | SET /p="Enabling git symlinks.........."
git config --global core.symlinks true
CALL :pretty_print_failure %ERRORLEVEL%, "Failed to enable git symlinks."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

echo %_COLORED_TEXT_OK%
EXIT /B 0

:: Checks if a git setting is set appropriately.
::
:: Arguments:
::   %~1: Setting name
::   %~2: Desired setting value
::
:: Returns:
::   0 if the setting has the intended valu.
:check_git_setting
git config --get %~1 > current_git_setting.txt
:: Unconditionally fail if we fail here.
set retval=%ERRORLEVEL%
IF %ERRORLEVEL% NEQ 0 GOTO check_git_setting_cleanup

echo %~2 > desired_git_setting.txt
set retval=%ERRORLEVEL%
IF %ERRORLEVEL% NEQ 0 GOTO check_git_setting_cleanup

:: There will be line ending differences, so enable /w.
fc /w current_git_setting.txt desired_git_setting.txt >NUL 2>&1
set retval=%ERRORLEVEL%

:check_git_setting_cleanup
del current_git_setting.txt >NUL 2>&1
del desired_git_setting.txt >NUL 2>&1

EXIT /B %retval%

:ensure_long_filenames
echo | SET /p="Checking if long paths are enabled.........."
reg query HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\FileSystem /v LongPathsEnabled | find "0x1" >NUL 2>&1
SET /a "retval=%ERRORLEVEL%" >NUL 2>&1

:: Print error and exit early if we're in check mode.
IF "%~1"=="CHECK" CALL :pretty_print_failure %retval%, "The registry value `LongPathsEnabled` at  HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\FileSystem is set to 0, which will cause failures in deeply nested directories."
IF "%~1"=="CHECK" CALL :pretty_print_ok %retval%
IF "%~1"=="CHECK" EXIT /B %retval%

:: Otherwise, enable long paths as admin.
IF %retval% NEQ 0 CALL :run_as_admin "reg", "add HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\FileSystem /v LongPathsEnabled  /t REG_DWORD /d 1 /f"
CALL :pretty_print_failure %ERRORLEVEL%, "Failed to enable long paths."
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%


echo %_COLORED_TEXT_OK%
EXIT /B 0

:: Checks if a functional system Python is present, installing one if it is not.
::
:: Arguments:
::   %~1: Output text for step
::   %~2: Name of winget package
::
:: Returns:
::   0 if a functional system Python is available.
:get_package
echo Installing %~1............
powershell -command "Start -Verb RunAs -Wait -File winget -ArgumentList \"install -e --id %~2 --source winget\""
IF %ERRORLEVEL% EQU 0 (
	echo %_COLORED_TEXT_INSTALL_SUCCEEDED%
	EXIT /B 0
)

set retval=%ERRORLEVEL%
echo %_COLORED_TEXT_INSTALL_FAILED%
EXIT /B %retval%


:: Runs a command as administrator.
::
:: Arguments:
::   %~1: Command name (string)
::   %~2: Command arguments (string)
::
:: Returns:
::   0 command succeeded.
:run_as_admin
powershell -command "$proc=(Start -Verb RunAs -Wait -PassThru -File %~1 -ArgumentList \"%~2\"); exit $proc.ExitCode"
EXIT /B %ERRORLEVEL%

:: Prints FAIL! and then an error.
::
:: Arguments:
::   %~1: Return value of the error, so it can propagate.
::   %~2: Error message.
::
:: Returns:
::   %~1: Original error code.
:pretty_print_failure
IF %~1 NEQ 0 (
	echo %_COLORED_TEXT_FAIL%
	echo | SET /p=%_COLORED_TEXT_ERROR%
	echo | SET /p "=%~2"
	echo:
)
EXIT /B %~1

:pretty_print_error
IF %~1 NEQ 0 (
	echo | SET /p=%_COLORED_TEXT_ERROR%
	echo | SET /p "=%~2"
	echo:
)
EXIT /B %~1

:pretty_print_ok
IF %~1 EQU 0 echo %_COLORED_TEXT_OK%
EXIT /B %~1
