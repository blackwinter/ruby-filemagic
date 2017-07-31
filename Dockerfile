FROM ruby:2.4

RUN apt-get update && apt-get install -y mingw-w64
RUN gem install rake-compiler
RUN rake-compiler cross-ruby VERSION=2.4.1 && \
    rake-compiler cross-ruby VERSION=2.3.4

RUN wget https://vorboss.dl.sourceforge.net/project/mingw/Other/UserContributed/regex/mingw-regex-2.5.1/mingw-libgnurx-2.5.1-src.tar.gz && \
    tar xvf mingw-libgnurx-2.5.1-src.tar.gz && \
    cd mingw-libgnurx-2.5.1 && \
    ./configure --host=i686-w64-mingw32 --enable-static --disable-shared && make

RUN wget ftp://ftp.astron.com/pub/file/file-5.31.tar.gz && \
    tar xvf file-5.31.tar.gz && \
    cd file-5.31 && \
    ./configure --disable-silent-rules --enable-fsect-man5 && \
    make && \
    make clean && \
    LDFLAGS=-L/mingw-libgnurx-2.5.1 CFLAGS=-I/mingw-libgnurx-2.5.1 ./configure --disable-silent-rules --enable-fsect-man5 --host=i686-w64-mingw32 && \
    make || true

COPY . /ruby-filemagic
WORKDIR /ruby-filemagic
RUN bundle install
RUN rake gem:native WITH_CROSS_MAGIC=/file-5.31 WITH_CROSS_GNURX=/mingw-libgnurx-2.5.1
CMD bash
