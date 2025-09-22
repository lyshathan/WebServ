import asyncio
import aiohttp
import random
import string
import nest_asyncio

nest_asyncio.apply()

URL = "http://localhost:8080"

def random_string(n=8):
    return ''.join(random.choice(string.ascii_letters) for _ in range(n))

async def get_request(session):
    async with session.get(URL + "/") as resp:
        await resp.text()

async def post_request(session):
    data = {"msg": random_string(20)}
    async with session.post(URL + "/upload", data=data) as resp:
        await resp.text()

async def delete_request(session):
    filename = random_string(6) + ".txt"
    async with session.delete(URL + f"/upload/{filename}") as resp:
        await resp.text()

async def worker(session, task_id):
    for _ in range(50):
        choice = random.choice(["GET", "POST", "DELETE"])
        try:
            if choice == "GET":
                await get_request(session)
            elif choice == "POST":
                await post_request(session)
            elif choice == "DELETE":
                await delete_request(session)
        except Exception as e:
            print(f"Worker {task_id} error: {e}")

async def main():
    async with aiohttp.ClientSession() as session:
        tasks = [worker(session, i) for i in range(100)]
        await asyncio.gather(*tasks)

if __name__ == "__main__":
    asyncio.run(main())

