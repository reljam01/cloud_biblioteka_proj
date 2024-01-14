FROM mongodb/mongo-cxx-driver:3.9.0-redhat-ubi-9.3

WORKDIR /build

RUN microdnf upgrade -y && microdnf install -y gcc

COPY . /build/

RUN gcc \
    -o libr \
    libr.c \
    -I/usr/local/include/libmongoc-1.0/ \
    -I/usr/local/include/libbson-1.0 \
    -L/usr/local/lib64/ \
    -lmongoc-1.0 \
    -lbson-1.0

EXPOSE 9001

CMD /build/libr
