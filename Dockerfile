FROM ubuntu:16.04
RUN apt-get update
RUN apt-get --yes install software-properties-common apt-utils
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get --yes install gcc-4.7 g++-4.7 qt-sdk libasound2-dev
RUN apt-get --yes install pkg-config

# RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.6
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.7 40 --slave /usr/bin/g++ g++ /usr/bin/g++-4.7
RUN update-alternatives --config gcc
# RUN update-alternatives --config g++
# g++
# g++-4.7
# g++-5
WORKDIR /app
# COPY ./ /
# VOLUME ["/build"]
VOLUME ["/app"]
ENTRYPOINT /bin/bash
# RUN make
# RUN make release
# ENTRYPOINT ["./build/vowelcat"]


# Run me with
# docker build . --tag vowel-cat && docker run --interactive --tty --volume $PWD:/app vowel-cat


