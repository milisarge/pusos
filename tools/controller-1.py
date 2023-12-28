#!/bin/env python3

# pusos node controller
# requirements:
# protobuf qemu.qmp

# heartbeat service
# vm save/restore

import threading
import socketserver
from datetime import datetime
import json
import raft_pb2
import time
import sys
import os

# recovery feature
import asyncio
from qemu.qmp import QMPClient

import subprocess

import logging
logging.basicConfig(filename='pus.log', encoding='utf-8', level=logging.DEBUG, format="%(asctime)s;%(levelname)s;%(message)s",datefmt='%d/%m/%Y %H:%M:%S')


peers={}
load_wait = 6
hb_wait = 1.2
#class MyTCPHandler(socketserver.BaseRequestHandler):
class MyUDPHandler(socketserver.BaseRequestHandler):
    """
    The request handler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """
    def setup(self):
      print("server stup")
    def handle(self):
        # self.request is the TCP socket connected to the client
        req = raft_pb2.AppendEntriesRequest()
        self.data = self.request[0]
        print(self.data.hex())
        req.ParseFromString(self.data)

        print("id:",req.id)
        print("term",req.term)
        print("entries",req.entries)
        ip = self.client_address[0]
        print("ip",ip)
        if "savevm" in req.entries:
          #cmd = ["python", "recovery.py","save"]
          #subprocess.check_output(cmd, stderr=subprocess.STDOUT, timeout=2) 
          logging.info("ip:%s save-vm")
          os.system("python recovery.py save "+ip)
          hb_wait = 5
        # save last hb record
        #peers[self.client_address[0]] = datetime.now().strftime("%Y-%m-%dT%H:%M:%S") 
        peers[ip] = datetime.now()
        logging.info("ip:%s hb",ip)

def hb_checker():
  global load_wait
  global hb_wait
  while(1):
    for p in list(peers):
      print(".......")
      diff = datetime.now() - peers[p]
      print(p,diff.total_seconds())
      #loop = asyncio.new_event_loop()
      #asyncio.set_event_loop(loop)
      #loop.run_until_complete(evaluate(diff.total_seconds()))
      #loop.close()
      #evaluate(diff.total_seconds())
      try:
        if diff.total_seconds() < hb_wait:
          #subprocess.run(["python", "recovery.py","save"])
          logging.info("ip:%s save",p)
          os.system("python recovery.py save "+p)
          # 5 sn geçtiyse son başarılı snapshot yükle
          load_wait = 7
          hb_wait = 1.2
        if diff.total_seconds() > load_wait:
          #subprocess.run(["python", "recovery.py","load"])
          logging.info("ip:%s load",p)
          os.system("python recovery.py load "+p)
          load_wait += diff.total_seconds() * 8
        if diff.total_seconds() > 250:
          #subprocess.run(["python", "recovery.py","reset"])
          #subprocess.run(["python", "recovery.py","cont"])
          os.system("python recovery.py reset "+p)
          os.system("python recovery.py cont "+p)
          os.system("python recovery.py load "+p)
      except subprocess.CalledProcessError as e:
        print("connection error: virtualization point", e.output)
      time.sleep(6)

if __name__ == "__main__":
    # check peers hb timeout
    checker_thread = threading.Thread(target=hb_checker)
    checker_thread.start()
    #asyncio.get_event_loop().run_until_complete(hb_checker())

    HOST, PORT = "0.0.0.0", 8811
    server = socketserver.UDPServer((HOST, PORT), MyUDPHandler)
    with server:
      server_thread = threading.Thread(target=server.serve_forever)
      server_thread.start()
      server_thread.join()
      checker_thread.join()
      server.shutdown()
