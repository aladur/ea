ea - Building from source
====

# Pre-requisites

## Linux

On **Debian** based Linux distributions like **Ubuntu** or **Linux Mint** install the following packages.

```sh
$ [sudo] apt-get install build-essential git cmake libboost-program-opions-dev libicu-dev
```

On **Red Hat** based Linux distributions like **Fedora** or **CentOS** install the following packages.

```sh
$ [sudo] dnf install git make cmake boost-devel libicu-devel
```

On **Arch Linux** based Linux distributions like **Manjaro** or **CachyOS** install the following packages.

```sh
$ [sudo] pacman -S git make cmake boost boost-libs icu
```

On **Alpine Linux** distribution install the following packages.

```sh
$ [sudo] apk add git cmake build-base boost-dev icu-dev
```


## Windows

On Windows the following applications have to be installed.

* Install Visual Studio 2022 or later (Visual C++ compiler).
* Install [Git](https://git-scm.com/).
* Install [CMake](https://cmake.org/download/).

[git for Windows](https://gitforwindows.org/) comes with a set of UNIX tools like `sha256sum`, `tar`, `unzip` or `curl` and the git bash. They are used in this project. Most installation steps will use git bash for execution.

# Build on Windows

For the users convenience a Windows installer can be downloaded from https://github.com/aladur/ea/releases.

The following chapters describe how to build `ea` including dependent libraries. The following dependent libraries are needed:

* [boost](https://github.com/boostorg/boost.git) (library `program_options` and `header only`)
* [icu](https://github.com/unicode-org/icu.git)


## Build boost

There are two possibilities to build boost:

* Using `git`
* Using `b2`

### Using `git`

The boost git repository is completely downloaded and only the needed libraries are build.

For this project only boost library `program_options` and `header only` libraries are needed and build.

Execute in a git bash. The folder `boost-1.86.0_build` is just an example folder where boost cmake build is stored. The path `/d/projects/boost-1.86.0_install` is just an example path where boost build artifacts are installed. They can be adapted as needed.

```sh
git clone --recursive https://github.com/boostorg/boost.git
cd boost
git checkout boost-1.86.0
git submodule update --init --recursive
cd ..
cmake -S boost -B boost-1.86.0_build -G "Visual Studio 17 2022" -A x64 -DCMAKE_INSTALL_PREFIX=/d/projects/boost-1.86.0_install -DBOOST_INCLUDE_LIBRARIES=program_options -DBUILD_SHARED_LIBS=OFF
cmake --build boost-1.86.0_build --config Release --target install
cmake --build boost-1.86.0_build --config Debug --target install
```

### Using `b2`

b2 is downloaded from [b2 github releases](https://github.com/bfgroup/b2/releases/) as source tar ball. Execute in a git bash.

```sh
curl -L https://github.com/bfgroup/b2/releases/download/5.5.3/b2-5.5.3.bz2 -o b2-5.5.3.bz2
sha256sum b2-5.5.3.bz2
tar xf b2-5.5.3.bz2
```

Execute in an Windows command prompt. The path `C:\\utils` is just an example path where the b2 build artifacts are installed. It can be adapted as needed. This path has to be added to the `PATH` environment variable.

```sh
cd <directory>\\b2-5.5.3
bootstrap.bat
.\b2 --prefix=C:\\utils install
```

`b2` now will be found in the path.

boost is downloaded from [boost github repository](https://github.com/boostorg/boost/). Execute in a git bash. The path `/d/projects/boost-1.86.0_install` is just an example path where boost build artifacts are installed. It can be adapted as needed.

```sh
curl -L https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-b2-nodocs.tar.xz -o boost-1.86.0-b2-nodocs.tar.xz
tar xf boost-1.86.0-b2-nodocs.tar.xz
cd boost-1.86.0
b2 --with-program_options toolset=msvc-14.3 address-model=64 variant=debug,release link=static,shared threading=multi --prefix=/d/projects/boost-1.86.0_install install
cd ..
```

## Build `icu`

Icu is downloaded from [icu github releases](https://github.com/unicode-org/icu/releases/) as prebuild binary package. Execute in a git bash.

```sh
curl -L https://github.com/unicode-org/icu/releases/download/release-78.3/icu4c-78.3-Win64-MSVC2022.zip -o icu4c-78.3-Win64-MSVC2022.zip
mkdir icu4c-78.3_install
cd icu4c-78.3_install
unzip ../icu4c-78.3-Win64-MSVC2022.zip
cd ..
```

## Build `ea`

After all dependent libraries are available ea can be downloaded from [ea github repository](https://github.com/aladur/ea). Execute in a git bash.

```sh
git clone https://github.com/aladur/ea.git
cmake -S ea -B ea_build -DBoost_DIR=/d/projects/boost-1.86.0_install/lib/cmake/Boost-1.86.0 -DICU_ROOT=/d/projects/iicu4c-78.3_install
cmake --build ea_build --config Release
```

# Build on Linux

After all dependent packages are available `ea` can be downloaded from [ea github repository](https://github.com/aladur/ea).

## Build `ea`

To build a specific `ea` release version replace `RELEASE_TAG_HERE` by a release tag, vor example `v0.5.0`.

```sh
$ git clone -b RELEASE_TAG_HERE https://github.com/aladur/ea.git
$ cmake -S ea -B ea_build
$ cmake --build ea_build
$ [sudo] cmake --install ea_build
```
