FROM lockblox/orthodox:latest

RUN git clone https://github.com/cpp-ipfs/vcpkg.git \
 && cd vcpkg \
 && ./bootstrap-vcpkg.sh \
 && ./vcpkg integrate install \
 && ./vcpkg install gtest

 ENV CMAKE_CONFIG_ARGS "-DCMAKE_TOOLCHAIN_FILE=/root/build/vcpkg/scripts/buildsystems/vcpkg.cmake"

 COPY . /root/src