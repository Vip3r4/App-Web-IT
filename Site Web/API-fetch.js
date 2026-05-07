async function Temperature() {
            try {
                const res = await fetch("http://localhost:8000/api/serre");
                const data = await res.json();
                document.getElementById("Temp").textContent = `${data.temperature}°C`;
            } catch (err) {
                console.error("Impossible de récupérer la température", err);
                document.getElementById("Temp").textContent = "--°C";
            }
        }
        
        setInterval(Temperature, 500);
        Temperature();

let isUpdating = false; 

export function setUpdating(value) {
    isUpdating = value;
}

window.isManualUpdate = false;

async function AngleOpen() {
    if (isUpdating) return; 
    if (window.isManualUpdate) return;

    try {
        const res = await fetch("http://127.0.0.1:8000/api/serre");
        const data = await res.json();
        const angleValue = data.angle_ouverture;
        document.getElementById("Angle").textContent = `${angleValue}°`;
        
        const angleSelect = document.getElementById("angleSelect");
        if (angleSelect && angleSelect.value != angleValue) {
            angleSelect.value = angleValue;
        }
    } catch (err) {
        console.error("Impossible de récupérer l'angle d'ouverture", err);
        document.getElementById("Angle").textContent = "--°";
    }
}

setInterval(AngleOpen, 500);
AngleOpen();

async function isLedActive() {
            try {
                const res = await fetch("http://localhost:8000/api/serre");
                const data = await res.json();
                document.getElementById("EtatLed").textContent = `${data.eclairage_actif}`;
            } catch (err) {
                console.error("Impossible de récupérer l'État de la LED", err);
                document.getElementById("EtatLed").textContent = "Unknown";
            }
        }

        setInterval(isLedActive, 500);
        isLedActive();

async function isAlarmActive() {
            try {
                const res = await fetch("http://localhost:8000/api/serre");
                const data = await res.json();
                document.getElementById("EtatAlarm").textContent = `${data.alarme_panne}`;
            } catch (err) {
                console.error("Impossible de récupérer l'État de l'alarme", err);
                document.getElementById("EtatAlarm").textContent = "Unknown";
            }
        }

        setInterval(isAlarmActive, 500);
        isAlarmActive();

async function etatLED() {
            try {
                const res = await fetch("http://localhost:8000/api/serre");
                const data = await res.json();
                document.getElementById("EtatLed").textContent = `${data.etatled}`;
            } catch (err) {
                console.error("Impossible de récupérer l'État la led", err);
                document.getElementById("EtatLed").textContent = "Unknown";
            }
        }

        setInterval(etatLED, 500);
        etatLED();

export async function getTemperature() {
    try {
        const res = await fetch("http://localhost:8000/api/serre");
        const data = await res.json();
        return data.temperature;
    } catch (err) {
        console.error("Impossible de récupérer la température pour le graphique", err);
        return null;
    }
}