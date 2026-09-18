"""
    Python bluetooth terminal, intended for the HM-10 BLE Module
        - scans, saves config, and opens a terminal to the module
        - quiting is a little buggy

    requirements.yml for required dependencies
        conda env create -f requirements.yml
        conda activate HM10Terminal

    python HM10Terminal.py

    :author: tlk1160
    :date: 9/1/25
"""

import asyncio, sys, os, yaml, select, termios, tty
from bleak import BleakScanner, BleakClient, BleakError
from tqdm import tqdm
from yaspin import yaspin
from collections import deque

CONFIG_FILE = "bt_config.yaml"
DEFAULT_CFG = {"default_scan_seconds": 5, "devices": []}

def load_config():
    if not os.path.exists(CONFIG_FILE): return DEFAULT_CFG.copy()
    try:
        with open(CONFIG_FILE, "r") as f: cfg = yaml.safe_load(f)
        if not isinstance(cfg, dict): raise ValueError("Config must be dict")
        cfg.setdefault("default_scan_seconds", DEFAULT_CFG["default_scan_seconds"])
        cfg.setdefault("devices", [])
        return cfg
    except Exception as e: sys.exit(f"Config error: {e}")

def save_config(cfg): 
    with open(CONFIG_FILE,"w") as f: yaml.safe_dump(cfg,f,sort_keys=False)

def upsert_device(cfg, newdev):
    cfg["devices"] = [d for d in cfg["devices"] if d.get("mac")!=newdev["mac"]]
    cfg["devices"].append(newdev); save_config(cfg)

async def scan_devices(seconds):
    print(f"\nScanning for {seconds} seconds... (press q to stop)\n")
    task = asyncio.create_task(BleakScanner.discover(timeout=seconds))
    fd = sys.stdin.fileno(); old = termios.tcgetattr(fd); tty.setraw(fd) # Raw init
    try:
        for _ in tqdm(range(seconds), desc="Scanning", bar_format="{l_bar}{bar}| {n_fmt}/{total_fmt}s", colour='cyan'):
            await asyncio.sleep(1)
            rlist,_,_ = select.select([sys.stdin],[],[],0)
            if rlist and sys.stdin.read(1).lower() == "q":
                task.cancel(); return []
        try: return [(d.name or "Unknown", d.address) for d in await task]
        except asyncio.CancelledError: return []
    finally: termios.tcsetattr(fd, termios.TCSADRAIN, old) # Restore

async def write_hm10(client, write_uuid, msg: str):
    mtu = 20  # HM-10 message transfer unit -> Max limit at once
    data = msg.encode()
    for i in range(0, len(data), mtu):
        chunk = data[i:i+mtu]
        await client.write_gatt_char(write_uuid, chunk)
        await asyncio.sleep(0.01)  # Small delay to let it rest

async def connect_and_terminal(dev):
    addr = dev["mac"]
    name = dev["name"]
    with yaspin(text=f"Connecting to: {name} ({addr})", color="cyan") as sp:
        try:
            async with BleakClient(addr, timeout=10) as client:
                if not client.is_connected:
                    sp.fail("✗")
                    print("Connection failed.")
                    return
                sp.ok("✔")
                print(f"Connected to {name}. Ctrl+C to quit.\n")

                # HM-10 BLE UUIDs
                notify = dev.get("notify_char_uuid") or "0000ffe1-0000-1000-8000-00805f9b34fb"
                write  = dev.get("write_char_uuid")  or "0000ffe1-0000-1000-8000-00805f9b34fb"

                recent_sent = deque(maxlen=10)
                recv_buffer = ""
                def cb(_, data):
                    nonlocal recv_buffer
                    recv_buffer += data.decode(errors="ignore")

                    # Might eat a few newlines (at the end probably)
                    while "\r\n" in recv_buffer:
                        line, recv_buffer = recv_buffer.split("\r\n", 1)
                        clean_line = line.rstrip("\r\n")
                        if clean_line and recent_sent and clean_line == recent_sent[0]:
                            recent_sent.popleft()  # Skip return echo once
                            continue
                        if dev.get("display_hex"):
                            print(f": {line.encode().hex()}", flush=True)
                        else:
                            print(f": {line}", flush=True)

                await client.start_notify(notify, cb)
                loop = asyncio.get_running_loop()
                while True:
                    try:
                        msg = await loop.run_in_executor(None, sys.stdin.readline)
                        if not msg: break
                        clean_msg = msg.rstrip("\r\n")
                        recent_sent.append(clean_msg)
                        if dev.get("send_crlf"): clean_msg += "\r\n"
                        await write_hm10(client, write, clean_msg)
                    except (KeyboardInterrupt, asyncio.exceptions.CancelledError, EOFError):
                        print("\nDisconnecting....")
                        break
        except (KeyboardInterrupt, EOFError):
            sp.fail("✗")
            print("\nQuiting....")
        except BleakError as e:
            sp.fail("✗")
            print(f"Connection error: {e}")
        except (TimeoutError, asyncio.TimeoutError) as e:
            sp.fail("✗")
            print(f"Device Timout: {e}")
        finally:
            if 'client' in locals() and client.is_connected:
                try:
                    await client.stop_notify(notify)
                except:
                    pass

