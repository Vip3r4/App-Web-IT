from fastapi import FastAPI
from pydantic import BaseModel
from typing import Optional

app = FastAPI()

# Modèle de données pour un souhait
class Souhait(BaseModel):
    id: int
    objet: str
    prix_estime: float
    urgent: bool = False

# Notre "base de données" temporaire (une liste)
mes_souhaits = [
    {"id": 1, "objet": "Vélo", "prix_estime": 200.0, "urgent": False},
    {"id": 2, "objet": "Livre de code", "prix_estime": 25.0, "urgent": True}
]

# ROUTE 1 : Message de bienvenue
@app.get("/")
def home():
    return {"message": "Bienvenue sur mon API de souhaits !"}

# ROUTE 2 : Récupérer tous les souhaits (GET)
@app.get("/souhaits")
def get_souhaits():
    return mes_souhaits

# ROUTE 3 : Ajouter un nouveau souhait (POST)
@app.post("/souhaits")
def ajouter_souhait(nouveau_souhait: Souhait):
    mes_souhaits.append(nouveau_souhait.dict())
    return {"message": "Souhait ajouté avec succès !", "donnees": nouveau_souhait}
