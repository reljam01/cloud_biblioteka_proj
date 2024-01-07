FROM alpine:3.19.0
WORKDIR /app
COPY . .
RUN apk add gcc
RUN apk add musl-dev
RUN apk add gtk+3.0-dev
RUN gcc `pkg-config --cflags gtk+-3.0` -o g1 grafika.c `pkg-config --libs gtk+-3.0`
CMD ["./g1"]
