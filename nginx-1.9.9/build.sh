#!/bin/bash
./configure --prefix=/home/liwenfeng/src/opensource/nginx-learing/nginx-1.9.9/run \
    --conf-path=/etc/nginx/nginx.conf \
    --sbin-path=/usr/sbin/nginx \
    --error-log-path=/var/log/nginx/error.log \
    --http-log-path=/var/log/nginx/access.log \
    --pid-path=/var/run/nginx/nginx.pid \
    --lock-path=/var/lock/nginx.lock \
    --add-module=../hello-world \
    --add-module=../nginx-rtmp-module \
    --with-debug

make
make install
