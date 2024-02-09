tag="v0-0.2.0"

for asset in "android" "headers-mingw" "linux" "macos" "windows"
do
    wget https://github.com/JerboaBurrow/Hop/releases/download/v0-0.2.0/$asset.zip
    unzip -o $asset.zip -d common/
    rm $asset.zip
done

mv common/headers-mingw common/headers
mv common/headers/VulkanSDK common/windows/