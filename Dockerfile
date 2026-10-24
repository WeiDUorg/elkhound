FROM docker.io/ocaml/opam:debian-ocaml-4.08

USER root

ENV PATH $PATH:/home/opam/.opam/4.08/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
ENV ELKHOUND_BUILD_DIR "build"
ENV ELKHOUND_SRC_DIR "src"

RUN apt-get update -yqqq && \
  apt-get install -yqqq bison wget flex cmake && \
  mkdir -p ${ELKHOUND_BUILD_DIR}

ADD src src

RUN mkdir -p ${ELKHOUND_BUILD_DIR} && \
  cmake -Wno-dev -S ${ELKHOUND_SRC_DIR} -B ${ELKHOUND_BUILD_DIR} -D CMAKE_BUILD_TYPE=Release && \
  make -j$(nproc) -C ${ELKHOUND_BUILD_DIR} && \
  mv ${ELKHOUND_BUILD_DIR}/elkhound/elkhound /usr/bin/elkhound && \
  chmod +x /usr/bin/elkhound

USER opam

CMD ["elkhound"]
