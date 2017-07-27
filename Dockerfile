FROM ruby:2.4

RUN apt-get update && apt-get install -y mingw-w64
RUN gem install rake-compiler

RUN wget https://vorboss.dl.sourceforge.net/project/mingw/Other/UserContributed/regex/mingw-regex-2.5.1/mingw-libgnurx-2.5.1-src.tar.gz && \
    tar xvf mingw-libgnurx-2.5.1-src.tar.gz && \
    cd mingw-libgnurx-2.5.1 && \
    ./configure --host=i686-w64-mingw32 --enable-static --disable-shared && make

RUN wget ftp://ftp.astron.com/pub/file/file-5.22.tar.gz && \
    tar xvf file-5.22.tar.gz && \
    cd file-5.22 && \
    LDFLAGS=-L/mingw-libgnurx-2.5.1 CFLAGS=-I/mingw-libgnurx-2.5.1 ./configure --disable-silent-rules --host=i686-w64-mingw32 --enable-static --disable-shared && make

COPY . /ruby-filemagic
WORKDIR /ruby-filemagic
RUN rake-compiler cross-ruby VERSION=2.4.1
RUN rake gem:native WITH_CROSS_MAGIC=/file WITH_CROSS_GNURX=/mingw-libgnurx-2.5.1-src
CMD bash