cmake -B build/win64 -DCMAKE_TOOLCHAIN_FILE=./TC-windows.cmake
start "" "%~dp0build\win64\UbiTorContestAPI.sln"