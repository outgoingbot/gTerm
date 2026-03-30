import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports
import threading
import math
import time

class SerialTesterApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Serial Port Tester")
        self.ser = None

        # Track sine wave progress in func 3
        self.sine_step = 0
        self.max_sine_steps = 100 # Total points in one 2*pi cycle
        
        # --- UI Layout ---
        self.setup_ui()
        
    def setup_ui(self):
        # Configuration Frame
        config_frame = ttk.LabelFrame(self.root, text="Connection Settings", padding="10")
        config_frame.pack(fill="x", padx=10, pady=5)

        ttk.Label(config_frame, text="Port:").grid(row=0, column=0, sticky="w")
        self.port_var = tk.StringVar()
        self.port_combo = ttk.Combobox(config_frame, textvariable=self.port_var)
        self.port_combo['values'] = [p.device for p in serial.tools.list_ports.comports()]
        self.port_combo.grid(row=0, column=1, padx=5)

        ttk.Label(config_frame, text="Baud:").grid(row=0, column=2, sticky="w")
        self.baud_var = tk.StringVar(value="9600")
        self.baud_combo = ttk.Combobox(config_frame, textvariable=self.baud_var)
        self.baud_combo['values'] = ("9600", "115200", "19200", "38400", "57600")
        self.baud_combo.grid(row=0, column=3, padx=5)

        self.btn_connect = ttk.Button(config_frame, text="Connect", command=self.toggle_connection)
        self.btn_connect.grid(row=0, column=4, padx=5)

        # Custom Actions Frame
        action_frame = ttk.LabelFrame(self.root, text="Custom Actions", padding="10")
        action_frame.pack(fill="x", padx=10, pady=5)

        ttk.Button(action_frame, text="Func 1", command=self.custom_func_1).grid(row=0, column=0, padx=5)
        ttk.Button(action_frame, text="Func 2", command=self.custom_func_2).grid(row=0, column=1, padx=5)
        ttk.Button(action_frame, text="Func 3", command=self.custom_func_3).grid(row=0, column=2, padx=5)
        ttk.Button(action_frame, text="Func 4", command=self.custom_func_4).grid(row=0, column=3, padx=5)

        # Text Area Frame
        text_frame = ttk.LabelFrame(self.root, text="Terminal / Input", padding="10")
        text_frame.pack(fill="both", expand=True, padx=10, pady=5)

        self.text_area = tk.Text(text_frame, height=10)
        self.text_area.pack(fill="both", expand=True)

    # --- Connection Logic ---
    def toggle_connection(self):
        if self.ser and self.ser.is_open:
            self.disconnect()
        else:
            self.connect()

    def connect(self):
        try:
            port = self.port_var.get()
            baud = self.baud_var.get()
            self.ser = serial.Serial(port, baud, timeout=1)
            self.btn_connect.config(text="Disconnect")
            self.text_area.insert(tk.END, f"Connected to {port} at {baud}\n")
        except Exception as e:
            messagebox.showerror("Error", f"Could not open port: {e}")

    def disconnect(self):
        if self.ser:
            self.ser.close()
        self.btn_connect.config(text="Connect")
        self.text_area.insert(tk.END, "Disconnected.\n")

    # --- Your Custom Functions ---
    def custom_func_1(self):
        # Example: Sending data from the text box
        content = self.text_area.get("1.0", tk.END).strip()
        self.send_data(f"FUNC1: {content}")
        print("Function 1 executed")

    def custom_func_2(self):
        self.send_data("COMMAND_ALPHA")
        print("Function 2 executed")

    def custom_func_3(self):
        # Add your logic here
        print("Function 3 executing")
        
        if not self.ser or not self.ser.is_open:
            #pass
            messagebox.showwarning("Warning", "Connect to a port first!")
            return

        # Simple for-loop as requested
        # Note: self.max_sine_steps should be defined in __init__ (e.g., 100)
        for i in range(self.max_sine_steps + 1):
            Scaler = 10
            Offset = 10
            # 1. Calculate current point (0 to 2*pi)
            x = (i / self.max_sine_steps) * (2 * math.pi)
            y = math.sin(x) * Scaler + Offset
            
            # 2. Format to 6 decimal places
            formatted_value = f"{y:.6f}"
            
            print(formatted_value)
            # 3. Send via your existing helper
            self.send_data(formatted_value)
            
            # 4. 1ms delay
            time.sleep(0.001) # use this to help debugging

        # 5. UI Feedback (this will only appear AFTER the loop finishes)
        self.text_area.insert("end", "Sine wave transmission complete.\n")
        self.text_area.see("end")
        print("Function 3 Done")
        
    def custom_func_4(self):
        # Add your logic here
        print("Function 4 executed")

    def send_data(self, data):
        if self.ser and self.ser.is_open:
            self.ser.write(f"{data}\n".encode('utf-8'))
        else:
            messagebox.showwarning("Warning", "Serial port is not open!")

if __name__ == "__main__":
    root = tk.Tk()
    app = SerialTesterApp(root)
    root.mainloop()