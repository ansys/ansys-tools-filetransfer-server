# syntax=docker/dockerfile:1
FROM ubuntu:22.04 AS builder

RUN apt update && apt install -y \
    gcc-11 \
    cmake \
    python3-pip \
    python3-venv \
    && rm -rf /var/lib/apt/lists/*

RUN useradd container

USER container

WORKDIR /home/container/

RUN python3 -m venv dev_env && \
    . dev_env/bin/activate && \
    pip install conan==1.50.0

COPY --chown=container \
    --chmod=755 \
    ./conan /home/container/conan

RUN . dev_env/bin/activate && \
    conan install -if build --profile ./conan/linux_x86_64_Release ./conan

COPY --chown=container \
    --chmod=755 \
    . /home/container

RUN . dev_env/bin/activate && \
    cmake -B build . && \
    cmake --build build


FROM ubuntu:22.04 AS app

RUN useradd container

COPY --from=builder \
    --chown=container \
    --chmod=777 \
    /home/container/build/bin/server /home/container/server

USER container

CMD ["/home/container/server"]
