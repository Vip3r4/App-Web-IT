from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()

class EtatSerre(BaseModel):
    temperature: float
    angle_ouverture: int
    eclairage_actif: bool
    alarme_panne: bool
    etatled: bool

donnees_actuelles = {
    "temperature": 0.0,
    "angle_ouverture": 0,
    "eclairage_actif": False,
    "alarme_panne": False,
    "etatled": False
}

@app.get("/api/serre")
def obtenir_etat():
    return donnees_actuelles

@app.post("/api/serre/update")
def mettre_a_jour(nouvel_etat: EtatSerre):
    global donnees_actuelles
    donnees_actuelles = nouvel_etat.dict()
    return {"status": "success"}

app.use(cors({
    origin: "http://localhost:3000"
}));
