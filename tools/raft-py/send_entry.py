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
data.id= "node1"
data.term = 1
data.prevLogIndex = 0
data.prevLogTerm = 0
data.entries.extend([sys.argv[1]])
data.leaderCommit = 1
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

# connect hb server and get peers
address = ('192.168.122.1', 5555)
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect(address)
client_socket.send(b"peers")
peersj = client_socket.recv(1024).strip()
peers = json.loads(peersj.decode())

#sys.exit()
for peer in peers:
  live_t = datetime.strptime(peers[peer], "%Y-%m-%dT%H:%M:%S")
  if timedelta(seconds=6) > datetime.now() - live_t:
    address = (peer, 8811)
    #client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.settimeout(10)
    client_socket.connect(address)

    messages = [data]

    for msg in messages:
      datax = msg.SerializeToString()
      #size = encode_varint(len(datax))
      client_socket.sendall(datax+b'\x00')
  else:
    print("live_t:",peer, live_t) 
