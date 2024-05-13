tag="v0-0.2.6"

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

if [ -d common/headers ];
then 
    rm -rf common/headers
fi

mv common/headers-mingw common/headers

if [ -d common/windows/VulkanSDK ];
then 
    rm -rf common/windows/VulkanSDK
fi

mv common/headers/VulkanSDK common/windows/
