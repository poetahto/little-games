@echo off

for %%a in (%*) do set "%%a=1"

if not "%release%"=="1" set "debug=1"
if not "%snake%"=="1" if not "%poe%"=="1" if not "%sam%"=="1" set "all=1"

if "%debug%"=="1" echo [debug mode]
if "%release%"=="1" echo [release mode]

set cl_compile_opt=/nologo /W3 /WX /GS- /DOS_WIN32 /DGPU_OPENGL
set cl_link_opt=/link /nologo /fixed /incremental:no /opt:icf,ref libvcruntime.lib
set cl_out=/out:
set cl_debug_cmd=call cl /Od /Zi %cl_compile_opt%
set cl_release_cmd=call cl /O2 %cl_compile_opt%

set clang_compile_opt=-Wall -DOS_WIN32 -DGPU_OPENGL
set clang_link_opt=
set clang_out=-o
set clang_debug_cmd=call clang -g -O0 %clang_compile_opt%
set clang_release_cmd=call clang -O2 %clang_compile_opt%

if "%debug%"=="1" set compile_cmd=%clang_debug_cmd%
if "%release%"=="1" set compile_cmd=%clang_release_cmd%
set link_opt=%clang_link_opt%
set out=%clang_out% build\

if not exist build mkdir build
if "%all%"=="1" set "poe=1" && set "sam=1" && set "snake=1"
if "%snake%"=="1" %compile_cmd% main_snake.c %link_opt% %out%snake.exe && echo compiling snake
if "%poe%"=="1" %compile_cmd% main_poe_scratch.c %link_opt% %out%poe_scratch.exe && echo compiling poe scratch
if "%sam%"=="1" %compile_cmd% main_sam_scratch.c %link_opt% %out%sam_scratch.exe && echo compiling sam scratch
