// Configuration - you should replace with your actual ESP32 IP address
const apiUrl = "http://your-esp32-ip/api/data"; // Replace with your ESP32 IP

// Initialize chart with improved styling
let chart;

function initChart() {
  const ctx = document.getElementById("sensorChart").getContext("2d");
  chart = new Chart(ctx, {
    type: "line",
    data: {
      labels: [],
      datasets: [
        {
          label: "Nhiệt độ (°C)",
          data: [],
          borderColor: "#4361ee",
          backgroundColor: "rgba(67, 97, 238, 0.1)",
          borderWidth: 3,
          fill: true,
          tension: 0.3
        },
        {
          label: "Độ ẩm (%)",
          data: [],
          borderColor: "#4cc9f0",
          backgroundColor: "rgba(76, 201, 240, 0.1)",
          borderWidth: 3,
          fill: true,
          tension: 0.3
        },
        {
          label: "Ánh sáng (lx)",
          data: [],
          borderColor: "#f72585",
          backgroundColor: "rgba(247, 37, 133, 0.1)",
          borderWidth: 3,
          fill: true,
          tension: 0.3
        },
        {
          label: "Không khí (ppm)",
          data: [],
          borderColor: "#7209b7",
          backgroundColor: "rgba(114, 9, 183, 0.1)",
          borderWidth: 3,
          fill: true,
          tension: 0.3
        }
      ]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        y: {
          beginAtZero: false,
          grid: {
            color: "rgba(0, 0, 0, 0.05)"
          }
        },
        x: {
          grid: {
            color: "rgba(0, 0, 0, 0.05)"
          }
        }
      },
      plugins: {
        legend: {
          position: 'top',
        },
        tooltip: {
          mode: 'index',
          intersect: false
        }
      },
      interaction: {
        mode: 'nearest',
        axis: 'x',
        intersect: false
      }
    }
  });
}

// DOM elements
const refreshBtn = document.getElementById("refreshBtn");
const statusText = document.getElementById("statusText");
const errorMessage = document.getElementById("errorMessage");
const lastUpdate = document.getElementById("lastUpdate");

// Load data function
async function loadData() {
  try {
    statusText.textContent = "Đang tải...";
    errorMessage.style.display = "none";
    
    const response = await fetch(apiUrl);
    
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    const data = await response.json();

    // Update values on the dashboard
    document.getElementById("tempValue").textContent = data.temperature;
    document.getElementById("humiValue").textContent = data.humidity;
    document.getElementById("lightValue").textContent = data.light;
    document.getElementById("airValue").textContent = data.air;

    // Update chart
    chart.data.labels.push(new Date().toLocaleTimeString());
    chart.data.datasets[0].data.push(data.temperature);
    chart.data.datasets[1].data.push(data.humidity);
    chart.data.datasets[2].data.push(data.light);
    chart.data.datasets[3].data.push(data.air);

    // Limit data points to 20 for performance
    if (chart.data.labels.length > 20) {
      chart.data.labels.shift();
      chart.data.datasets.forEach(dataset => {
        dataset.data.shift();
      });
    }

    chart.update();

    // Update status and last update time
    statusText.textContent = "Kết nối thành công";
    lastUpdate.textContent = new Date().toLocaleString();
    
  } catch (err) {
    console.error("Error loading data:", err);
    statusText.textContent = "Lỗi kết nối";
    errorMessage.textContent = `Lỗi: ${err.message || "Không thể tải dữ liệu từ ESP32"}`;
    errorMessage.style.display = "block";
  }
}

// Relay control elements
const relay1Switch = document.getElementById('relay1');
const relay2Switch = document.getElementById('relay2');
const autoModeSwitch = document.getElementById('autoMode');

// Relay control functions
async function toggleRelay(relayId, state) {
  try {
    const response = await fetch(`${apiUrl}/control`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        relay: relayId,
        state: state
      })
    });
    
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    const result = await response.json();
    console.log(`Relay ${relayId} ${state ? 'ON' : 'OFF'}:`, result);
  } catch (error) {
    console.error(`Error controlling relay ${relayId}:`, error);
    // Update switch back to previous state if there was an error
    const switchElement = relayId === 1 ? relay1Switch : relay2Switch;
    switchElement.checked = !state;
  }
}

// Mode control function
async function setMode(auto) {
  try {
    const response = await fetch(`${apiUrl}/mode`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        mode: auto ? 'auto' : 'manual'
      })
    });
    
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    const result = await response.json();
    console.log(`Mode set to ${auto ? 'auto' : 'manual'}:`, result);
  } catch (error) {
    console.error(`Error setting mode:`, error);
    // Update switch back to previous state if there was an error
    autoModeSwitch.checked = !auto;
  }
}

// Event listeners for relay switches
relay1Switch.addEventListener('change', function() {
  toggleRelay(1, this.checked);
});

relay2Switch.addEventListener('change', function() {
  toggleRelay(2, this.checked);
});

// Event listener for mode switch
autoModeSwitch.addEventListener('change', function() {
  setMode(this.checked);
});

// Manual refresh button event
refreshBtn.addEventListener("click", loadData);

// Initialize the chart when the page loads
document.addEventListener('DOMContentLoaded', function() {
  initChart();
  // Initial load
  loadData();
  // Auto-refresh every 3 seconds
  setInterval(loadData, 3000);
});