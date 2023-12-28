#!/bin/env python

import os
import sys
import subprocess as s
from subprocess import check_output as co

#[ $UID -ne 0 ] && echo "run as root!!!" && exit 1

def setup_net(n):
  user = co(["ls", "/home"]).decode().replace("\n","")
  cmd = [
    "ip link del dev tp{}".format(n),
    "ip tuntap add dev tp{} mode tap user {}".format(n, user),
    "brctl addif virbr0 tp{}".format(n),
    "ifconfig tp{} 0.0.0.0".format(n)
  ]
  for c in cmd:
    try:
      co(c.split())
    except Exception as e:
      print("error:", e)
  print("setting up tp{}".format(n))

if os.getlogin() != "root" :
  print("run as root!")
  sys.exit(1) 

if len(sys.argv) > 1:
  count=sys.argv[1]
else:
  count=2

for i in range(1,int(count)+1):
  setup_net(i)


