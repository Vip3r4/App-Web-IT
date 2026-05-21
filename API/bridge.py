import serial
import requests
import time
import json

arduino_port = "COM20" 
baud_rate = 9600
api_url = "http://127.0.0.1:8000/api/serre/update"
get_url = "http://127.0.0.1:8000/api/serre"

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
                
                if line.startswith("POS:"):
                    angle = int(line.split(":")[1])
                    current_res = requests.get(get_url)
                    current_data = current_res.json()
                    current_data["angle_ouverture"] = angle
                    current_data["synchro_angle"] = angle
                    requests.post(api_url, json=current_data)
                    print(f"📐 Position synchronisée: {angle}°")
                    continue
                
                parts = line.split(',')
                
                if len(parts) == 5:
                    try:
                        current_res = requests.get(get_url)
                        current_data = current_res.json()
                        current_etatled = current_data.get("etatled", False)
                    except:
                        current_etatled = False
                    
                    data = {
                        "temperature": float(parts[0]),
                        "angle_ouverture": int(parts[1]),
                        "eclairage_actif": bool(int(parts[2])),
                        "alarme_panne": bool(int(parts[3])),
                        "etatled": current_etatled
                    }
                    
                    response = requests.post(api_url, json=data)
                    
                    if response.status_code == 200:
                        print(f"✅ API mise à jour")
                    else:
                        print(f"⚠️ Erreur API (Code {response.status_code})")
                    
    except Exception as e:
        print(f"Erreur pendant la boucle : {e}")

    time.sleep(0.5)