@echo off

for %%a in (%*) do set "%%a=1"

if not "%release%"=="1" set debug=1
if not "%snake%"=="1" if not "%poe%"=="1" if not "%sam%"=="1" set all=1

if "%debug%"=="1" echo [debug mode]
if "%release%"=="1" echo [release mode]

set cl_compile_opt=/nologo /W3 /WX /GS- /DOS_WIN32 /DWM_WIN32 /DGPU_OPENGL
set cl_link_opt=/link /nologo /fixed /incremental:no /opt:icf,ref libvcruntime.lib
set cl_out=/out:
set cl_debug_cmd=call cl /Od /Zi %cl_compile_opt%
set cl_release_cmd=call cl /O2 %cl_compile_opt%

if "%debug%"=="1" set compile_cmd=%cl_debug_cmd%
if "%release%"=="1" set compile_cmd=%cl_release_cmd%
set link_opt=%cl_link_opt%
set out=%cl_out%

if not exist build mkdir build
pushd build
if "%snake%"=="1" %compile_cmd% ..\main_snake.c %link_opt% %out%snake.exe
if "%poe%"=="1" %compile_cmd% ..\main_poe_scratch.c %link_opt% %out%poe_scratch.exe
if "%sam%"=="1" %compile_cmd% ..\main_sam_scratch.c %link_opt% %out%sam_scratch.exe
popd
