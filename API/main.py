from fastapi import FastAPI
from pydantic import BaseModel
from fastapi.middleware.cors import CORSMiddleware

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
    message = f"L{int(data.etatLed)};A{data.angle};T{data.temperature}\n"
    ser.write(message.encode())

class EtatSerre(BaseModel):
    temperature: float
    angle_ouverture: int
    etatled: bool
    alarme_panne: bool

donnees_actuelles = {
    "temperature": 0.0,
    "angle_ouverture": 0,
    "etatled": False,
    "alarme_panne": False,
}

@app.get("/api/serre")
def obtenir_etat():
    return donnees_actuelles

@app.post("/api/serre/update")
def mettre_a_jour(nouvel_etat: EtatSerre):
    global donnees_actuelles
    donnees_actuelles = nouvel_etat.dict()
    return {"status": "success"}

@app.post("/control")
def control(data: EtatSerre):
    donnees_actuelles["etatled"] = data.etatled
    donnees_actuelles["angle"] = data.angle
    donnees_actuelles["temperature"] = data.temperature

    print(donnees_actuelles)

    send_to_arduino(data)

    return {"status": "ok"}
