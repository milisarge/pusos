#!/usr/bin/env python3
import sys
import simple_pb2
data = simple_pb2.SimpleMessage()
data.lucky_number = int(sys.argv[1])

"""
try:
  with open("./data", "rb") as f:
    data.ParseFromString(f.read())
except IOError:
  print("data: Could not open file.  Creating a new one.")
"""

with open("./data", "wb") as f:
  f.write(data.SerializeToString())

import socket
import struct
from google.protobuf.internal.encoder import _VarintEncoder
from google.protobuf.internal.decoder import _DecodeVarint

def encode_varint(value):
    """ Encode an int as a protobuf varint """
    data = []
    _VarintEncoder()(data.append, value, False)
    return b''.join(data)


def decode_varint(data):
    """ Decode a protobuf varint to an int """
    return _DecodeVarint(data, 0)[0]

address = ('192.168.122.239', 8888)
#client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect(address)

messages = [data]

for msg in messages:
  datax = msg.SerializeToString()
  size = encode_varint(len(datax))
  client_socket.sendall(datax+b'\x00')
