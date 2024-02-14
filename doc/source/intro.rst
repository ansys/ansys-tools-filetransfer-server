.. _getting_started:

Getting started
---------------

Getting the code
~~~~~~~~~~~~~~~~

The first step to get started with the Filetransfer Tool Server is to clone the repository and its submodules:

.. code-block:: bash

    git clone https://github.com/ansys-internal/ansys-tools-filetransfer-server
    cd ansys-tools-filetransfer-server
    git submodule update --init --recursive


Building the server
~~~~~~~~~~~~~~~~~~~

There are two options to build the Filetransfer Tool Server:

- Using Docker: Build a Docker image that contains the server.
- Manual build (Linux or Windows): Build the server directly on your machine.

.. tab-set::

    .. tab-item:: Docker

        To build the Docker image, run the following command from the root of the repository:

        .. code-block:: bash

            docker build -f docker/Dockerfile . --tag filetransfer-tool-server

    .. tab-item:: Linux

        To build on Linux, you need to have the following tools installed:

        - ``python3``
        - ``cmake``
        - the ``g++`` compiler

        First, you need to install the development dependencies:

        .. code-block:: bash

            python3 -m pip install pipx
            pipx ensurepath
            pipx install poetry
            poetry install --no-root

        Then, use Conan to fetch the C++ dependencies:

        .. code-block:: bash

            poetry run conan install -of build --build missing --profile:host=./conan/linux_x86_64_Release --profile:build=./conan/linux_x86_64_Release ./conan

        Finally, use CMake to configure and build the project:

        .. code-block:: bash

            cmake -B build . -DCMAKE_TOOLCHAIN_FILE='build/conan_toolchain.cmake' -DCMAKE_BUILD_TYPE=Release
            cmake --build build --config Release --parallel

    .. tab-item:: Windows

        To build on Windows, you need to have the following tools installed:

        - ``python``
        - ``cmake``
        - the MSVC compiler

        First, you need to install the development dependencies:

        .. code-block:: powershell

            python -m pip install pipx
            pipx ensurepath
            pipx install poetry
            poetry install --no-root

        Then, use Conan to fetch the C++ dependencies:

        .. code-block:: powershell

            poetry run conan install -of build --build missing --profile:host=.\conan\windows_x86_64_Release --profile:build=.\conan\windows_x86_64_Release .\conan

        Finally, use CMake to configure and build the project:

        .. code-block:: powershell

            cmake -B build . -DCMAKE_TOOLCHAIN_FILE='build/conan_toolchain.cmake' -DCMAKE_BUILD_TYPE=Release
            cmake --build build --config Release --parallel


Running the server
~~~~~~~~~~~~~~~~~~

After the server is built, you can run it in the following ways:

.. tab-set::

    .. tab-item:: Docker

        .. code-block:: bash

            docker run -p 50000:50000 filetransfer-tool-server

        This will start the server and expose it on port 50000.

        To make the uploaded files available to another process, you can share a volume between two Docker containers. A Docker Compose file might look like this:

        .. code-block:: yaml

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
                image: filetransfer-tool-server
                ports:
                  - "50000:50000"
                working_dir: /home/container/workdir
                volumes:
                  - "shared_data:/home/container/workdir/"
                user: "1000:1000"

            volumes:
              shared_data:


    .. tab-item:: Linux

        .. code-block:: bash

            ./build/src/server --server-address localhost:50000


    .. tab-item:: Windows

        .. code-block:: powershell

            .\build\src\Release\server.exe --server-address localhost:50000


Command-line options
~~~~~~~~~~~~~~~~~~~~

The Filetransfer Tool Server provides the following command-line options:

- ``--help`` - Display a help message and exit.
- ``--server-address`` - Configure the address on which the server is listening.
