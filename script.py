import subprocess
import time

server = "localhost"
port = 6661
clients  = []
for i in range(1000):
    clients.append("client"+str(i))

for nickname in clients:
    cmd = f'echo "Hello from {nickname}!" | nc {server} {port}'
    subprocess.Popen(cmd, shell=True)
