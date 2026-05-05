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

document.getElementById("angleChanger").addEventListener('change', async () => {
    const val = Number(document.getElementById("angleChanger").value);
    await sendUpdate({ angle_ouverture: val });
});

document.addEventListener('DOMContentLoaded', () => {
    const btnMax = document.getElementById("btnMaxAngle");
    const angleSlider = document.getElementById("angleChanger");
    
    if (btnMax) {
        btnMax.onclick = async (e) => {
            e.preventDefault();
            const value72 = 72;

            window.isManualUpdate = true;

            document.getElementById("Angle").textContent = value72 + "°";

            const originalInterval = setInterval(() => {
                if (!window.isManualUpdate) {
                    AngleOpen();
                }
            }, 500);

            const success = await sendUpdate({ angle_ouverture: value72 });

            setTimeout(() => {
                window.isManualUpdate = false;
                clearInterval(originalInterval);
                AngleOpen();
            }, 3000);
        };
    }
});