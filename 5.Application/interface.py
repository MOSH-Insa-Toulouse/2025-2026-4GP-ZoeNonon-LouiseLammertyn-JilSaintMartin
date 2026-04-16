from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QLabel, QPushButton
from PyQt5.QtCore import QTimer
import sys
import serial
import pyqtgraph as pg

# --- CONSTANTES ---
VCC = 5.0
R1 = 100000.0
R3 = 100000.0
R5 = 10000.0
RES_POT = 5000.0  # Résistance du pot digital après calibration

class SensorInterface(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Caractérisation Capteur : Déformation vs Résistance")
        self.resize(1000, 700)

        # 1. Connexion Série (Ajuste le port COM si besoin)
        try:
            self.ser = serial.Serial('COM7', 9600, timeout=0.1)
        except:
            print("Erreur: Impossible de se connecter au port série. Vérifie le port COM.")
            sys.exit()

        # 2. Variables de données
        self.R_repos = 0 
        self.data_res = []    # Axe X
        self.data_deform = [] # Axe Y
        self.tracing = False

        # 3. Interface Graphique (UI)
        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)
        self.layout = QVBoxLayout(self.central_widget)

        # Labels
        self.label_res = QLabel("Résistance: -- Ω")
        self.label_res.setStyleSheet("font-size: 20px; color: #e74c3c;")
        self.layout.addWidget(self.label_res)

        self.label_deform = QLabel("Déformation: -- %")
        self.label_deform.setStyleSheet("font-size: 28px; color: #27ae60; font-weight: bold;")
        self.layout.addWidget(self.label_deform)

        # Bouton
        self.start_btn = QPushButton("Démarrer Capture (10s)")
        self.start_btn.setFixedHeight(50)
        self.start_btn.clicked.connect(self.start_10s_trace)
        self.layout.addWidget(self.start_btn)

        # Graphique
        self.graph_widget = pg.PlotWidget()
        self.graph_widget.setBackground('k')
        self.graph_widget.setLabel('bottom', 'Résistance', units='Ω')
        self.graph_widget.setLabel('left', 'Déformation', units='%')
        self.graph_widget.showGrid(x=True, y=True)
        self.layout.addWidget(self.graph_widget)

        # Plot : on utilise des symboles pour voir les points de mesure
        self.line = self.graph_widget.plot(self.data_res, self.data_deform, pen=pg.mkPen('g', width=1), symbol='o', symbolSize=5)

        # 4. Timers
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_data)
        self.timer.start(50) # Lecture toutes les 50ms

        self.trace_timer = QTimer()
        self.trace_timer.timeout.connect(self.stop_10s_trace)

    def calculate_resistance(self, raw_byte):
        adc_val = raw_byte * 4 # Conversion 8 bits vers 10 bits approx
        if adc_val <= 0: return 0
        v_out = adc_val * (VCC / 1023.0)
        try:
            term1 = 1 + (R3 / RES_POT)
            term2 = VCC / v_out
            rc = (term1 * term2) * R1 - R1 - R5
            return round(rc, 2)
        except ZeroDivisionError:
            return 0

    def calculate_deformation(self, current_res):
        if self.R_repos == 0 and current_res > 0: 
            self.R_repos = current_res
            return 0
        if self.R_repos == 0: return 0
        
        # Delta R / R0
        delta_r = abs(current_res - self.R_repos)
        deformation = (delta_r / self.R_repos) * 100
        return round(deformation, 2)

    def update_data(self):
        if self.ser.in_waiting > 0:
            raw_data = self.ser.read(1)
            if raw_data:
                byte_val = ord(raw_data)
                res_val = self.calculate_resistance(byte_val)
                deform_val = self.calculate_deformation(res_val)

                if res_val > 0:
                    self.label_res.setText(f"Résistance: {res_val} Ω")
                    self.label_deform.setText(f"Déformation: {deform_val} %")

                    if self.tracing:
                        self.data_res.append(res_val)
                        self.data_deform.append(deform_val)
                        
                        # Trier les points pour que la ligne suive la courbe de gauche à droite
                        sorted_indices = sorted(range(len(self.data_res)), key=lambda k: self.data_res[k])
                        x_sorted = [self.data_res[i] for i in sorted_indices]
                        y_sorted = [self.data_deform[i] for i in sorted_indices]
                        
                        self.line.setData(x_sorted, y_sorted)

    def start_10s_trace(self):
        print("Début du tracé...")
        self.tracing = True
        self.data_res = []
        self.data_deform = []
        self.trace_timer.start(10000) # S'arrête après 10s

    def stop_10s_trace(self):
        print("Fin du tracé.")
        self.tracing = False
        self.trace_timer.stop()

    def closeEvent(self, event):
        self.ser.close()
        event.accept()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = SensorInterface()
    window.show()
    sys.exit(app.exec_())