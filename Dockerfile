FROM alpine:3.12.0 AS builder

WORKDIR /workspace
RUN apk --no-cache add git g++ cmake ninja libgomp openblas-dev
COPY . .
RUN git submodule update --init --recursive && \
  rm -rf build && mkdir build && cd build && \
  cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON -GNinja && \
  ninja
RUN cd build && ./run-test

FROM alpine:3.12.0
LABEL maintainer="ScatterLab developers@scatterlab.co.kr"

RUN apk --no-cache add libgcc libstdc++ libgomp openblas
WORKDIR /
COPY --from=builder /workspace/build/third_party/simdjson/libsimdjson* /lib/
COPY --from=builder /workspace/build/faiss-serving .
ENTRYPOINT ["/faiss-serving", "--host=0.0.0.0"]
