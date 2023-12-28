#!/bin/sh

# nanopb python kütüphanesi
# pip3 install nanopb

# .local/bin $PATH ekle

# ör
# nanopb_generator -I . cmdserver.proto

# parametre proto file olacak - C için
#nanopb_generator  $1.proto

# python için ise alttaki şekilde
nanopb_generator --protoc-opt "--python_out=."  $1.proto
sed -i "s#<pb.h>#<lib/pb.h>#g" $1.pb.h
