import { getTemperature } from '/workspaces/App-Web-IT/Site Web/API-fetch';

const tempDisplay = document.getElementById("Temp");
const ctx = document.getElementById('tempChart').getContext('2d');

let chartData = {
    labels: [],
    datasets: [{
        label: 'Température (°C)',
        data: [],
        borderColor: 'red',
        fill: false,
        tension: 0.3
    }]
};

const tempChart = new Chart(ctx, {
    type: 'line',
    data: chartData,
    options: {
        responsive: true,
        scales: {
            x: { title: { display: true, text: 'Heure' } },
            y: { title: { display: true, text: 'Température (°C)' }, min: 10, max: 30 }
        }
    }
});

async function updateChart() {
    const temp = await getTemperature();
    if (temp === null) return;

    tempDisplay.textContent = temp;

    const now = new Date().toLocaleTimeString();
    chartData.labels.push(now);
    chartData.datasets[0].data.push(temp);

    if (chartData.labels.length > 20) {
        chartData.labels.shift();
        chartData.datasets[0].data.shift();
    }

    tempChart.update();
}

setInterval(updateChart, 500);
updateChart();