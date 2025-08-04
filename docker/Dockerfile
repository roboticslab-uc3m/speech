ARG UBUNTU_TAG=latest
FROM ubuntu:${UBUNTU_TAG}

ARG GCC_TAG=g++
ARG CLANG_TAG=clang

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        automake \
        libtool \
        gfortran \
        ${GCC_TAG} \
        ${CLANG_TAG} \
        git \
        cmake \
        wget \
        unzip \
        ccache \
        libespeak-dev \
        mbrola-en1 \
        pulseaudio \
        bison \
        googletest \
        python3-setuptools \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/* && \
    apt-get clean

ARG SWIG_TAG=4.2.1
ARG CORES=1

RUN wget -O swig.zip https://github.com/swig/swig/archive/v${SWIG_TAG}.zip && \
    unzip swig.zip && \
    cmake -S swig-${SWIG_TAG} -B swig-${SWIG_TAG}/build -DWITH_PCRE:BOOL=OFF && \
    cmake --build swig-${SWIG_TAG}/build -- -j$CORES && \
    cmake --install swig-${SWIG_TAG}/build && \
    rm swig.zip && \
    rm -rf swig-${SWIG_TAG}

RUN wget -O piper1-gpl.zip https://github.com/roboticslab-uc3m/piper1-gpl/archive/rl.zip && \
    unzip piper1-gpl.zip && \
    cmake -S piper1-gpl-rl/libpiper -B piper1-gpl-rl/libpiper/build && \
    cmake --build piper1-gpl-rl/libpiper/build -- -j$CORES && \
    cmake --install piper1-gpl-rl/libpiper/build && \
    rm piper1-gpl.zip && \
    rm -rf piper1-gpl-rl

RUN wget -O kaldi-vosk-rl.zip https://github.com/roboticslab-uc3m/kaldi/archive/vosk-rl.zip && \
    unzip kaldi-vosk-rl.zip && \
    cd kaldi-vosk-rl/tools && \
    make -j$CORES openfst cub && \
    mkdir -p /opt/kaldi/tools/openfst && \
    cp -r openfst/lib openfst/include /opt/kaldi/tools/openfst && \
    extras/install_openblas_clapack.sh && \
    mkdir -p /opt/kaldi/tools/OpenBLAS/install && \
    cp -r OpenBLAS/install/lib OpenBLAS/install/include /opt/kaldi/tools/OpenBLAS/install && \
    cd ../src && \
    ./configure --shared --mathlib=OPENBLAS && \
    make -j$CORES online2 lm rnnlm && \
    mkdir -p /opt/kaldi/src && \
    find . -type f -name "*.h" -o -name "*.a" | xargs cp --parents -t /opt/kaldi/src/ && \
    cd ../.. && \
    rm kaldi-vosk-rl.zip && \
    rm -rf kaldi-vosk-rl
