export PRODUCT=0
export MAJOR=0
export MINOR=0
export PATCH=1
sed -i "s/VERSION_NAME/$PRODUCT-$MAJOR.$MINOR.$PATCH/g" android/app/build.gradle