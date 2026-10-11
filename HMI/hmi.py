import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports
import threading
import time

class HMAPerceptronApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Control & Monitoreo — Perceptrón Multidimensional ESP32")
        self.root.geometry("850x680")
        self.root.configure(bg="#1e1e2e")
        self.root.resizable(True, True)

        self.ser = None
        self.leyendo_serial = False
        self.lbl_pots = []  # Lista de etiquetas visuales para los pots activos

        self.setup_styles()
        self.create_widgets()
        self.actualizar_puertos()

    def setup_styles(self):
        self.style = ttk.Style()
        self.style.theme_use("clam")

        BG_DARK = "#1e1e2e"
        PANEL_BG = "#2a2a3c"
        TEXT_COLOR = "#cdd6f4"
        ACCENT_COLOR = "#89b4fa"

        self.style.configure("TLabelframe", background=PANEL_BG, borderwidth=1, relief="solid")
        self.style.configure("TLabelframe.Label", background=PANEL_BG, foreground=ACCENT_COLOR, font=("Segoe UI", 10, "bold"))
        self.style.configure("TLabel", background=PANEL_BG, foreground=TEXT_COLOR, font=("Segoe UI", 9))
        self.style.configure("Header.TLabel", background=BG_DARK, foreground="#ffffff", font=("Segoe UI", 14, "bold"))
        self.style.configure("Status.TLabel", background=BG_DARK, foreground="#a6adc8", font=("Segoe UI", 9, "italic"))

        self.style.configure("TCombobox", fieldbackground="#313244", background="#45475a", foreground="#ffffff")
        self.style.map("TCombobox", fieldbackground=[("readonly", "#313244")])

        self.style.configure("Accent.TButton", font=("Segoe UI", 10, "bold"), background="#89b4fa", foreground="#11111b")
        self.style.map("Accent.TButton", background=[("active", "#b4befe")])

        self.style.configure("Connect.TButton", font=("Segoe UI", 9, "bold"), background="#a6e3a1", foreground="#11111b")
        self.style.map("Connect.TButton", background=[("active", "#94e2d5")])

        self.style.configure("Disconnect.TButton", font=("Segoe UI", 9, "bold"), background="#f38ba8", foreground="#11111b")
        self.style.map("Disconnect.TButton", background=[("active", "#eba0ac")])

    def create_widgets(self):
        # -------------------------------------------------------------
        # ENCABEZADO
        # -------------------------------------------------------------
        header_frame = tk.Frame(self.root, bg="#1e1e2e")
        header_frame.pack(fill="x", padx=20, pady=(15, 5))

        lbl_titulo = ttk.Label(header_frame, text="SISTEMA DE ENTRENAMIENTO PERCEPTRÓN N-D", style="Header.TLabel")
        lbl_titulo.pack(side="left")

        self.lbl_estado_conexion = ttk.Label(header_frame, text="● Desconectado", style="Status.TLabel")
        self.lbl_estado_conexion.pack(side="right")

        # -------------------------------------------------------------
        # PANEL SUPERIOR: CONEXIÓN SERIAL
        # -------------------------------------------------------------
        frame_serial = ttk.LabelFrame(self.root, text=" Conexión Hardware (UART / Serial) ", padding=10)
        frame_serial.pack(fill="x", padx=20, pady=5)

        ttk.Label(frame_serial, text="Puerto COM:").grid(row=0, column=0, padx=5, pady=5, sticky="w")
        self.combo_puertos = ttk.Combobox(frame_serial, state="readonly", width=15)
        self.combo_puertos.grid(row=0, column=1, padx=5, pady=5)

        btn_refrescar = ttk.Button(frame_serial, text="🗘", width=3, command=self.actualizar_puertos)
        btn_refrescar.grid(row=0, column=2, padx=2, pady=5)

        ttk.Label(frame_serial, text="Baudrate:").grid(row=0, column=3, padx=(15, 5), pady=5, sticky="w")
        self.combo_baud = ttk.Combobox(frame_serial, values=["9600", "115200"], state="readonly", width=10)
        self.combo_baud.current(1)
        self.combo_baud.grid(row=0, column=4, padx=5, pady=5)

        self.btn_conectar = ttk.Button(frame_serial, text="Conectar", style="Connect.TButton", command=self.toggle_conexion)
        self.btn_conectar.grid(row=0, column=5, padx=(20, 5), pady=5)

        # -------------------------------------------------------------
        # PANEL MEDIO: PARÁMETROS DEL MODELO
        # -------------------------------------------------------------
        frame_config = ttk.LabelFrame(self.root, text=" Configuración de la Tabla de Verdad ", padding=15)
        frame_config.pack(fill="x", padx=20, pady=5)

        ttk.Label(frame_config, text="Función Lógica:").grid(row=0, column=0, padx=5, pady=5, sticky="w")
        self.combo_funcion = ttk.Combobox(frame_config, values=["AND", "OR", "OTRO (Personalizada)"], state="readonly", width=22)
        self.combo_funcion.current(0)
        self.combo_funcion.grid(row=0, column=1, padx=5, pady=5)
        self.combo_funcion.bind("<<ComboboxSelected>>", self.on_funcion_changed)

        ttk.Label(frame_config, text="Secuencia Binaria (Para 'OTRO'):").grid(row=0, column=2, padx=(20, 5), pady=5, sticky="w")
        
        self.entry_secuencia = tk.Entry(frame_config, bg="#313244", fg="#ffffff", insertbackground="white", 
                                       font=("Consolas", 10, "bold"), relief="flat", width=20, disabledbackground="#181825")
        self.entry_secuencia.grid(row=0, column=3, padx=5, pady=5)
        self.entry_secuencia.config(state="disabled")

        self.btn_entrenar = ttk.Button(frame_config, text="▶ Enviar & Entrenar", style="Accent.TButton", command=self.enviar_entrenamiento)
        self.btn_entrenar.grid(row=0, column=4, padx=(25, 5), pady=5)

        # -------------------------------------------------------------
        # PANEL TELEMETRÍA: LECTURA BINARIZADA DE ENTRADAS (POTS)
        # -------------------------------------------------------------
        frame_telemetria = ttk.LabelFrame(self.root, text=" Telemetría de Entradas Activas (Binarizado 0 / 1) ", padding=10)
        frame_telemetria.pack(fill="x", padx=20, pady=5)

        self.frame_pots_container = tk.Frame(frame_telemetria, bg="#2a2a3c")
        self.frame_pots_container.pack(fill="x", expand=True)

        self.lbl_no_data = ttk.Label(self.frame_pots_container, text="Esperando transmisión de entradas desde el ESP32...", font=("Segoe UI", 9, "italic"))
        self.lbl_no_data.pack(anchor="center", pady=5)

        # -------------------------------------------------------------
        # PANEL INFERIOR: MONITOREO Y CONSOLA
        # -------------------------------------------------------------
        frame_console = ttk.LabelFrame(self.root, text=" Monitor en Tiempo Real (ESP32 Feedback) ", padding=10)
        frame_console.pack(fill="both", expand=True, padx=20, pady=(5, 15))

        self.txt_consola = tk.Text(frame_console, bg="#11111b", fg="#a6e3a1", font=("Consolas", 9),
                                   relief="flat", wrap="word", insertbackground="white")
        self.txt_consola.pack(side="left", fill="both", expand=True)

        scrollbar = ttk.Scrollbar(frame_console, orient="vertical", command=self.txt_consola.yview)
        scrollbar.pack(side="right", fill="y")
        self.txt_consola.config(yscrollcommand=scrollbar.set)

    # -----------------------------------------------------------------
    # LÓGICA Y EVENTOS
    # -----------------------------------------------------------------
    def actualizar_puertos(self):
        puertos = [port.device for port in serial.tools.list_ports.comports()]
        self.combo_puertos['values'] = puertos
        if puertos:
            self.combo_puertos.current(0)
        else:
            self.combo_puertos.set("No detectado")

    def on_funcion_changed(self, event):
        seleccion = self.combo_funcion.get()
        if "OTRO" in seleccion:
            self.entry_secuencia.config(state="normal")
            self.entry_secuencia.focus()
        else:
            self.entry_secuencia.delete(0, tk.END)
            self.entry_secuencia.config(state="disabled")

    def toggle_conexion(self):
        if self.ser is None or not self.ser.is_open:
            puerto = self.combo_puertos.get()
            baudrate = self.combo_baud.get()

            if not puerto or puerto == "No detectado":
                messagebox.showwarning("Advertencia", "Selecciona un puerto COM válido.")
                return

            try:
                self.ser = serial.Serial(puerto, int(baudrate), timeout=1)
                self.leyendo_serial = True
                
                self.thread_lectura = threading.Thread(target=self.leer_puerto_serie, daemon=True)
                self.thread_lectura.start()

                self.lbl_estado_conexion.config(text=f"● Conectado ({puerto})", foreground="#a6e3a1")
                self.btn_conectar.config(text="Desconectar", style="Disconnect.TButton")
                self.log_consola(f"[SISTEMA] Conectado exitosamente al puerto {puerto} a {baudrate} baudios.\n")

            except Exception as e:
                messagebox.showerror("Error de Conexión", f"No se pudo abrir el puerto {puerto}.\nDetalle: {e}")
        else:
            self.leyendo_serial = False
            if self.ser and self.ser.is_open:
                self.ser.close()
            self.ser = None
            self.lbl_estado_conexion.config(text="● Desconectado", foreground="#f38ba8")
            self.btn_conectar.config(text="Conectar", style="Connect.TButton")
            self.log_consola("[SISTEMA] Puerto serie desconectado.\n")

    def enviar_entrenamiento(self):
        if self.ser is None or not self.ser.is_open:
            messagebox.showwarning("Atención", "Debes conectar el puerto serie antes de enviar datos.")
            return

        opcion = self.combo_funcion.get()
        
        if "AND" in opcion:
            comando = "CMD:AND\n"
        elif "OR" in opcion:
            comando = "CMD:OR\n"
        else:
            secuencia = self.entry_secuencia.get().strip()
            if not secuencia or not all(c in '01' for c in secuencia):
                messagebox.showerror("Error de entrada", "Para la opción personalizada debes ingresar una secuencia válida de caracteres '0' y '1'.")
                return
            comando = f"CMD:OTRO:{secuencia}\n"

        try:
            self.ser.write(comando.encode('utf-8'))
            self.log_consola(f"[TX -> ESP32] Comando enviado: {comando.strip()}\n")
        except Exception as e:
            messagebox.showerror("Error de Transmisión", f"Fallo al enviar datos por el puerto serie: {e}")

    def actualizar_widgets_pots(self, bits_str):
        # Si cambia el número de potenciómetros detectados, reconstruimos los widgets
        n_pots = len(bits_str)
        if len(self.lbl_pots) != n_pots:
            for w in self.frame_pots_container.winfo_children():
                w.destroy()
            self.lbl_pots.clear()

            for i in range(n_pots):
                f_item = tk.Frame(self.frame_pots_container, bg="#313244", relief="solid", bd=1, padx=10, pady=5)
                f_item.pack(side="left", padx=10, expand=True)

                lbl_titulo = tk.Label(f_item, text=f"Entrada {i+1}", bg="#313244", fg="#a6adc8", font=("Segoe UI", 8, "bold"))
                lbl_titulo.pack()

                lbl_val = tk.Label(f_item, text="0", bg="#313244", fg="#f38ba8", font=("Consolas", 14, "bold"))
                lbl_val.pack()

                self.lbl_pots.append(lbl_val)

        # Actualizar los valores y colores de cada potenciómetro activo
        for i, bit in enumerate(bits_str):
            if bit == '1':
                self.lbl_pots[i].config(text="1 (HIGH)", fg="#a6e3a1") # Verde para HIGH
            else:
                self.lbl_pots[i].config(text="0 (LOW)", fg="#f38ba8")  # Rojo/Rosa para LOW

    def leer_puerto_serie(self):
        while self.leyendo_serial and self.ser and self.ser.is_open:
            try:
                if self.ser.in_waiting > 0:
                    linea = self.ser.readline().decode('utf-8', errors='replace').strip()
                    if linea:
                        # Si es una trama de telemetría de los potenciómetros
                        if linea.startswith("POTS:"):
                            bits = linea.split("POTS:")[1]
                            self.root.after(0, self.actualizar_widgets_pots, bits)
                        else:
                            self.log_consola(f"[RX <- ESP32] {linea}\n")
            except Exception:
                break
            time.sleep(0.02)

    def log_consola(self, texto):
        self.txt_consola.insert(tk.END, texto)
        self.txt_consola.see(tk.END)

if __name__ == "__main__":
    root = tk.Tk()
    app = HMAPerceptronApp(root)
    root.mainloop()