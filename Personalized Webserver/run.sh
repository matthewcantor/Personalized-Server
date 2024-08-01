read -r -d '' SLEEP_FUNC <<-'EOF'
    const sleep = (ms = 0) => new Promise(resolve => setTimeout(resolve, ms));
EOF

read -r -d '' NEW_FUNC_HEADER <<-'EOF'
    async function instantiateAsync(binary, binaryFile, imports, callback) { await sleep(500);
EOF

read -r -d '' NEW_EMCC << 'EOF'
EMCC_FLAGS = -s EXIT_RUNTIME=1 -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=655360000 -s USE_SDL=2 -s USE_SDL_GFX=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s ASSERTIONS=1 -O2 -g --preload-file assets 
EOF

set -e

cd game
sed -i "s/^EMCC_FLAGS.*$/${NEW_EMCC}/" Makefile
make bin/game.html
sed -i "s/^var Module /${SLEEP_FUNC} var Module /" bin/*.js
sed -i "s/.*function instantiateAsync.*/${NEW_FUNC_HEADER}/" bin/*.js
cd ..
make clean
make
