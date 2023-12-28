#!/usr/bin/env python3
import sys
import cmdserver_pb2

request = cmdserver_pb2.CmdRequest()
request.id = 345
#request.fn = "systime"
request.fn = sys.argv[1]
request.data = "info"

"""
try:
  with open("./data", "rb") as f:
    data.ParseFromString(f.read())
except IOError:
  print("data: Could not open file.  Creating a new one.")
"""

"""
with open("./data", "wb") as f:
  f.write(data.SerializeToString())
"""

import socket
import struct
import nodes

num=sys.argv[2]

address = (nodes.port[num], 5000)
#client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect(address)

datax = request.SerializeToString()
client_socket.sendall(datax+b'\x00')

# get response
resp_data = client_socket.recv(1024).strip()
print(resp_data.hex())
response = cmdserver_pb2.CmdResponse()
response.ParseFromString(resp_data)
print("id:",response.id)
print("data:",response.data)
