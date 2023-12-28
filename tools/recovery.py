#!/usr/bin/python3
# pip3 install qemu.qmp
import asyncio
from qemu.qmp import QMPClient
import sys
import nodes

#nodes_port = {
#"192.168.122.237":4441,
#"192.168.122.238":4442,
#"192.168.122.239":4443,
#"192.168.122.240":4444,
#"192.168.122.241":4445,
#}

async def main():
    qmp = QMPClient('deneme2')
    cmd = sys.argv[1]
    print("----------------------",cmd)
    ip = sys.argv[2]
    port = nodes.port[ip]
    print(cmd, port)

    await qmp.connect(("192.168.122.1",port))

    qcmd = "human-monitor-command"
    args = {}
    if cmd == "cont":
      qcmd = "cont"
    if cmd == "stop":
      qcmd = "stop"
    if cmd == "status":
      qcmd = "query-status"
    if cmd == "snapshots":
      args = {"command-line":"query-block"}	
    if cmd == "reset":
      qcmd = "system_reset"
    if cmd == "migrate":
      target_port = sys.argv[3]		
      args = {"command-line":f'migrate -d tcp:0:{target_port}'}	
    if cmd == "save":
      args = {"command-line":"savevm state101"}	
    if cmd == "load":
      args = {"command-line":"loadvm state101"}	
    if cmd == "save2":
      qcmd = "snapshot-save"
      args = {"job-id": "saver102", "tag":"state102", "vmstate" : "blk2", "devices":["blk2"]}
    if cmd == "load2":
      qcmd = "snapshot-load"
      args = {"job-id": "loader102", "tag":"state102", "vmstate" : "blk2", "devices":["blk2"]}
    print("run:",qcmd,args)
    res = await qmp.execute(qcmd, args)
    #res = await qmp.execute('query-status')
    #print(f"VM status: {res['status']}")
    print(res)
    await qmp.disconnect()

asyncio.run(main())
