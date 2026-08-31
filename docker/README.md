# Use docker to run and test `ea`

Docker provides a lightweight and resource efficient virtualization. It can be used to quickly run and test `ea` project on different linux distributions.

To use docker on linux it first has to be installed. The installation steps are [documented here](https://docs.docker.com/desktop/) under the topic "Install docker Desktop". It can be installed on Windows, MacOS or linux. This HowTo focusses on linux host. So the one or the other step may be different on a MacOS or Windows host.

## Build a docker image

The folder hierarchy represents the linux distribution. Each subfolder contains a Dockerfile which can be used to create a docker image for a specific linux distribution, sometimes with variants like building with gcc or clang. All Dockerfiles contain the same steps. Depending on the used docker base image the one or the other step is skipped because the necessary dependeny is already available in the used docker base image. These are the steps executed in the Dockerfile.

* Update and upgrade package manger
* Install development environment
* Install dependent libraries and development headers
* Create a build user (with default access rights and sudo capability)
* Switch to build user
* Download `ea` distribution from github
* Checkout a specific git commit or tag
* Initialize and update git submodules
* Configure `ea` project
* Build `ea` and execute unittests
* Install e`ea`a (with root access)

### Build with legacy build

Docker provides an easy to use command line tool to build a docker image. When using Docker version 23.0 or higher this feature is deprecated. Read next chapter instead.

        docker build --build-arg GIT_COMMIT=<commit> -f <distribution>/Dockerfile -t <name>:<tag> .

Explicitly specifying the path to the Dockerfile by `-f <distribution>/Dockerfile` allows to widen the context and for example use a reusable shell script in the current directory.

Please do not forget the dot at the end of `docker build` command. To avoid confusion about the linux distribution I introduced a naming convention for the docker image to use `<distribution>_ea` for `<name>` which is used within this HowTo. `<tag>` either can be `latest` or a git tag. Other naming conventions can make sense to maybe also take the version of the linux distribution into account, so this Howto simply is ment as a guide line.

There is a parameter `--build-arg GIT_COMMIT=<git_commit>` to specify a git commit hash or tag to checkout for the build.

For example to build `ea` based on a [debian docker base image](https://hub.docker.com/_/debian) using git tag `v0.1.0` can be built as follows.

        docker build --build-arg GIT_COMMIT=v0.1.0 -f debian/Dockerfile -t debian_ea:0.1.0 .

The build takes several minutes and after this the docker image can be referenced by it's name `debian_ea:0.1.0`.

### Build with buildx

Since Docker version 19.03 an extended build on the command line is available to build a docker image. The extended features are not needed here. The legacy build is deprecated since Docker version 23.0.

        docker buildx build --build-arg GIT_COMMIT=<commit> -f <distribution>/Dockerfile -t <name>:<tag> --load .

Explicitly specifying the path to the Dockerfile by `-f <distribution>/Dockerfile` allows to widen the context and for example use a reusable shell script in the current directory.

Please do not forget the dot at the end of `docker buildx` command. To avoid confusion about the linux distribution I introduced a naming convention for the docker image to use `<distribution>_ea` for `<name>` which is used within this HowTo. `<tag>` either can be `latest` or a git tag. Other naming conventions can make sense to maybe also take the version of the linux distribution into account, so this Howto simply is ment as a guide line.

There is a parameter `--build-arg GIT_COMMIT=<git_commit>` to specify a git commit hash or tag to checkout for the build.

For example to build `ea` based on a [debian docker base image](https://hub.docker.com/_/debian) using git tag `v0.1.0` can be built as follows.

        docker buildx build --build-arg GIT_COMMIT=v0.1.0 -f debian/Dockerfile -t debian_ea:0.1.0 --load .

The build takes several minutes and after this the docker image can be referenced by it's name `debian_ea:0.1.0`.

## Run a docker image

Docker also provides an easy to use command line tool to run a docker image.

        docker run -it <distribution>_ea:<tag> ea [<param>...]

For example to print help for `ea` the following command line can be used:

        docker run -it debian_ea:latest ea -h

By default a docker image does not store changes made in the file system. Instead a directory (`-v` for volume) can be set within docker to exchange files. The parameter syntax is `-v <local-dir>:<docker-dir>`. If any of the directories do not exist yet they are created with root access.

For example to check a file located on the host with `ea` the following command line can be used:

        docker run -v ~/exchange:/home/exchange -it debian_ea:latest ea /home/exchange/some_file

