tag="v0-0.2.2"

if [ $# -gt 1 ];
then
	tag=$1
fi

for asset in "android" "headers-mingw" "linux" "macos" "windows"
do
    wget https://github.com/JerboaBurrow/Hop/releases/download/$tag/$asset.zip
    unzip -o $asset.zip -d common/
    rm $asset.zip
done

mv common/headers-mingw common/headers
mv common/headers/VulkanSDK common/windows/
