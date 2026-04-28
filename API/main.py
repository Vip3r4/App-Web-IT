from fastapi import FastAPI
from pydantic import BaseModel
from fastapi.middleware.cors import CORSMiddleware
import serial

app = FastAPI()

ser = serial.Serial('COM3', 9600)

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

    nouvel_etat.angle_ouverture = max(5, min(15, nouvel_etat.angle_ouverture))
    nouvel_etat.temperature = max(14, min(28, nouvel_etat.temperature))

    donnees_actuelles = nouvel_etat.dict()

    send_to_arduino(nouvel_etat)

    return {"status": "success"}
