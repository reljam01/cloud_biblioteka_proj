FROM ubuntu:20.04
#FROM gui:slika
WORKDIR /app
COPY . .
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt install -y gcc
RUN apt-get install -y libgtk-3-dev
RUN gcc `pkg-config --cflags gtk+-3.0` -o g1 grafika.c `pkg-config --libs gtk+-3.0`
CMD ["./g1"]
