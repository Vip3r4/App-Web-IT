async function sendUpdate(updatedData) {
    try {
        const res = await fetch("http://127.0.0.1:8000/api/serre");
        const currentState = await res.json();
        
        const newState = { ...currentState, ...updatedData };

        const response = await fetch("http://127.0.0.1:8000/api/serre/update", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(newState)
        });

        return response.ok;
    } catch (err) {
        console.error("Erreur API :", err);
        return false;
    }
}

document.getElementById("tempChanger").addEventListener('change', async () => {
    const val = Number(document.getElementById("tempChanger").value);
    await sendUpdate({ temperature: val });
});

const angleSelect = document.getElementById("angleSelect");
if (angleSelect) {
    angleSelect.addEventListener('change', async () => {
        const val = Number(angleSelect.value);
        
        window.isManualUpdate = true;
        
        document.getElementById("Angle").textContent = val + "°";
        
        const success = await sendUpdate({ angle_ouverture: val });
        
        setTimeout(() => {
            window.isManualUpdate = false;
            if (typeof AngleOpen === 'function') {
                AngleOpen();
            }
        }, 1000);
    });
}

document.addEventListener('DOMContentLoaded', () => {
    async function syncSelectWithAPI() {
        try {
            const res = await fetch("http://127.0.0.1:8000/api/serre");
            const data = await res.json();
            if (angleSelect && data.angle_ouverture) {
                angleSelect.value = data.angle_ouverture;
                document.getElementById("Angle").textContent = data.angle_ouverture + "°";
            }
        } catch (err) {
            console.error("Erreur lors de la synchronisation du select", err);
        }
    }
    
    syncSelectWithAPI();
});

async function forceSync() {
    try {
        const res = await fetch("http://127.0.0.1:8000/api/serre");
        const data = await res.json();
        await fetch("http://127.0.0.1:8000/api/serre/update", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ ...data, synchro_angle: data.angle_ouverture })
        });
    } catch (err) {
        console.error("Sync error:", err);
    }
}

setInterval(forceSync, 10000);