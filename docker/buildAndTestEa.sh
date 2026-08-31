#!/bin/sh
#
# parameters:
#  $1         (optional) The git commit hash or tag to checkout. If not set
#             master is used.

git clone https://github.com/aladur/ea.git
cd ea
if [ -n "$1" ]; then
    git checkout "$1"
fi
git submodule update --init --recursive
cmake -S . -B ../ea_build -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_INSTALL_SYSCONFDIR:PATH=/etc
cd ../ea_build
cmake --build . -j8
sudo cmake --install . --strip
ldd src/ea
test/unittests
ea /usr/share/doc/ea/examples/all_types.txt
