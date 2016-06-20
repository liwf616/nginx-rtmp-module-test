#!/bin/bash
mkdir /var/run/nginx/
nginx -V
nginx -s stop
nohup nginx &

echo "start nginx success!!"
