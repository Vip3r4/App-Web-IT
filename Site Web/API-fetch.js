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

async function AngleOpen() {
            try {
                const res = await fetch("http://localhost:8000/api/serre");
                const data = await res.json();
                document.getElementById("Angle").textContent = `${data.angle_ouverture}°`;
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