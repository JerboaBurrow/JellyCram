tag="v0-0.2.0"

for asset in "android" "header-mingw" "headers" "linux" "macos" "windows"
do
    wget https://github.com/JerboaBurrow/Hop/releases/download/v0-0.2.0/$asset.zip
    unzip $asset.zip -d common/
done