async def is_bluetooth_on(timeout=0.01):
    try:
        await BleakScanner.discover(timeout=timeout)
        return True
    except BleakError: return False
    except Exception: return False

async def menu():
    cfg = load_config()
    while True:
        print("\n=== BLE Serial ===")
        print(f"Config: {CONFIG_FILE}")
        print(f"Default scan: {cfg.get('default_scan_seconds', 6)}s")
        print(" [1] Scan new device")
        print(" [2] Connect saved")
        print(" [3] Set new scan interval")
        print(" [q] Quit program")
        opt = input("Select option: ").strip() or "1"
        if opt == "1":
            devices = await scan_devices(cfg["default_scan_seconds"])
            if not devices: continue
            print("")
            for i,(n,a) in enumerate(devices): print(f" [{i}] {n} ({a})")
            while True:
                sel = input("Select device (q to menu): ").strip().lower()
                if sel == "q": sel = None; break
                if sel.isdigit() and 0 <= int(sel) < len(devices):
                    sel = int(sel); break
                print("Invalid option, try again.")
            if sel is None: continue
            name, addr = devices[int(sel)]
            dev = {
                   "mac": addr,
                   "name": name,
                   "send_crlf": True,
                   "display_hex": False,
                   "notify_char_uuid": None,
                   "write_char_uuid": None
                  }
            if input("Save device to config? (y/n): ").strip().lower() == "y": upsert_device(cfg,dev)
            await connect_and_terminal(dev)
        elif opt == "2":
            if not cfg["devices"]: print("No saved devices"); continue
            for i,d in enumerate(cfg["devices"]): print(f" [{i}] {d.get('name','?')} ({d.get('mac')})")
            sel = input("Select saved (q to menu): ").strip().lower()
            if sel == "q": continue
            if not (sel.isdigit() and 0 <= int(sel) < len(cfg["devices"])):
                print("Invalid option!"); continue
            sel = int(sel)
            await connect_and_terminal(cfg["devices"][int(sel)])
        elif opt == "3":
            newv = input("New scan interval (sec, q to cancel): ").strip().lower()
            if newv == "q": continue
            if not newv.isdigit(): print("Must be a valid integer!"); continue
            newv = max(1, int(newv))
            cfg["default_scan_seconds"] = newv
            save_config(cfg)
        elif opt == "q":
            for task in asyncio.tasks.all_tasks():
                task.cancel()
            sys.exit("Goodbye!")
        else: print("Invalid Option")

if __name__=="__main__":
    print("Checking bluetooth connection...")
    if not asyncio.run(is_bluetooth_on()):
        sys.exit(" Bluetooth adapter is OFF or otherwise unavailable!")
    print(" Bluetooth is active!")
    try: asyncio.run(menu())
    except KeyboardInterrupt: sys.exit("\nQuiting....")
