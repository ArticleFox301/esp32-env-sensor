// Configuration - you should replace with your actual ESP32 IP address
const apiUrl = "http://your-esp32-ip/api/data"; // Replace with your ESP32 IP

// MQTT Configuration for HiveMQ public broker
// Using unencrypted WebSocket endpoint for HiveMQ public broker
const mqttBroker = "ws://broker.hivemq.com:8000/mqtt";
const mqttTopic = "esp32/sensors/data";
const clientId = "dashboard_client_" + Math.random().toString(16).substr(2, 8);

// Initialize chart with improved styling
let chart;
let mqttClient;

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

// Initialize MQTT client
function initMqtt() {
  // Check if Paho library is loaded
  console.log("Checking if Paho is available:", typeof Paho);
  if (typeof Paho === 'undefined' || typeof Paho.Client === 'undefined') {
    console.error("Paho MQTT library is not available");
    statusText.textContent = "MQTT library not loaded, using HTTP fallback";
    startHttpPolling();
    return;
  }
  
  // Create a client instance
  console.log("Attempting to connect to MQTT broker: " + mqttBroker);
  console.log("Client ID: " + clientId);
  
  try {
    // Create MQTT client - using correct constructor format
    if (typeof Paho.Client !== 'undefined') {
      mqttClient = new Paho.Client(mqttBroker, clientId);
    } else {
      console.error("Paho.Client is not available");
      statusText.textContent = "MQTT client not available, using HTTP fallback";
      startHttpPolling();
      return;
    }
    
    // Set callback handlers
    mqttClient.onConnectionLost = onConnectionLost;
    mqttClient.onMessageArrived = onMessageArrived;
    
    // Connect the client with more specific options
    const connectOptions = {
      onSuccess: onConnect,
      onFailure: onConnectFailure,
      useSSL: false, // Don't use SSL for unsecure WebSocket
      timeout: 10,  // Connection timeout in seconds
      cleanSession: true
    };
    
    console.log("Connecting to MQTT broker with options...");
    mqttClient.connect(connectOptions);
  } catch (error) {
    console.error("Error creating MQTT client:", error);
    console.error("Error details:", error.message);
    statusText.textContent = "MQTT client error, using HTTP fallback";
    errorMessage.textContent = `MQTT client error: ${error.message}`;
    errorMessage.style.display = "block";
    startHttpPolling();
  }
  
  function onConnect() {
    // Once a connection has been made, make a subscription and send a message.
    console.log("MQTT connected successfully!");
    console.log("Connected to: " + mqttBroker);
    console.log("Client ID: " + clientId);
    statusText.textContent = "MQTT kết nối thành công";
    errorMessage.style.display = "none";
    
    // Subscribe to the sensor data topic
    try {
      console.log("Subscribing to topic: " + mqttTopic);
      const subscriptionOptions = {
        qos: 0, // Quality of service level 0
        onSuccess: function() {
          console.log("Successfully subscribed to: " + mqttTopic);
        },
        onFailure: function() {
          console.error("Failed to subscribe to: " + mqttTopic);
        }
      };
      
      mqttClient.subscribe(mqttTopic, subscriptionOptions);
    } catch (error) {
      console.error("Error subscribing to topic:", error);
      console.error("Error details:", error.message);
    }
  }
  
  function onConnectFailure(error) {
    console.log("MQTT connection failed");
    console.log("Error object:", error);
    console.log("Error message:", error ? error.errorMessage : "No error message");
    console.log("Error code:", error ? error.errorCode : "No error code");
    
    statusText.textContent = "MQTT lỗi kết nối, chuyển sang chế MODE HTTP";
    errorMessage.textContent = `MQTT connection failed: ${error ? error.errorMessage : "Unknown error"}`;
    errorMessage.style.display = "block";
    
    // Fallback to HTTP polling if MQTT fails
    startHttpPolling();
  }
  
  function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
      console.log("MQTT Connection lost:");
      console.log("Error code:", responseObject.errorCode);
      console.log("Error message:", responseObject.errorMessage || "No message");
      
      statusText.textContent = "MQTT mất kết nối";
      errorMessage.textContent = "MQTT mất kết nối, đang thử lại...";
      errorMessage.style.display = "block";
      
      // Try to reconnect after delay
      setTimeout(function() {
        console.log("Attempting to reconnect to MQTT...");
        try {
          const reconnOptions = {
            onSuccess: onConnect,
            onFailure: onConnectFailure,
            useSSL: true,
            timeout: 10,
            cleanSession: true
          };
          
          mqttClient.connect(reconnOptions);
        } catch (error) {
          console.error("Error during reconnection attempt:", error);
        }
      }, 5000);
    } else {
      console.log("MQTT Connection cleanly lost (no error code)");
    }
  }
  
  function onMessageArrived(message) {
    console.log("Message arrived!");
    console.log("Topic:", message.destinationName);
    console.log("Payload:", message.payloadString);
    
    try {
      const payload = JSON.parse(message.payloadString);
      console.log("Parsed MQTT message data:", payload);
      
      // Update dashboard with received sensor data
      updateDashboardData(payload);
      
      // Update status and last update time
      statusText.textContent = "MQTT cập nhật dữ liệu";
      lastUpdate.textContent = new Date().toLocaleString();
      errorMessage.style.display = "none";
    } catch (e) {
      console.error("Error parsing MQTT message:", e);
      console.error("Problematic message:", message.payloadString);
      console.error("Parse error:", e.message);
    }
  }
}

