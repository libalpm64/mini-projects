import discord
from discord.ext import commands, tasks
import requests
import json
import asyncio
from datetime import datetime, timedelta

TOKEN = 'REPLACE_ME'

url = "https://cdn2.arkdedicated.com/servers/asa/officialserverlist.json"

server_tags = {
    "2001": "ST6",
    "2006": "Unrivaled",
    "2007": "Wizard team",
    "2011": "Outlawz",
    "2014": "C++",
    "2023": "BDE",
    "2024": "Good Coms",
    "2025": "TBD",
    "2026": "M9ID",
    "2027": "Nameless",
    "2030": "BNE",
    "2031": "DisneyHood",
    "2032": "Eclipse",
    "2034": "100% Savages",
    "2039": "BSG",
    "2041": "Plumps",
    "2046": "SSS",
    "2048": "Good Coms",
    "2049": "Oh No Savages",
    "2056": "Goonpuff",
    "2060": "Agency",
    "2063": "BOS",
    "2071": "BOS",
    "2074": "WDF",
    "2077": "DOTW",
    "2078": "Disorder",
    "2079": "AOD",
    "2086": "CxD",
    "2097": "Bad Vibez",
    "2100": "PNG + NM",
    "2102": "SRU",
    "2113": "NLNB",
    "2115": "Pistole",
    "2120": "Pistole",
    "2121": "Chem B",
    "2122": "FTG",
    "2124": "Space Cowboys",
    "2125": "HCE",
    "2128": "TPM",
    "2129": "VOID",
    "2132": "Skynet",
    "2133": "Black Fellas",
    "2134": "Mandingos",
    "2135": "BD",
    "2136": "HMP",
    "2139": "Broken Bullet",
    "2140": "Wookies",
    "2142": "B&G",
    "2143": "Boonk Gang",
    "2144": "NLDX",
    "2148": "N3",
    "2149": "Gang Gang",
    "2150": "Good Comms",
    "2154": "ADAT",
    "2156": "N1C",
    "2158": "DTF",
    "2159": "Panda",
    "2160": "NHF",
    "2162": "Cute",
    "2163": "DT6",
    "2165": "Hydra",
    "2174": "Panda",
    "2175": "Otter Club",
    "2178": "Panda",
    "2179": "Panda",
    "2181": "Hot Fix",
    "2183": "IRR",
    "2196": "HMP",
    "2201": "SBF",
    "2202": "FD",
    "2205": "Invictus",
    "2206": "TBN",
    "2209": "WLDD",
    "2211": "ArkFellas",
    "2216": "B&G",
    "2218": "WDF",
    "2219": "BNE",
    "2222": "AXK",
    "2230": "N1C?",
    "2239": "Bloodbourne",
    "2242": "Gang Gang",
    "2244": "FTG",
    "2246": "Void",
    "2249": "Bloodbourne",
    "2252": "Panda",
    "2256": "TMNT",
    "2268": "Notorious",
    "2269": "Gang Gang",
    "2270": "The fellas",
    "2271": "Invictus",
    "2283": "Space Rangers",
    "2288": "Biu Biu",
    "2295": "FEDEX",
    "2307": "FEDEX",
    "2309": "Disorder",
    "2312": "N3",
    "2316": "Good Comms",
    "2319": "WBW",
    "2322": "TPG",
    "2324": "Notorious",
    "2327": "NE",
    "2330": "Cute",
    "2336": "HMP",
    "2339": "Good Coms",
    "2341": "IDGAF",
    "2342": "Wild Wookies",
    "2344": "Gang Gang",
    "2345": "Boom Evil",
    "2348": "WLDD",
    "2350": "DALG",
    "2353": "Wiu Wiu",
    "2364": "B&G",
    "2365": "Lucifer Morningstar",
    "2371": "NLDX",
    "2377": "Invictus",
    "2390": "Disorder",
    "2399": "Pistole",
    "2403": "HCE",
    "2405": "TPG",
    "2406": "TBN",
    "2411": "NLDX",
    "2417": "Good Comms"
}

intents = discord.Intents.default()
bot = commands.Bot(command_prefix='!', intents=intents)

capped_servers = set()
suspicious_servers = {}

@tasks.loop(seconds=45)
async def update_server_info():
    global capped_servers, suspicious_servers

    channel_id = 1258215768395354174
    channel = bot.get_channel(channel_id)

    if not channel:
        print(f"Channel with ID {channel_id} not found.")
        return

    try:
        response = requests.get(url)
        response.raise_for_status()

        server_data = response.json()

        player_count = {}

        for server in server_data:
            if server["SessionIsPve"] == 0:
                skip_server = False
                for keyword in ["Small", "Console", "Modded", "Expires", "SOTF", "Isolated", "Conquest", "Arkpocalypse", "Test"]:
                    if keyword in server["SessionName"]:
                        skip_server = True
                        break
                if not skip_server:
                    player_count[server["Name"]] = server["NumPlayers"]

        sorted_servers = sorted(player_count.items(), key=lambda x: x[1], reverse=True)

        message = "Top 25 populated non-PvE servers:\n"
        count = 0
        for name, players in sorted_servers[:25]:
            count += 1
            tag = get_server_tag(name)
            message += f"{count}. {name}{tag}: {players}\n"

        capped_servers.clear()
        for server_name, num_players in player_count.items():
            if num_players >= 67:
                capped_servers.add(server_name)

        if capped_servers:
            message += "\nCapped Servers:\n"
            for server_name in capped_servers:
                tag = get_server_tag(server_name)
                message += f"{server_name}{tag}\n"

        now = datetime.utcnow()
        updated_message = message

        for server_name, current_players in player_count.items():
            if server_name in suspicious_servers:
                previous_players, flagged_time, expire_time = suspicious_servers[server_name]
                if current_players >= previous_players + 10:
                    if server_name not in capped_servers:
                        if now < expire_time:
                            updated_message += f"\n\nSuspicious servers (more than 10 players joined in the past five minutes, this note will expire in 30 minutes):\n"
                            tag = get_server_tag(server_name)
                            updated_message += f"{server_name}{tag} - Players: {current_players}\n"
                        else:
                            del suspicious_servers[server_name]
                else:
                    suspicious_servers[server_name] = (current_players, flagged_time, expire_time)
            else:
                suspicious_servers[server_name] = (current_players, now, now + timedelta(minutes=30))

        found_message = None
        async for msg in channel.history(limit=100):
            if msg.author == bot.user:
                found_message = msg
                break

        if found_message:
            await found_message.edit(content=updated_message)
        else:
            await channel.send(updated_message)

        print("Message sent successfully.")

    except Exception as e:
        print(f"Error: {e}")

def get_server_tag(server_name):
    for number, tag in server_tags.items():
        if f"{number}" in server_name:
            return f" ({tag})"
    return ""

@bot.command()
async def update(ctx):
    await ctx.send("Updating server info...")
    update_server_info.restart()

@bot.event
async def on_ready():
    print(f'Logged in as {bot.user}!')
    print(f'Bot is connected to guilds:')
    for guild in bot.guilds:
        print(f'- {guild.name} (id: {guild.id})')
    update_server_info.start()

bot.run(TOKEN)