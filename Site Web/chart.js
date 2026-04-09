import { getTemperature } from './API-fetch.js';

const ctx = document.getElementById('tempChart').getContext('2d');

const chartData = {
    labels: [],
    datasets: [{
        label: 'Température (°C)',
        data: [],
        borderColor: 'white',
        backgroundColor: 'rgba(167, 139, 250, 0.14)',
        fill: true,
        tension: 0.3
    }]
};

const tempChart = new Chart(ctx, {
    type: 'line',
    data: chartData,
    options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
        scales: {
            x: {
                title: { display: true, text: 'Heure' }
            },
            y: {
                title: { display: true, text: 'Température (°C)' },
                min: 14,
                max: 28
            }
        }
    }
});

async function updateChart() {
    const temp = await getTemperature();
    if (temp === null) return;

    const now = new Date().toLocaleTimeString();

    chartData.labels.push(now);
    chartData.datasets[0].data.push(temp);

    if (chartData.labels.length > 20) {
        chartData.labels.shift();
        chartData.datasets[0].data.shift();
    }

    tempChart.update();

    const tempDisplay = document.getElementById("Temp");
    if (tempDisplay) tempDisplay.textContent = `${temp}°C`;
}

setInterval(updateChart, 800);
updateChart();