// Update dashboard with sensor data (from MQTT or HTTP)
function updateDashboardData(data) {
  // Update values on the dashboard
  document.getElementById("tempValue").textContent = data.temperature !== undefined ? data.temperature.toFixed(1) : '--';
  document.getElementById("humiValue").textContent = data.humidity !== undefined ? data.humidity.toFixed(1) : '--';
  document.getElementById("lightValue").textContent = data.light !== undefined ? data.light : '--';
  document.getElementById("airValue").textContent = data.air_quality !== undefined ? data.air_quality : '--';

  // Update chart
  chart.data.labels.push(new Date().toLocaleTimeString());
  chart.data.datasets[0].data.push(data.temperature !== undefined ? data.temperature : null);
  chart.data.datasets[1].data.push(data.humidity !== undefined ? data.humidity : null);
  chart.data.datasets[2].data.push(data.light !== undefined ? data.light : null);
  chart.data.datasets[3].data.push(data.air_quality !== undefined ? data.air_quality : null);

  // Limit data points to 20 for performance
  if (chart.data.labels.length > 20) {
    chart.data.labels.shift();
    chart.data.datasets.forEach(dataset => {
      dataset.data.shift();
    });
  }

  chart.update();
}

// DOM elements
const refreshBtn = document.getElementById("refreshBtn");
const statusText = document.getElementById("statusText");
const errorMessage = document.getElementById("errorMessage");
const lastUpdate = document.getElementById("lastUpdate");

// Load data function (HTTP fallback)
async function loadData() {
  try {
    statusText.textContent = "Đang tải dữ liệu qua HTTP...";
    errorMessage.style.display = "none";
    
    const response = await fetch(apiUrl);
    
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    const data = await response.json();
    
    // Update dashboard with received sensor data
    updateDashboardData(data);

    // Update status and last update time
    statusText.textContent = "HTTP cập nhật dữ liệu";
    lastUpdate.textContent = new Date().toLocaleString();
    
  } catch (err) {
    console.error("Error loading data via HTTP:", err);
    statusText.textContent = "Lỗi kết nối HTTP";
    errorMessage.textContent = `Lỗi HTTP: ${err.message || "Không thể tải dữ liệu từ ESP32"}`;
    errorMessage.style.display = "block";
  }
}

// Start HTTP polling as fallback
function startHttpPolling() {
  // Initial load
  loadData();
  // Auto-refresh every 5 seconds via HTTP if MQTT fails
  setInterval(loadData, 5000);
}

// Relay control elements
const relay1Switch = document.getElementById('relay1');
const relay2Switch = document.getElementById('relay2');
const autoModeSwitch = document.getElementById('autoMode');

// Relay control functions - using MQTT
function toggleRelay(relayId, state) {
  // First try MQTT
  if (mqttClient && mqttClient.isConnected()) {
    try {
      const controlTopic = "esp32/relays/control";
      const payload = {
        relay: relayId,
        state: state
      };
      
      const message = new Paho.MQTT.Message(JSON.stringify(payload));
      message.destinationName = controlTopic;
      mqttClient.send(message);
      
      console.log(`Relay ${relayId} command sent via MQTT: ${state ? 'ON' : 'OFF'}`);
    } catch (error) {
      console.error(`Error sending MQTT command to relay ${relayId}:`, error);
      // If MQTT fails, fallback to HTTP
      toggleRelayHttp(relayId, state);
    }
  } else {
    // Fallback to HTTP if MQTT is not connected
    toggleRelayHttp(relayId, state);
  }
}

// HTTP version of relay control as fallback
async function toggleRelayHttp(relayId, state) {
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
    console.log(`Relay ${relayId} ${state ? 'ON' : 'OFF'} (HTTP):`, result);
  } catch (error) {
    console.error(`Error controlling relay ${relayId} via HTTP:`, error);
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

// Manual refresh button event - allows manual refresh via HTTP
refreshBtn.addEventListener("click", function() {
  if (mqttClient && mqttClient.isConnected()) {
    statusText.textContent = "MQTT đang hoạt động, không cần làm mới";
  } else {
    loadData();
  }
});

// Initialize the chart and MQTT when the page loads
document.addEventListener('DOMContentLoaded', function() {
  initChart();
  
  // Check if Paho is loaded, and if not, wait for it
  const checkPahoAndInit = function() {
    console.log("Checking Paho availability...");
    console.log("Paho object exists:", typeof Paho);
    
    if (typeof Paho !== 'undefined') {
      console.log("Paho object:", Paho);
      console.log("Paho has Client:", typeof Paho.Client);
      
      // The library might expose Client directly rather than Paho.MQTT.Client
      if (typeof Paho.Client !== 'undefined') {
        console.log("Paho.Client is available, initializing...");
        // Initialize MQTT connection
        initMqtt();
        
        // Start HTTP polling as fallback after a delay
        setTimeout(function() {
          if (!mqttClient || !mqttClient.isConnected()) {
            console.log("MQTT not connected after 5 seconds, starting HTTP polling");
            startHttpPolling();
          } else {
            console.log("MQTT is connected, no need for HTTP polling");
          }
        }, 5000);
      } else {
        console.log("Paho.Client not yet available, checking again in 500ms...");
        setTimeout(checkPahoAndInit, 500);
      }
    } else {
      console.log("Paho object not yet available, checking again in 500ms...");
      setTimeout(checkPahoAndInit, 500);
    }
  };
  
  // Wait a short time to ensure the library is fully loaded, then start checking
  setTimeout(checkPahoAndInit, 1000);
});