document.getElementById("tempChanger").addEventListener('change', async () => {
    const inputValue = Number(document.getElementById("tempChanger").value);
    try {
        const res = await fetch("http://localhost:8000/api/serre");
        const currentState = await res.json();
        currentState.temperature = inputValue;
        await fetch("http://localhost:8000/api/serre/update", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(currentState)
        });
        console.log("Température envoyée :", inputValue);
    } catch (err) {
        console.error("Impossible de changer la température", err);
    }
});

document.getElementById("angleChanger").addEventListener('change', async () => {
    const inputValue = Number(document.getElementById("angleChanger").value);
    try {
        const res = await fetch("http://localhost:8000/api/serre");
        const currentState = await res.json();
        currentState.angle_ouverture = inputValue;
        await fetch("http://localhost:8000/api/serre/update", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(currentState)
        });
        console.log("Angle envoyé :", inputValue);
    } catch (err) {
        console.error("Impossible de changer l'angle", err);
    }
});