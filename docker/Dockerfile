# syntax=docker/dockerfile:1

FROM ubuntu:22.04 AS builder

# Here we install all system libraries that are needed both in the
# 'builder' and 'precommit' stages. This makes the image slightly
# heavier (but builder image size doesn't really matter), but increases
# the incremental build speed.
RUN \
    --mount=type=cache,target=/var/cache/apt,id=filetransfer_server \
    apt update && apt install -y \
    clang-format-14 \
    clang-tidy-14 \
    cmake \
    g++-11 \
    git \
    make \
    python3-pip \
    --no-install-recommends && rm -rf /var/lib/apt/lists/*

ENV CC='/usr/bin/gcc-11' CPP='/usr/bin/cpp-11' CXX='/usr/bin/g++-11'

WORKDIR /workdir/

COPY --chmod=755 \
    ./requirements_dev.txt /workdir/requirements_dev.txt

RUN python3 -m pip install -r /workdir/requirements_dev.txt \
    && python3 -m pip cache purge

COPY --chmod=755 \
    ./conan /workdir/conan

RUN conan install -of build --build missing --profile:host=./conan/linux_x86_64_Release --profile:build=./conan/linux_x86_64_Release ./conan

COPY --chmod=755 \
    . /workdir

RUN cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE='build/conan_toolchain.cmake' .
RUN cmake --build build --parallel

FROM builder AS precommit

RUN \
    ln -s /usr/bin/run-clang-tidy-14 /usr/local/bin/run-clang-tidy && \
    ln -s /usr/bin/clang-format-14 /usr/local/bin/clang-format

ENTRYPOINT [ "pre-commit" ]
CMD ["run", "--all-files"]


FROM ubuntu:22.04 AS app

RUN useradd container

COPY --from=builder \
    --chown=root \
    --chmod=755 \
    /workdir/build/src/server /usr/local/bin/filetransfer_server

EXPOSE 50000

RUN mkdir -p /home/container/workdir && chown -R container:container /home/container

USER container

WORKDIR /home/container/workdir
VOLUME /home/container/workdir

ENTRYPOINT ["/usr/local/bin/filetransfer_server"]
CMD ["--server-address", "0.0.0.0:50000"]
