@echo off
echo Generating self-signed TLS certificate for Healix Vault...
echo.

REM Try OpenSSL (bundled with Git for Windows, MSYS2, or standalone)
set PATH=%PATH%;C:\Program Files\Git\usr\bin
where openssl >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 3650 -nodes ^
        -subj "/C=IN/ST=Local/L=Local/O=HealixVault/CN=localhost" ^
        -addext "subjectAltName=IP:127.0.0.1,DNS:localhost"
    echo.
    echo Done! cert.pem and key.pem created.
    echo Run: .\build\password_vault.exe
    echo Open: http://localhost:8080
    echo.
    echo NOTE: To enable HTTPS later, you'll need OpenSSL dev headers installed.
    echo       Click Advanced - Proceed to localhost ^(unsafe^)
) else (
    echo OpenSSL not found in PATH.
    echo Install via one of:
    echo   1. Git for Windows  ^(https://git-scm.com/^) - comes with openssl
    echo   2. MSYS2: pacman -S mingw-w64-x86_64-openssl
    echo   3. Chocolatey: choco install openssl
    echo   4. Winget: winget install openssl
    echo.
    echo After install, re-run this script.
)
pause
