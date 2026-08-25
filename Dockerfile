# syntax=docker/dockerfile:1

FROM ubuntu:24.04 AS build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install --yes --no-install-recommends \
        catch2 \
        cmake \
        g++ \
        libncurses-dev \
        libtbb-dev \
        ninja-build \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . .

RUN cmake --preset docker-tests
RUN cmake --build --preset docker-tests

FROM build AS test

CMD ["ctest", "--preset", "docker-tests", "--output-on-failure"]
