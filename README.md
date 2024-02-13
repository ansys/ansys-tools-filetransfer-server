# Filetransfer Tool Server

The Filetransfer Tool is a minimal gRPC API for transferring files between a client and a remote server.

This code implements the server-side in C++.

**WARNING**:

The Filetransfer Tool does not provide any security measures. Any file
on the server component can be accessed by any client. Without additional security
measures, it is unsuited for use over an untrusted network.

## Quickstart

The easiest way to use the Filetransfer Tool Server is through the Docker container:

```bash
docker run ghcr.io/ansys/tools-filetransfer
```

**NOTE**:

This Docker container is not publicly available. To use the Filetransfer Tool Server,
you will need to build the Docker container yourself. See the
[Building](#building) section for instructions.

To expose the uploaded files to another process, you can for example share a volume between two Docker containers. A docker compose file might look like this:

```yaml
version: '3.8'
services:
  other-service:
    restart: unless-stopped
    image: <other_service_image>
    <any other options needed for this service>
    working_dir: /home/container/workdir
    volumes:
      - "shared_data:/home/container/workdir/"
    user: "1000:1000"
  ansys-tools-filetransfer:
    restart: unless-stopped
    image: ${IMAGE_NAME_FILETRANSFER:-ghcr.io/ansys/tools-filetransfer:latest}
    ports:
      - "${PORT_FILETRANSFER:-50556}:50000"
    working_dir: /home/container/workdir
    volumes:
      - "shared_data:/home/container/workdir/"
    user: "1000:1000"

volumes:
  shared_data:
```

## Building

### Fetch the code

To build the server, we first need to clone the repository and its submodules

```bash
git clone https://github.com/ansys/ansys-tools-filetransfer-server
cd ansys-tools-filetransfer-server
git submodule update --init --recursive
```

### Dependencies

The dependencies are managed using ``conan``. To get it along with other development dependencies, execute the following commands in a Python (virtual) environment:

```bash
pip install -U pip wheel
pip install -r requirements_dev.txt
```

If you are otherwise using ``conan``, ensure that you have a clean "default" profile, e.g. by setting the ``CONAN_USER_HOME`` environment variable to an empty directory.

Fetch the dependencies with

On Windows:

```bash
conan install -of build --build missing --profile:host=./conan/windows_x86_64_Release --profile:build=./conan/windows_x86_64_Release ./conan
```

On Linux:

```bash
conan install -of build --build missing --profile:host=./conan/linux_x86_64_Release --profile:build=./conan/linux_x86_64_Release ./conan
```

### Configure + Build

Then use CMake to configure and build the project:

```bash
cmake -B build . -DCMAKE_TOOLCHAIN_FILE='build/conan_toolchain.cmake' -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

### Docker build

To build the docker image, run the following command from the root of the repository:

```bash
docker build -f docker/Dockerfile .
```
