import asyncio

MAX_SOCKETS = 15
DELAY = 0.01
k = 0

async def connect_socket(i):
    try:
        reader, writer = await asyncio.open_connection("127.0.0.1", 6667)
        print("Connected!")
        writer.write(f"NICK mok{i}\r\n".encode())
        writer.write("USER o o o o\r\n".encode())
        writer.write("PASS 123\r\n".encode())
        writer.write("JOIN #general\r\n".encode())
        await writer.drain()
        while True:
            await asyncio.sleep(0.003)
            writer.write(f"PRIVMSG #general :chrbi wdouwchi{k}\r\n".encode())
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
