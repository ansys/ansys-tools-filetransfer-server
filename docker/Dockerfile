# syntax=docker/dockerfile:1
FROM gcc:11-bullseye AS builder_base

RUN \
    --mount=type=cache,target=/var/cache/apt \
    apt update && apt install -y \
    cmake \
    python3-pip \
    --no-install-recommends && rm -rf /var/lib/apt/lists/*

RUN python3 -m pip install conan==1.54.0 \
    && python3 -m pip cache purge

COPY --chown=container \
    --chmod=755 \
    ./conan /workdir/conan

WORKDIR /workdir/

RUN conan install -if build --build missing --profile ./conan/linux_x86_64_Release ./conan

COPY --chmod=755 \
    . /workdir

RUN cmake -B build .

FROM builder_base AS builder

RUN cmake --build build --parallel

FROM builder_base AS precommit

RUN \
    --mount=type=cache,target=/var/cache/apt \
    apt update && apt install -y \
    clang-format-13 \
    clang-tidy-13 \
    git \
    --no-install-recommends && rm -rf /var/lib/apt/lists/*

RUN \
    ln -s /usr/bin/run-clang-tidy-13 /usr/local/bin/run-clang-tidy && \
    ln -s /usr/bin/clang-format-13 /usr/local/bin/clang-format

RUN python3 -m pip install -r requirements_dev.txt \
    && python3 -m pip cache purge

RUN cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .

ENTRYPOINT [ "pre-commit" ]
CMD ["run", "--all-files"]

FROM debian:bullseye-slim AS app

RUN useradd container

COPY --from=builder \
    --chown=root \
    --chmod=755 \
    /workdir/build/bin/server /usr/local/bin/filetransfer_server

EXPOSE 50000

RUN mkdir -p /home/container/workdir && chown -R container:container /home/container

USER container

WORKDIR /home/container/workdir
VOLUME /home/container/workdir

ENTRYPOINT ["/usr/local/bin/filetransfer_server"]
CMD ["--server-address", "0.0.0.0:50000"]
