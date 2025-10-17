import customtkinter as ctk
import serial
import serial.tools.list_ports
import json
import subprocess
from tkinter import messagebox, filedialog

# ============ APP CONFIG ============
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

# ============ MAIN APP ============
class LEDControllerApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("ESP32 LED Controller")
        self.geometry("500x620")
        self.minsize(500, 620)
        self.iconbitmap("icon.ico")

        # Serial connection (not connected yet)
        self.serial_conn = None

        # UI components
        self.create_widgets()

    def create_widgets(self):
        # Title
        self.label_title = ctk.CTkLabel(self, text="ESP32 LED Controller", font=("Arial", 22, "bold"))
        self.label_title.pack(pady=15)

        # Port selection + connect row
        port_frame = ctk.CTkFrame(self)
        port_frame.pack(pady=10)

        self.port_var = ctk.StringVar(value="Select Port")
        self.port_menu = ctk.CTkOptionMenu(port_frame, variable=self.port_var, values=self.get_serial_ports())
        self.port_menu.grid(row=0, column=0, padx=5, pady=5)

        self.connect_btn = ctk.CTkButton(port_frame, text="Connect", command=self.connect_serial)
        self.connect_btn.grid(row=0, column=1, padx=5)

        # === NEW: Flash firmware button ===
        self.flash_btn = ctk.CTkButton(port_frame, text="⚡ Flash Firmware", fg_color="#4444aa", command=self.flash_firmware)
        self.flash_btn.grid(row=0, column=2, padx=5)

        # Brightness slider
        self.brightness_slider = ctk.CTkSlider(self, from_=0, to=255, number_of_steps=256, command=self.on_brightness_change)
        self.brightness_slider.pack(pady=10)
        ctk.CTkLabel(self, text="Brightness").pack()

        # Speed slider
        self.speed_slider = ctk.CTkSlider(self, from_=0.1, to=5, command=self.on_speed_change)
        self.speed_slider.set(1)
        self.speed_slider.pack(pady=10)
        ctk.CTkLabel(self, text="Speed Multiplier").pack()

        # Color sliders
        self.r_slider = self.create_color_slider("Red")
        self.g_slider = self.create_color_slider("Green")
        self.b_slider = self.create_color_slider("Blue")

        # Preset buttons
        self.label_preset = ctk.CTkLabel(self, text="Presets", font=("Arial", 16, "bold"))
        self.label_preset.pack(pady=10)

        preset_frame = ctk.CTkFrame(self)
        preset_frame.pack(pady=10)

        presets = ["Rainbow", "Solid", "Random Shift", "Preset 4", "Preset 5", "Preset 6"]
        rows, cols = 2, 3
        for i, preset in enumerate(presets):
            btn = ctk.CTkButton(preset_frame, text=preset, width=120,
                                command=lambda p=preset: self.send_preset(p))
            btn.grid(row=i // cols, column=i % cols, padx=5, pady=5)

        # Save preset button
        self.save_btn = ctk.CTkButton(self, text="💾 Save Preset", fg_color="orange", command=self.save_preset)
        self.save_btn.pack(pady=10)

    def create_color_slider(self, name):
        label = ctk.CTkLabel(self, text=name)
        label.pack()
        slider = ctk.CTkSlider(self, from_=0, to=255, number_of_steps=256, command=self.on_color_change)
        slider.pack(pady=5)
        return slider

    def get_serial_ports(self):
        ports = serial.tools.list_ports.comports()
        return [p.device for p in ports] or ["No ports found"]

    # ==== Serial Functions ====
    def connect_serial(self):
        port = self.port_var.get()
        try:
            self.serial_conn = serial.Serial(port, 115200, timeout=1)
            print(f"Connected to {port}")
            self.connect_btn.configure(text="Connected", fg_color="green")
        except Exception as e:
            messagebox.showerror("Connection Error", f"Failed to connect:\n{e}")
            print("Failed to connect:", e)

    # ==== Flash Firmware ====
    def flash_firmware(self):
        # Ask user to choose .bin file
        firmware_path = filedialog.askopenfilename(
            title="Select ESP32 Firmware (.bin)",
            filetypes=[("Binary Files", "*.bin")]
        )
        if not firmware_path:
            return  # user cancelled

        port = self.port_var.get()
        if port == "Select Port" or port == "No ports found":
            messagebox.showerror("Error", "Please select a valid port before flashing.")
            return

        # If serial is open, close it before flashing
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()

        try:
            # Run esptool.py command
            messagebox.showinfo("Flashing", "Flashing firmware... Please wait, this may take a few seconds.")
            subprocess.run([
                "esptool.py", "--chip", "esp32",
                "--port", port, "--baud", "460800",
                "write_flash", "-z", "0x1000", firmware_path
            ], check=True)
            messagebox.showinfo("Success", "Firmware updated successfully! 🎉")
        except FileNotFoundError:
            messagebox.showerror("Error", "esptool.py not found. Please install it with:\n\npython -m pip install esptool")
        except subprocess.CalledProcessError:
            messagebox.showerror("Error", "Failed to flash firmware.\nMake sure your ESP32 is in bootloader mode.")

    # ==== Control Handlers ====
    def send_to_esp32(self, data):
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.write((json.dumps(data) + "\n").encode())

    def on_brightness_change(self, value):
        self.send_to_esp32({"type": "brightness", "value": int(value)})

    def on_speed_change(self, value):
        self.send_to_esp32({"type": "speed", "value": float(value)})

    def on_color_change(self, value):
        color = {
            "r": int(self.r_slider.get()),
            "g": int(self.g_slider.get()),
            "b": int(self.b_slider.get())
        }
        self.send_to_esp32({"type": "color", "value": color})

    def send_preset(self, preset):
        self.send_to_esp32({"type": "preset", "value": preset})

    def save_preset(self):
        self.send_to_esp32({"type": "save"})

# ============ RUN APP ============
if __name__ == "__main__":
    app = LEDControllerApp()
    app.mainloop()
