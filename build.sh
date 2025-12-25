#!/bin/sh
BINARY_DIR="build"
TARGET_DIR="dist"
DEST_ARCHIVE="XBMC4Xbox.zip"

echo "Compiling XBMC..."
if [ -d "$BINARY_DIR" ]; then
    echo "Build directory already exists."
    printf "Do you want to skip configuring the build? [y/N]: "
    read answer
    case "$answer" in
        y|Y)
            echo "Skipping build directory setup."
            cd $BINARY_DIR || exit 1
            ;;
        *)
            echo "Reconfiguring build directory..."
            rm -rf $BINARY_DIR || exit 1
            mkdir $BINARY_DIR || exit 1
            cd $BINARY_DIR || exit 1
            cmake .. -DCMAKE_TOOLCHAIN_FILE=${NXDK_DIR}/share/toolchain-nxdk.cmake -DCMAKE_BUILD_TYPE=Release || exit 1
            ;;
    esac
else
    mkdir $BINARY_DIR || exit 1
    cd $BINARY_DIR || exit 1
    cmake .. -DCMAKE_TOOLCHAIN_FILE=${NXDK_DIR}/share/toolchain-nxdk.cmake -DCMAKE_BUILD_TYPE=Release
fi
cmake --build .
echo "XBMC is compiled."

echo "Cleaning previous build..."
cd ..
rm -rf "$TARGET_DIR"
rm -rf "$DEST_ARCHIVE"

echo "Packaging XBMC build..."
mkdir -p "$TARGET_DIR"
cp -f "$BINARY_DIR/default.xbe" "$TARGET_DIR/"

cat > exclude.txt <<EOF
.git
.svn
Thumbs.db
Desktop.ini
exclude.txt
skin.estuary
EOF

mkdir -p "$TARGET_DIR/home"
rsync -a --exclude-from=exclude.txt userdata/ "$TARGET_DIR/home/userdata/"
for folder in addons system media; do
  rsync -a --exclude-from=exclude.txt "$folder/" "$TARGET_DIR/$folder/"
done
rsync -a --exclude-from=exclude.txt "addons/skin.estuary/" "$TARGET_DIR/addons/skin.estuary/"

rm exclude.txt

echo "Compressing build..."
if ! command -v zip >/dev/null 2>&1; then
    echo "Zip not installed! Skipping compression..."
    exit 1
fi
(cd $TARGET_DIR && zip -r ../$DEST_ARCHIVE .)

echo "XBMC is ready!"
