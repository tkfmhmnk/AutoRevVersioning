mkdir AutoRevVersioning_binary

copy Release\AutoRevVersioning.exe AutoRevVersioning_binary
copy Release\libUnicodeFileIO.dll AutoRevVersioning_binary

del AutoRevVersioning_binary.zip
powershell compress-archive AutoRevVersioning_binary AutoRevVersioning_binary.zip
pause
rmdir /s /q AutoRevVersioning_binary
pause