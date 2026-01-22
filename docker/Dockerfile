# syntax=docker/dockerfile:1

FROM ghcr.io/astral-sh/uv:0.7.17 AS uv-getter

FROM ubuntu:24.04 AS builder

# Here we install all system libraries that are needed both in the
# 'builder' and 'precommit' stages. This makes the image slightly
# heavier (but builder image size doesn't really matter), but increases
# the incremental build speed.
RUN \
    --mount=type=cache,target=/var/cache/apt,id=filetransfer_server \
    apt update && apt install -y \
    ca-certificates \
    clang-format-14 \
    clang-tidy-14 \
    cmake \
    g++-13 \
    git \
    make \
    --no-install-recommends && rm -rf /var/lib/apt/lists/*

COPY --from=uv-getter /uv /uvx /bin/

ENV CC='/usr/bin/gcc-13' CPP='/usr/bin/cpp-13' CXX='/usr/bin/g++-13'
ENV UV_LINK_MODE=copy

WORKDIR /workdir/

COPY --chmod=755 \
    ./uv.lock ./pyproject.toml /workdir/

RUN uv sync

COPY --chmod=755 \
    ./conan /workdir/conan

########
# This block is kept for debugging purposes
# show diff between available and required packages
# RUN uv run conan graph explain --profile:host=./conan/linux/linux_x86_64_Release --profile:build=./conan/linux/linux_x86_64_Release ./conan/linux
# write conan.lock file to ./conan/linux
# RUN uv run conan lock create ./conan/linux --profile:host=./conan/linux/linux_x86_64_Release --profile:build=./conan/linux/linux_x86_64_Release
########

RUN uv run conan install --lockfile=./conan/linux/conan.lock -of build --profile:host=./conan/linux/linux_x86_64_Release --profile:build=./conan/linux/linux_x86_64_Release ./conan/linux

COPY --chmod=755 \
    . /workdir

RUN cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE='build/conan_toolchain.cmake' .
RUN cmake --build build --parallel

FROM builder AS precommit

RUN \
    ln -s /usr/bin/run-clang-tidy-14 /usr/local/bin/run-clang-tidy && \
    ln -s /usr/bin/clang-format-14 /usr/local/bin/clang-format

ENTRYPOINT [ "uv", "run", "pre-commit" ]
CMD ["run", "--all-files"]


FROM ubuntu:22.04 AS workdir_creator

RUN useradd container

RUN mkdir -p /home/container/workdir && chown -R container:container /home/container

FROM workdir_creator AS app_full

COPY --from=builder \
    --chown=root \
    --chmod=755 \
    /workdir/build/src/server /usr/local/bin/filetransfer_server

EXPOSE 50000

USER container

WORKDIR /home/container/workdir
VOLUME /home/container/workdir

ARG BUILD_DATE
ARG VCS_REF

LABEL org.opencontainers.image.created=$BUILD_DATE
LABEL org.opencontainers.image.authors="Dominik Gresch <dominik.gresch@ansys.com>"
LABEL org.opencontainers.image.vendor="ANSYS Inc."
LABEL org.opencontainers.image.revision=$VCS_REF

ENTRYPOINT ["/usr/local/bin/filetransfer_server"]

FROM scratch AS app_minimal

COPY --from=builder \
    --chmod=755 \
    /workdir/build/src/server /usr/local/bin/filetransfer_server

USER 1000:1000

COPY --from=workdir_creator --chown=1000:1000 --chmod=777 /home/container/workdir /home/container/workdir

EXPOSE 50000

WORKDIR /home/container/workdir
VOLUME /home/container/workdir

ARG BUILD_DATE
ARG VCS_REF

LABEL org.opencontainers.image.created=$BUILD_DATE
LABEL org.opencontainers.image.authors="Dominik Gresch <dominik.gresch@ansys.com>"
LABEL org.opencontainers.image.vendor="ANSYS Inc."
LABEL org.opencontainers.image.revision=$VCS_REF

ENTRYPOINT ["/usr/local/bin/filetransfer_server"]
