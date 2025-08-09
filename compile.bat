@echo off

for %%a in (%*) do set "%%a=1"

if not "%release%"=="1" set debug=1
if not "%snake%"=="1" if not "%poe%"=="1" if not "%sam%"=="1" set all=1

if "%debug%"=="1" echo [debug mode]
if "%release%"=="1" echo [release mode]

set cl_common=/nologo /W3 /WX /GS- /DOS_WIN32 /DWM_WIN32 /DGPU_OPENGL
set cl_link=/link /nologo /fixed /incremental:no /opt:icf,ref libvcruntime.lib
set cl_out=/out:
set cl_debug=call cl /Od /Zi %cl_common%
set cl_release=call cl /O2 %cl_common%

if "%debug%"=="1" set compile=%cl_debug%
if "%release%"=="1" set compile=%cl_release%
set compile_link=%cl_link%
set out=%cl_out%

if not exist build mkdir build
pushd build
if "%snake%"=="1" %compile% ..\main_snake.c %compile_link% %out%snake.exe
if "%poe%"=="1" %compile% ..\main_poe_scratch.c %compile_link% %out%poe_scratch.exe
if "%sam%"=="1" %compile% ..\main_sam_scratch.c %compile_link% %out%sam_scratch.exe
popd
