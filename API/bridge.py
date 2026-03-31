import serial
import requests
import time

arduino_port = "COM3" 
baud_rate = 9600
api_url = "http://127.0.0.1:8000/api/serre/update"

print(f"--- Lancement du Pont Arduino -> API ---")

try:
    ser = serial.Serial(arduino_port, baud_rate, timeout=1)
    time.sleep(2)
    print(f"Connecté avec succès sur {arduino_port}")
except Exception as e:
    print(f"ERREUR : Impossible de se connecter à l'Arduino sur {arduino_port}")
    print(f"Détail : {e}")
    exit()

while True:
    try:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            
            if line:
                print(f"Brut reçu : {line}")
                
               
                parts = line.split(',')
                
                if len(parts) == 5:
                    
                    data = {
                        "temperature": float(parts[0]),
                        "angle_ouverture": int(parts[1]),
                        "eclairage_actif": bool(int(parts[2])),
                        "alarme_panne": bool(int(parts[3])),
                        "etatled": False
                    }
                    

                    response = requests.post(api_url, json=data)
                    
                    if response.status_code == 200:
                        print("✅ API mise à jour avec succès !")
                    else:
                        print(f"⚠️ Erreur API (Code {response.status_code})")
                else:
                    print("❌ Format de données incorrect (attendu: 5 valeurs)")
                    
    except Exception as e:
        print(f"Erreur pendant la boucle : {e}")

    time.sleep(0.5)
