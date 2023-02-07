# Filetransfer Utility Server

The Filetranser Utility is a minimal gRPC API for transferring files between a client and a remote server.

This code implements the server-side in C++.

## Quickstart

The easiest way to use the Filetransfer Utility Server is through the provided docker container:

```bash
docker run ghcr.io/ansys/tools-filetransfer
```

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

If you are otherwise using ``conan``, ensure that you have a clean "default" profile, e.g. by setting the ``CONAN_USER_HOME`` to an empty directory.

Fetch the dependencies with

On Windows:

```bash
conan install -if build --build missing --profile ./conan/windows_x86_64_Release ./conan
```

On Linux:

```bash
conan install -if build --build missing --profile ./conan/linux_x86_64_Release ./conan
```

### Configure + Build

Then use CMake to configure and build the project:

```bash
cmake -B build .
cmake --build build --config Release --parallel
```
