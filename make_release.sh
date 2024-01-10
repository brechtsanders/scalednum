#!/bin/sh

# determine version number
SCALEDNUM_VERSION_MAJOR=$(sed -ne "s/^#define\s*SCALEDNUM_VERSION_MAJOR\s*\(.*\)/\1/p" include/scalednum.h)
SCALEDNUM_VERSION_MINOR=$(sed -ne "s/^#define\s*SCALEDNUM_VERSION_MINOR\s*\(.*\)/\1/p" include/scalednum.h)
SCALEDNUM_VERSION_MICRO=$(sed -ne "s/^#define\s*SCALEDNUM_VERSION_MICRO\s*\(.*\)/\1/p" include/scalednum.h)

# determine target platform
SCALEDNUM_PLATFORM_TRIPLET=$(${CC:-gcc} -v 2>&1|sed -ne "s/^Target: \(.*\)$/\1/p")
SCALEDNUM_ARCH=$(echo $SCALEDNUM_PLATFORM_TRIPLET|sed -e "s/^\([^-]*\)-.*$/\1/")
SCALEDNUM_PLATFORM=$(echo $SCALEDNUM_PLATFORM_TRIPLET|sed -e "s/^.*-\([^-]*\)$/\1/")
if echo $SCALEDNUM_PLATFORM|grep -q "^\(mingw\|windows\)"; then
 SCALEDNUM_PLATFORM=windows
fi

# build source package
tar cfJ scalednum-$SCALEDNUM_VERSION_MAJOR.$SCALEDNUM_VERSION_MINOR.$SCALEDNUM_VERSION_MICRO.tar.xz doc/* include/* src/* CMakeLists.txt LICENSE README.md Changelog.txt &&

# build binary packages
BUILDDIR=$(realpath $(dirname $0))/build.$SCALEDNUM_ARCH.$SCALEDNUM_PLATFORM
cmake -Wno-dev -GNinja -DCMAKE_INSTALL_PREFIX:PATH=$BUILDDIR/install -DCMAKE_BUILD_TYPE:STRING=Release -DBUILD_SHARED:BOOL=ON -DBUILD_STATIC:BOOL=ON -DBUILD_DEMO:BOOL=ON -S. -B$BUILDDIR/build &&
ninja -C$BUILDDIR/build install/strip &&
pushd $BUILDDIR/install &&
tar cfJ ../../scalednum-$SCALEDNUM_VERSION_MAJOR.$SCALEDNUM_VERSION_MINOR.$SCALEDNUM_VERSION_MICRO-$SCALEDNUM_PLATFORM-$SCALEDNUM_ARCH.tar.xz * &&
zip -r9 ../../scalednum-$SCALEDNUM_VERSION_MAJOR.$SCALEDNUM_VERSION_MINOR.$SCALEDNUM_VERSION_MICRO-$SCALEDNUM_PLATFORM-$SCALEDNUM_ARCH.zip * &&
popd &&
rm -rf $BUILDDIR &&
echo Done
