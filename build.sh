#!/bin/sh
echo "Compiling XBMC..."

eval "$(${NXDK_DIR}/bin/activate -s)"
make

echo "XBMC is compiled."

# Package XBMC
mkdir -p BUILD

set -e

BINARY_DIR="RELEASE"
TARGET_DIR="BUILD"

# Remove and recreate target directory
rm -rf "$TARGET_DIR"
mkdir -p "$TARGET_DIR"
rm -rf XBMC.zip

echo "Copying files to $TARGET_DIR..."

# Copy binary
cp -f "$BINARY_DIR/default.xbe" "$TARGET_DIR/"

# Create exclude.txt
cat > exclude.txt <<EOF
.git
.svn
Thumbs.db
Desktop.ini
exclude.txt
skin.estuary
EOF

# Copy userdata
mkdir -p "$TARGET_DIR/home"
rsync -a --exclude-from=exclude.txt userdata/ "$TARGET_DIR/home/userdata/"

# Copy folders
for folder in addons system media; do
  rsync -a --exclude-from=exclude.txt "$folder/" "$TARGET_DIR/$folder/"
done

# Copy skin
rsync -a --exclude-from=exclude.txt "addons/skin.estuary/" "$TARGET_DIR/addons/skin.estuary/"

# Clean up
rm exclude.txt

echo "XBMC is ready."

echo "Compressing build to XBMC.zip file..."
if ! command -v zip >/dev/null 2>&1; then
    echo "Zip not installed! Skipping compression..."
    exit 1
fi

(cd BUILD && zip -r ../XBMC.zip .)

echo "Compressing XBMC is completed."
