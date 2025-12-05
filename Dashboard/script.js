// Configuration - This is only used for HTTP fallback which is now disabled
// Since ESP32 no longer hosts a web server, we rely solely on MQTT

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
      if (statusText) statusText.textContent = "MQTT client not available - no relay control possible";
      if (errorMessage) {
        errorMessage.textContent = "MQTT library not loaded - check if paho-mqtt.js is accessible";
        errorMessage.style.display = "block";
      }
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
      
      // Also subscribe to relay status updates
      mqttClient.subscribe("esp32/relays/status", subscriptionOptions);
    } catch (error) {
      console.error("Error subscribing to topic:", error);
      console.error("Error details:", error.message);
    }
    
    // Enable switches when MQTT is connected
    if (relay1Switch) relay1Switch.disabled = false;
    if (relay2Switch) relay2Switch.disabled = false;
    if (autoModeSwitch) autoModeSwitch.disabled = false;
  }
  
  function onConnectFailure(error) {
    console.log("MQTT connection failed");
    console.log("Error object:", error);
    console.log("Error message:", error ? error.errorMessage : "No error message");
    console.log("Error code:", error ? error.errorCode : "No error code");
    
    statusText.textContent = "MQTT lỗi kết nối - không có phương thức dự phòng";
    errorMessage.textContent = `MQTT connection failed: ${error ? error.errorMessage : "Unknown error"}`;
    errorMessage.style.display = "block";
    
    // In non-web-server mode, there is no fallback method
    console.warn("No fallback available - ESP32 does not host a web server");
  }
  
  function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
      console.log("MQTT Connection lost:");
      console.log("Error code:", responseObject.errorCode);
      console.log("Error message:", responseObject.errorMessage || "No message");
      
      statusText.textContent = "MQTT mất kết nối";
      errorMessage.textContent = "MQTT mất kết nối, đang thử lại...";
      errorMessage.style.display = "block";
      
      // Disable controls when MQTT is disconnected
      if (relay1Switch) relay1Switch.disabled = true;
      if (relay2Switch) relay2Switch.disabled = true;
      if (autoModeSwitch) autoModeSwitch.disabled = true;
      
      // Try to reconnect after delay
      setTimeout(function() {
        console.log("Attempting to reconnect to MQTT...");
        try {
          // Note: Removed useSSL: true since we're using unencrypted WebSocket
          const reconnOptions = {
            onSuccess: onConnect,
            onFailure: onConnectFailure,
            useSSL: false, // Don't use SSL for unsecure WebSocket
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
      
      // Check if this is a relay status message
      if (message.destinationName === "esp32/relays/status") {
        // Update relay switches based on status received from ESP32
        if (payload.hasOwnProperty('relay1_state') && relay1Switch) {
          relay1Switch.checked = payload.relay1_state;
        }
        if (payload.hasOwnProperty('relay2_state') && relay2Switch) {
          relay2Switch.checked = payload.relay2_state;
        }
        if (payload.hasOwnProperty('manual_mode') && autoModeSwitch) {
          autoModeSwitch.checked = !payload.manual_mode; // ESP32 sends manualMode, dashboard shows autoMode
        }
        
        if (statusText) statusText.textContent = "Cập nhật trạng thái relay từ ESP32";
        if (lastUpdate) lastUpdate.textContent = new Date().toLocaleString();
        if (errorMessage) errorMessage.style.display = "none";
      }
      // Handle sensor data message
      else if (message.destinationName === "esp32/sensors/data") {
        // Update dashboard with received sensor data
        updateDashboardData(payload);
        
        // Update status and last update time
        if (statusText) statusText.textContent = "MQTT cập nhật dữ liệu cảm biến";
        if (lastUpdate) lastUpdate.textContent = new Date().toLocaleString();
        if (errorMessage) errorMessage.style.display = "none";
      }
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
let refreshBtn = document.getElementById("refreshBtn");
let statusText = document.getElementById("statusText");
let errorMessage = document.getElementById("errorMessage");
let lastUpdate = document.getElementById("lastUpdate");

// Verify that all required DOM elements exist
if (!refreshBtn) {
  console.error("Refresh button element not found");
}
if (!statusText) {
  console.error("Status text element not found");
}
if (!errorMessage) {
  console.error("Error message element not found");
}
if (!lastUpdate) {
  console.error("Last update element not found");
}

// Load data function (HTTP fallback) - DISABLED since ESP32 no longer hosts web server
async function loadData() {
  console.warn('HTTP data loading is disabled. Using MQTT only.');
  statusText.textContent = "HTTP polling disabled, ensure MQTT is connected for sensor data";
  errorMessage.textContent = "Using MQTT only - ensure MQTT connection is active";
  errorMessage.style.display = "block";
}

// HTTP polling is disabled when ESP32 doesn't host a web server
// All communication happens via MQTT

// Relay control elements - initially disabled until MQTT is connected
let relay1Switch = document.getElementById('relay1');
let relay2Switch = document.getElementById('relay2');
let autoModeSwitch = document.getElementById('autoMode');

// Verify that all required relay switch elements exist
if (!relay1Switch) {
  console.error("Relay 1 switch element not found");
}
if (!relay2Switch) {
  console.error("Relay 2 switch element not found");
}
if (!autoModeSwitch) {
  console.error("Auto mode switch element not found");
}

// Initially disable switches until MQTT is connected
if (relay1Switch) relay1Switch.disabled = true;
if (relay2Switch) relay2Switch.disabled = true;
if (autoModeSwitch) autoModeSwitch.disabled = true;

// Relay control functions - using MQTT
function toggleRelay(relayId, state) {
  // Check if Paho MQTT library is available
  if (typeof Paho === 'undefined' || typeof Paho.Message === 'undefined') {
    console.error("Paho MQTT library not available");
    if (errorMessage) {
      errorMessage.textContent = "MQTT library not loaded - check if paho-mqtt.js is accessible";
      errorMessage.style.display = "block";
    }
    // Update switch back to previous state if it exists
    const switchElement = relayId === 1 ? relay1Switch : relay2Switch;
    if (switchElement) {
      switchElement.checked = !state;
      switchElement.disabled = true;
      setTimeout(() => {
        if (switchElement) switchElement.disabled = false;
      }, 2000);
    }
    return;
  }

  // First try MQTT
  if (mqttClient && mqttClient.isConnected()) {
    try {
      const controlTopic = "esp32/relays/control";
      const payload = {
        relay: relayId,
        state: state
      };
      
      const message = new Paho.Message(JSON.stringify(payload));
      message.destinationName = controlTopic;
      mqttClient.send(message);
      
      console.log(`Relay ${relayId} command sent via MQTT: ${state ? 'ON' : 'OFF'}`);
    } catch (error) {
      console.error(`Error sending MQTT command to relay ${relayId}:`, error);
      // In non-web-server mode, there is no fallback method
      console.warn("No fallback available - ESP32 does not host a web server");
    }
  } else {
    // In non-web-server mode, MQTT is required for relay control
    console.warn("MQTT not connected - relay control unavailable");
    if (statusText) statusText.textContent = "MQTT không kết nối - điều khiển relay không khả dụng";
    if (errorMessage) {
      errorMessage.textContent = "MQTT không kết nối - vui lòng kiểm tra kết nối MQTT";
      errorMessage.style.display = "block";
    }
    // Update switch back to previous state if it exists
    const switchElement = relayId === 1 ? relay1Switch : relay2Switch;
    if (switchElement) {
      switchElement.checked = !state;
      // Disable the switch temporarily until MQTT is reconnected
      switchElement.disabled = true;
      setTimeout(() => {
        if (switchElement) switchElement.disabled = false;
      }, 2000); // Re-enable after 2 seconds
    }
  }
}

// HTTP version of relay control as fallback - DISABLED since ESP32 no longer hosts web server
async function toggleRelayHttp(relayId, state) {
  console.warn('HTTP relay control is disabled. Using MQTT only.');
  // In non-web-server mode, we only use MQTT
  statusText.textContent = "HTTP fallback disabled, ensure MQTT is connected";
  errorMessage.textContent = "Please ensure MQTT connection is active for relay control";
  errorMessage.style.display = "block";
}

// Mode control function - MQTT only since ESP32 no longer hosts web server
function setMode(auto) {
  // Check if Paho MQTT library is available
  if (typeof Paho === 'undefined' || typeof Paho.Message === 'undefined') {
    console.error("Paho MQTT library not available");
    if (errorMessage) {
      errorMessage.textContent = "MQTT library not loaded - check if paho-mqtt.js is accessible";
      errorMessage.style.display = "block";
    }
    // Update switch back to previous state if it exists
    if (autoModeSwitch) {
      autoModeSwitch.checked = !auto;
      autoModeSwitch.disabled = true;
      setTimeout(() => {
        if (autoModeSwitch) autoModeSwitch.disabled = false;
      }, 2000);
    }
    return;
  }

  // First try MQTT
  if (mqttClient && mqttClient.isConnected()) {
    try {
      const controlTopic = "esp32/relays/control";
      const payload = {
        mode: auto ? 'auto' : 'manual'
      };
      
      const message = new Paho.Message(JSON.stringify(payload));
      message.destinationName = controlTopic;
      mqttClient.send(message);
      
      console.log(`Mode set to ${auto ? 'auto' : 'manual'} via MQTT`);
    } catch (error) {
      console.error(`Error sending MQTT mode command:`, error);
    }
  } else {
    console.warn("MQTT not connected, cannot set mode");
    if (statusText) statusText.textContent = "MQTT không kết nối - không thể đặt chế độ";
    if (errorMessage) {
      errorMessage.textContent = "MQTT không kết nối - vui lòng kiểm tra kết nối MQTT";
      errorMessage.style.display = "block";
    }
    // Update switch back to previous state if it exists
    if (autoModeSwitch) {
      autoModeSwitch.checked = !auto;
      // Disable the switch temporarily until MQTT is reconnected
      autoModeSwitch.disabled = true;
      setTimeout(() => {
        if (autoModeSwitch) autoModeSwitch.disabled = false;
      }, 2000); // Re-enable after 2 seconds
    }
  }
}

// Event listeners for relay switches - with null checks
if (relay1Switch) {
  relay1Switch.addEventListener('change', function() {
    toggleRelay(1, this.checked);
  });
} else {
  console.error("Could not find relay1 switch element");
}

if (relay2Switch) {
  relay2Switch.addEventListener('change', function() {
    toggleRelay(2, this.checked);
  });
} else {
  console.error("Could not find relay2 switch element");
}

// Event listener for mode switch - with null check
if (autoModeSwitch) {
  autoModeSwitch.addEventListener('change', function() {
    setMode(this.checked);
  });
} else {
  console.error("Could not find autoMode switch element");
}

// Manual refresh button event - in MQTT-only mode, this sends a status request
if (refreshBtn) {
  refreshBtn.addEventListener("click", function() {
    // Check if Paho MQTT library is available
    if (typeof Paho === 'undefined' || typeof Paho.Message === 'undefined') {
      console.error("Paho MQTT library not available");
      if (errorMessage) {
        errorMessage.textContent = "MQTT library not loaded - check if paho-mqtt.js is accessible";
        errorMessage.style.display = "block";
      }
      if (statusText) statusText.textContent = "Không thể gửi yêu cầu - MQTT library không khả dụng";
      return;
    }

    if (mqttClient && mqttClient.isConnected()) {
      // Request current status from ESP32 via MQTT
      const statusTopic = "esp32/relays/control";
      const payload = {
        request: "status"
      };
      
      const message = new Paho.Message(JSON.stringify(payload));
      message.destinationName = statusTopic;
      mqttClient.send(message);
      
      if (statusText) statusText.textContent = "Yêu cầu trạng thái được gửi qua MQTT";
    } else {
      if (statusText) statusText.textContent = "MQTT không kết nối - không thể làm mới dữ liệu";
    }
  });
} else {
  console.error("Could not find refresh button element");
}

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
        
        // In non-web-server mode, MQTT is the only communication method
        // We don't start HTTP polling since ESP32 doesn't host a web server
        console.log("MQTT is the only communication method - no HTTP fallback available");
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