import asyncio

MAX_SOCKETS = 15
DELAY = 0.1
k = 0

async def connect_socket(i):
    try:
        reader, writer = await asyncio.open_connection("127.0.0.1", 6666)
        print("Connected!")
        writer.write("PASS pass\r\n".encode())
        await asyncio.sleep(0.003)
        writer.write(f"NICK user{i}\r\n".encode())
        await asyncio.sleep(0.003)
        writer.write(f"USER a{i} 0 *\r\n".encode())
        await asyncio.sleep(0.003)
        writer.write("JOIN #general\r\n".encode())
        await asyncio.sleep(0.003)
        await writer.drain()
        
        while True:
            data = await reader.read(1024)
            if not data:
                break
            response = data.decode()
            print(f"Received: {response}")

            await asyncio.sleep(0.1)
            writer.write(f"PRIVMSG #general :chrbi wdouwchi{i}\r\n".encode())
            await writer.drain()
            
    except Exception as e:
        print(f"Error: {e}")

async def main():
    tasks = []
    for i in range(MAX_SOCKETS):
        task = asyncio.create_task(connect_socket(i))
        tasks.append(task)
    await asyncio.gather(*tasks)

if __name__ == "__main__":
    asyncio.run(main())
