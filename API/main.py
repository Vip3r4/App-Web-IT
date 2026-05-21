from fastapi import FastAPI
from pydantic import BaseModel
from fastapi.middleware.cors import CORSMiddleware
import serial

app = FastAPI()

try:
    ser = serial.Serial('COM20', 9600)
    print("✅ Port COM17 connecté à l'API")
except serial.serialutil.SerialException:
    ser = None
    print("⚠️ COM17 occupé ou indisponible. L'API tourne sans accès USB direct.")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

def send_to_arduino(data):
    message = f"L{int(data.etatled)};A{data.angle_ouverture};T{int(data.temperature)}\n"
    print("Envoyé :", message)
    ser.write(message.encode())

class EtatSerre(BaseModel):
    temperature: float
    angle_ouverture: int
    etatled: bool
    alarme_panne: bool

donnees_actuelles = {
    "temperature": 0.0,
    "angle_ouverture": 5,
    "etatled": False,
    "alarme_panne": False,
}

@app.get("/api/serre")
def obtenir_etat():
    return donnees_actuelles

@app.post("/api/serre/update")
def mettre_a_jour(nouvel_etat: EtatSerre):
    global donnees_actuelles

    nouvel_etat.angle_ouverture = max(0, min(72, nouvel_etat.angle_ouverture))
    nouvel_etat.temperature = max(14, min(28, nouvel_etat.temperature))

    donnees_actuelles = nouvel_etat.dict()

    send_to_arduino(nouvel_etat)

    return {"status": "success"}
