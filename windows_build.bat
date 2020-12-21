mkdir windows_build
cd windows_build
cmake -G "Visual Studio 14 Win64" ..
msbuild btcud.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild btcu-cli.vcxproj /p:Configuration=Release /p:Platform=x64
