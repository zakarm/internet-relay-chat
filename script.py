import subprocess
import time

server = "localhost"
port = 5000
clients  = []
for i in range(1000):
    clients.append("client"+str(i))

for nickname in clients:
    cmd = f'nc {server} {port}'
    subprocess.Popen(cmd, shell=True)
