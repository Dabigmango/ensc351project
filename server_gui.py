import os
import json
import socket
import tkinter as tk
from tkinter import ttk, filedialog, messagebox

# ------------------------------------------------------------
# Config – adjust BEAGLE_IP if needed
# ------------------------------------------------------------
BEAGLE_IP = "192.168.7.2"   # Beagle’s IP
PORT = 12345                # UDP port

BASE_DIR = os.path.expanduser("~/ensc351/public/myApps/ms2-node")
MUSIC_DIR = os.path.join(BASE_DIR, "music")
PLAYLIST_FILE = os.path.join(BASE_DIR, "playlists.json")

os.makedirs(MUSIC_DIR, exist_ok=True)

# ------------------------------------------------------------
# UDP helpers (same protocol as Node server)
# ------------------------------------------------------------
def send_cmd(msg: str):
    print("[UDP] Sending:", msg)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(msg.encode("utf-8"), (BEAGLE_IP, PORT))
    sock.close()

def play_song(filename: str):
    send_cmd(f"PLAY {filename}")

def stop_song():
    send_cmd("STOP")

def set_volume(vol: int):
    send_cmd(f"SET_VOL {vol}")

# ------------------------------------------------------------
# Playlist data model – shared with Node server
# Structure: { "playlists": { name: [song, ...], ... } }
# ------------------------------------------------------------
def load_state():
    if not os.path.exists(PLAYLIST_FILE):
        return {"playlists": {}}
    try:
        with open(PLAYLIST_FILE, "r") as f:
            data = json.load(f)
        if "playlists" not in data:
            data = {"playlists": {}}
        return data
    except Exception as e:
        print("Error reading playlists.json:", e)
        return {"playlists": {}}

def save_state(state):
    try:
        with open(PLAYLIST_FILE, "w") as f:
            json.dump(state, f, indent=2)
    except Exception as e:
        print("Error writing playlists.json:", e)

def scan_music_dir():
    files = []
    for name in os.listdir(MUSIC_DIR):
        if name.lower().endswith((".wav", ".mp3")):
            files.append(name)
    files.sort()
    return files

# ------------------------------------------------------------
# Tkinter GUI
# ------------------------------------------------------------

