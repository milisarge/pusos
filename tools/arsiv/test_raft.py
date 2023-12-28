#!/usr/bin/env python3
import sys
import json
from datetime import datetime, timedelta
import raft_pb2
data = raft_pb2.AppendEntriesRequest()
"""
    string id = 1;              // The uuid of the Raft node sending the request.(leader)
    int32 term = 2;             // The term from the requester's perspective.
    int32 prevLogIndex = 4;     // Index of entry immediately preceding new ones.
    int32 prevLogTerm = 5;      // The term of the prevLogIndex entry.
    repeated string entries = 6; // New ops to append.
    int32 leaderCommit = 7;     // The leader’s commitIndex. - what index have been received by the majority
"""
data.id= "pus1"
data.term = 1000
data.prevLogIndex = 0
data.prevLogTerm = 0
data.entries.extend([sys.argv[1]])
data.leaderCommit = 0
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


address = ("192.168.122.1", 8811)
#client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.settimeout(10)
client_socket.connect(address)

messages = [data]

for msg in messages:
  datax = msg.SerializeToString()
  #size = encode_varint(len(datax))
  #client_socket.sendall(datax+b'\x00')
  client_socket.sendall(datax)
