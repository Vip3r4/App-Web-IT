from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()

# Modèle pour l'état complet de la serre
class EtatSerre(BaseModel):
    temperature: float
    angle_ouverture: int
    eclairage_actif: bool
    alarme_panne: bool
    couleur_led: str  # Rouge, Jaune ou Vert

# Stockage en mémoire (sera mis à jour par l'Arduino)
donnees_actuelles = {
    "temperature": 0.0,
    "angle_ouverture": 0,
    "eclairage_actif": False,
    "alarme_panne": False,
    "couleur_led": "Vert"
}

@app.get("/api/serre")
def obtenir_etat():
    return donnees_actuelles

@app.post("/api/serre/update")
def mettre_a_jour(nouvel_etat: EtatSerre):
    global donnees_actuelles
    donnees_actuelles = nouvel_etat.dict()
    return {"status": "success"}
