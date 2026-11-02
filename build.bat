@echo off
title Inventory Management System - Build & Run
echo =======================================================
echo    INVENTORY MANAGEMENT SYSTEM (IMS) - C++ OOP
echo    3rd Semester Object-Oriented Programming Project
echo =======================================================
echo.

where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo [!] WARNING: g++ (MinGW/GCC) was not found in your system PATH.
    echo.
    echo If you are using Dev-C++ or Code::Blocks:
    echo   1. Open Dev-C++ or Code::Blocks.
    echo   2. Open the file: single_file\ims_oop_unified.cpp
    echo   3. Press F11 (or Execute -^> Compile ^& Run).
    echo.
    echo If you want to compile from command line:
    echo   Install MinGW-w64 and add its 'bin' directory to your PATH.
    echo.
    pause
    exit /b 1
)

echo [*] Compiling modular project (include/ + src/)...
g++ -std=c++11 -Iinclude src\*.cpp -o inventory_oop.exe

if %errorlevel% equ 0 (
    echo [OK] Compilation successful!
    echo [*] Starting Inventory Management System...
    echo.
    inventory_oop.exe
) else (
    echo.
    echo [!] Compilation failed with modular sources.
    echo [*] Attempting fallback compilation with single_file/ims_oop_unified.cpp...
    g++ -std=c++11 single_file\ims_oop_unified.cpp -o inventory_oop.exe
    if %errorlevel% equ 0 (
        echo [OK] Fallback compilation successful!
        echo [*] Starting Inventory Management System...
        echo.
        inventory_oop.exe
    ) else (
        echo [!] Compilation failed. Please check compiler errors above.
    )
)

pause