class MS2Gui(tk.Tk):
    def send_initial_state(self):
        # Send each playlist
        for pl_name, songs in self.state["playlists"].items():
            send_cmd(f"ADD_PLAYLIST {pl_name}")
            # Send each song inside the playlist
            for song in songs:
                send_cmd(f"ADD_SONG {pl_name} {song}")

    def __init__(self):
        super().__init__()
        self.title("Music Server")
        self.state = load_state()
        self.all_songs = scan_music_dir()

        self.create_widgets()
        self.refresh_all_songs()
        self.refresh_playlists()
        self.after(100, self.send_initial_state)

    # ---------- UI layout ----------
    def create_widgets(self):
        root = ttk.Frame(self, padding=10)
        root.grid(row=0, column=0, sticky="nsew")
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)

        # Left: All songs + upload
        left = ttk.Frame(root)
        left.grid(row=0, column=0, sticky="nsew", padx=(0, 10))
        root.columnconfigure(0, weight=1)
        root.rowconfigure(0, weight=1)

        ttk.Label(left, text="All Songs").grid(row=0, column=0, sticky="w")

        self.song_list = tk.Listbox(left, height=15)
        self.song_list.grid(row=1, column=0, sticky="nsew")
        scroll_all = ttk.Scrollbar(left, orient="vertical", command=self.song_list.yview)
        scroll_all.grid(row=1, column=1, sticky="ns")
        self.song_list.config(yscrollcommand=scroll_all.set)
        left.rowconfigure(1, weight=1)
        left.columnconfigure(0, weight=1)

        btn_upload = ttk.Button(left, text="Upload new songs", command=self.upload_songs)
        btn_upload.grid(row=2, column=0, pady=(5, 0), sticky="ew")

        btn_rescan = ttk.Button(left, text="Rescan music/", command=self.rescan_music)
        btn_rescan.grid(row=3, column=0, pady=(5, 0), sticky="ew")

        # Right: Playlists
        right = ttk.Frame(root)
        right.grid(row=0, column=1, sticky="nsew")
        root.columnconfigure(1, weight=1)

        # Playlist row
        top_pl = ttk.Frame(right)
        top_pl.grid(row=0, column=0, sticky="ew", pady=(0, 5))
        ttk.Label(top_pl, text="Playlists").grid(row=0, column=0, sticky="w")

        self.playlist_combo = ttk.Combobox(top_pl, state="readonly", width=20)
        self.playlist_combo.grid(row=0, column=1, padx=5)
        self.playlist_combo.bind("<<ComboboxSelected>>", lambda e: self.refresh_songs_in_playlist())

        ttk.Label(top_pl, text="New playlist:").grid(row=1, column=0, sticky="w", pady=(5,0))
        self.new_playlist_name = tk.StringVar()
        ttk.Entry(top_pl, textvariable=self.new_playlist_name, width=20).grid(row=1, column=1, padx=5, pady=(5,0))

        ttk.Button(top_pl, text="Add", command=self.add_playlist).grid(row=1, column=2, padx=5)
        ttk.Button(top_pl, text="Delete", command=self.delete_playlist).grid(row=1, column=3, padx=5)

        # Songs in playlist
        ttk.Label(right, text="Songs in playlist").grid(row=1, column=0, sticky="w")
        self.pl_song_list = tk.Listbox(right, height=10)
        self.pl_song_list.grid(row=2, column=0, sticky="nsew")
        scroll_pl = ttk.Scrollbar(right, orient="vertical", command=self.pl_song_list.yview)
        scroll_pl.grid(row=2, column=1, sticky="ns")
        self.pl_song_list.config(yscrollcommand=scroll_pl.set)
        right.rowconfigure(2, weight=1)
        right.columnconfigure(0, weight=1)

        # Buttons to manage songs in playlist
        buttons_pl = ttk.Frame(right)
        buttons_pl.grid(row=3, column=0, pady=5, sticky="ew")
        ttk.Button(buttons_pl, text="Add selected from All Songs →", command=self.add_song_to_playlist).grid(row=0, column=0, padx=5)
        ttk.Button(buttons_pl, text="Remove selected from playlist", command=self.remove_song_from_playlist).grid(row=0, column=1, padx=5)

        # Bottom row: playback controls
        controls = ttk.Frame(root, padding=(0,10,0,0))
        controls.grid(row=1, column=0, columnspan=2, sticky="ew")
        root.rowconfigure(1, weight=0)

        ttk.Button(controls, text="Play selected (playlist)", command=self.play_selected_from_playlist).grid(row=0, column=0, padx=5)
        ttk.Button(controls, text="Play selected (All Songs)", command=self.play_selected_from_all).grid(row=0, column=1, padx=5)
        ttk.Button(controls, text="Stop", command=stop_song).grid(row=0, column=2, padx=5)

        ttk.Label(controls, text="Volume").grid(row=0, column=3, padx=(20,5))
        self.vol_var = tk.IntVar(value=50)
        vol_scale = ttk.Scale(controls, from_=0, to=100, orient="horizontal",
                              command=lambda v: set_volume(int(float(v))))
        vol_scale.set(50)
        vol_scale.grid(row=0, column=4, sticky="ew")
        controls.columnconfigure(4, weight=1)

    # ---------- Data & UI refresh ----------
    def refresh_all_songs(self):
        self.all_songs = scan_music_dir()
        self.song_list.delete(0, tk.END)
        for name in self.all_songs:
            self.song_list.insert(tk.END, name)

    def refresh_playlists(self):
        names = sorted(self.state["playlists"].keys())
        self.playlist_combo["values"] = names
        if names:
            current = self.playlist_combo.get()
            if current not in names:
                self.playlist_combo.set(names[0])
        else:
            self.playlist_combo.set("")
        self.refresh_songs_in_playlist()

    def refresh_songs_in_playlist(self):
        self.pl_song_list.delete(0, tk.END)
        pl_name = self.playlist_combo.get()
        if not pl_name:
            return
        songs = self.state["playlists"].get(pl_name, [])
        for s in songs:
            self.pl_song_list.insert(tk.END, s)

    # ---------- Playlist operations ----------
    def add_playlist(self):
        name = self.new_playlist_name.get().strip()
        if not name:
            messagebox.showwarning("Playlist", "Enter a playlist name.")
            return
        if name in self.state["playlists"]:
            messagebox.showwarning("Playlist", "Playlist already exists.")
            return
        self.state["playlists"][name] = []
        save_state(self.state)
        self.new_playlist_name.set("")
        self.refresh_playlists()
        send_cmd(f"ADD_PLAYLIST {name}")

    def delete_playlist(self):
        pl_name = self.playlist_combo.get()
        if not pl_name:
            return
        if not messagebox.askyesno("Delete", f"Delete playlist '{pl_name}'?"):
            return
        self.state["playlists"].pop(pl_name, None)
        save_state(self.state)
        self.refresh_playlists()
        send_cmd(f"DELETE_PLAYLIST {pl_name}")

    def add_song_to_playlist(self):
        pl_name = self.playlist_combo.get()
        if not pl_name:
            messagebox.showwarning("Playlist", "Select a playlist first.")
            return
        sel = self.song_list.curselection()
        if not sel:
            messagebox.showwarning("Song", "Select a song from 'All Songs'.")
            return
        song = self.song_list.get(sel[0])
        songs = self.state["playlists"].setdefault(pl_name, [])
        if song not in songs:
            songs.append(song)
            save_state(self.state)
            self.refresh_songs_in_playlist()
            send_cmd(f"ADD_SONG {pl_name} {song}")

    def remove_song_from_playlist(self):
        pl_name = self.playlist_combo.get()
        if not pl_name:
            return
        sel = self.pl_song_list.curselection()
        if not sel:
            return
        song = self.pl_song_list.get(sel[0])
        songs = self.state["playlists"].get(pl_name, [])
        if song in songs:
            songs.remove(song)
            save_state(self.state)
            self.refresh_songs_in_playlist()
            send_cmd(f"DELETE_SONG {pl_name} {song}")

    # ---------- Playback helpers ----------
    def play_selected_from_playlist(self):
        sel = self.pl_song_list.curselection()
        if not sel:
            messagebox.showwarning("Play", "Select a song from the playlist.")
            return
        song = self.pl_song_list.get(sel[0])
        play_song(song)

    def play_selected_from_all(self):
        sel = self.song_list.curselection()
        if not sel:
            messagebox.showwarning("Play", "Select a song from 'All Songs'.")
            return
        song = self.song_list.get(sel[0])
        play_song(song)

    # ---------- File upload ----------
    def upload_songs(self):
        files = filedialog.askopenfilenames(
            title="Select audio files",
            filetypes=[("Audio", "*.wav *.mp3"), ("All files", "*.*")]
        )
        if not files:
            return
        for path in files:
            name = os.path.basename(path)
            dest = os.path.join(MUSIC_DIR, name)
            try:
                with open(path, "rb") as src, open(dest, "wb") as dst:
                    dst.write(src.read())
                print("Copied", name, "to", dest)
            except Exception as e:
                print("Failed to copy", path, "->", dest, ":", e)
        self.refresh_all_songs()

    def rescan_music(self):
        self.refresh_all_songs()
        messagebox.showinfo("Rescan", "Rescanned music/ folder.")

# ------------------------------------------------------------
# Main
# ------------------------------------------------------------
if __name__ == "__main__":
    app = MS2Gui()
    app.mainloop()
