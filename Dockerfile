FROM ruby:2.4

RUN apt-get update && apt-get install -y mingw-w64 && gem install rake-compiler
ENV HOST_ARCH i686-w64-mingw32

ENV GNURX_VERSION 2.5.1
ENV GNURX_SOURCE /mingw-libgnurx-$GNURX_VERSION
RUN curl https://vorboss.dl.sourceforge.net/project/mingw/Other/UserContributed/regex/mingw-regex-$GNURX_VERSION/mingw-libgnurx-$GNURX_VERSION-src.tar.gz | \
    tar xzvf - && \
    cd $GNURX_SOURCE && \
    ./configure --host=$HOST_ARCH --enable-static --disable-shared && \
    make

ENV MAGIC_VERSION 5.31
ENV MAGIC_SOURCE /file-$MAGIC_VERSION
RUN curl ftp://ftp.astron.com/pub/file/file-$MAGIC_VERSION.tar.gz | \
    tar xzvf - && \
    cd $MAGIC_SOURCE && \
    ./configure --disable-silent-rules --enable-fsect-man5 && \
    make && \
    make clean && \
    LDFLAGS=-L$GNURX_SOURCE CFLAGS=-I$GNURX_SOURCE ./configure --disable-silent-rules --enable-fsect-man5 --host=$HOST_ARCH && \
    make || true

ENV CROSS_RUBIES 2.3.4 2.4.1
RUN for i in $CROSS_RUBIES; do rake-compiler cross-ruby VERSION=$i; done

ENV APP_SOURCE /ruby-filemagic
RUN mkdir $APP_SOURCE
WORKDIR $APP_SOURCE

RUN echo "source 'https://rubygems.org'\ngemspec\n" > Gemfile
COPY *.gemspec .
RUN bundle install

COPY . .
ENTRYPOINT rake gem:native WITH_CROSS_GNURX=$GNURX_SOURCE WITH_CROSS_MAGIC=$MAGIC_SOURCE
