import socketserver
from datetime import datetime
import json

peers={}
#class MyTCPHandler(socketserver.BaseRequestHandler):
class MyTCPHandler(socketserver.StreamRequestHandler):
    """
    The request handler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        # self.request is the TCP socket connected to the client
        self.data = self.request.recv(1024).strip()
        if self.data == b"peers":
          print("....")
          self.request.sendall(json.dumps(peers).encode())
        else:
          print("{} wrote:".format(self.client_address[0]))
          print(self.data)
          # store its alive time to dict
          peers[self.client_address[0]] = datetime.now().strftime("%Y-%m-%dT%H:%M:%S") 
          # just send back the same data, but upper-cased
          self.request.sendall(b"OK!")

if __name__ == "__main__":
    HOST, PORT = "0.0.0.0", 5555

    # Create the server, binding to localhost on port 9999
    
    with socketserver.TCPServer((HOST, PORT), MyTCPHandler) as server:
        # Activate the server; this will keep running until you
        # interrupt the program with Ctrl-C
        server.serve_forever()

