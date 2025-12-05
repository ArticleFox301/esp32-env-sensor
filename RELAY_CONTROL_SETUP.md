# ESP32 Relay Control via MQTT Dashboard

This document explains how to set up ESP32 to receive relay control signals from a web dashboard via MQTT.

## System Overview

The system uses MQTT protocol for communication between the web dashboard and ESP32:
- **Dashboard**: Runs web interface and sends MQTT commands
- **ESP32**: Listens for MQTT commands and controls relays

## Hardware Setup

1. Connect your relays to the following GPIO pins:
   - Relay 1: GPIO 26
   - Relay 2: GPIO 27
   - VCC and GND to your power source
   - IN pins to the corresponding GPIO pins

## Software Setup

### ESP32 Side

1. Install required libraries in your PlatformIO project:
   - `PubSubClient` for MQTT communication
   - `ArduinoJson` for JSON parsing
   - `WiFi` for network connectivity

2. The ESP32 is configured to connect to your WiFi network with the credentials in `main.cpp`:
   ```cpp
   const char* ssid = "P 2908";
   const char* password = "@dimuonnoi@";
   ```

3. The ESP32 will:
   - Connect to WiFi
   - Connect to MQTT broker (HiveMQ public broker by default)
   - Subscribe to the relay control topic: `esp32/relays/control`
   - Initialize relay pins as outputs
   - Publish sensor data to `esp32/sensors/data`
   - Publish relay status to `esp32/relays/status`

### Dashboard Side

1. Deploy the dashboard files to a web server or run locally in a browser
2. Update the MQTT broker configuration in `Dashboard/script.js`:
   ```javascript
   const mqttBroker = "ws://broker.hivemq.com:8000/mqtt";  // For public broker
   // Or for local broker:
   // const mqttBroker = "ws://[your-mqtt-server-ip]:9001/mqtt";
   ```

## Communication Protocol

The ESP32 and dashboard communicate using MQTT messages over specific topics:

### MQTT Topics Explained

- **`esp32/sensors/data`** - Sensor data channel
  - ESP32 publishes sensor readings (temperature, humidity, light, air quality)
  - Dashboard subscribes to receive and display sensor data

- **`esp32/relays/control`** - Control channel  
  - Dashboard publishes control commands (relay ON/OFF, mode changes)
  - ESP32 subscribes to receive and execute these commands

- **`esp32/relays/status`** - Status channel
  - ESP32 publishes current relay states and operation mode
  - Dashboard subscribes to update the UI with current status

- **`esp32/system/status`** - Connection status (not currently used in this implementation)

### MQTT Message Formats

**Relay Control Command:**
```json
{
  "relay": 1,      // Relay number (1 or 2)
  "state": true    // true = ON, false = OFF
}
```

**Mode Control Command:**
```json
{
  "mode": "auto"   // "auto" = automatic mode, "manual" = manual mode
}
```

**Status Request (sent to control topic):**
```json
{
  "request": "status"  // Requests ESP32 to publish current status
}
```

**Relay Status Message (published by ESP32):**
```json
{
  "relay1_state": true,    // Current state of relay 1 (true=ON, false=OFF)
  "relay2_state": false,   // Current state of relay 2 (true=ON, false=OFF)
  "manual_mode": true,     // Operation mode (true=manual, false=auto)
  "timestamp": 1234567890, // Time in milliseconds since boot
  "device_id": "xx:xx:xx:xx:xx:xx"  // ESP32 MAC address
}
```

**Sensor Data Message (published by ESP32):**
```json
{
  "temperature": 25.6,     // Temperature in Celsius
  "humidity": 65.2,        // Humidity in %
  "light": 85,             // Light level (0-100)
  "air_quality": 42,       // Air quality reading
  "timestamp": 1234567890, // Time in milliseconds since boot
  "device_id": "xx:xx:xx:xx:xx:xx"  // ESP32 MAC address
}
```

## Operation Modes

The system supports two operation modes:

1. **Manual Mode** (default): Relays are controlled only via dashboard
2. **Auto Mode**: Relays are controlled automatically based on sensor readings
   - Relay 1: Turns ON when air quality > 70%
   - Relay 2: Turns ON when light level < 20%

## Dashboard Controls

1. **Relay 1/2 Toggle Switches**: Control individual relays via MQTT
2. **Auto Mode Switch**: Enable/disable automatic relay control based on sensors

## MQTT Broker Options

### Option 1: Public HiveMQ Broker (Simple)
- Uses HiveMQ public broker (no setup required)
- Less reliable for production
- WebSocket port: 8000

### Option 2: Local MQTT Broker (Recommended)
- Install Mosquitto or EMQX on your network
- More reliable and secure
- Better performance
- Example WebSocket endpoint: `ws://[broker-ip]:9001/mqtt`

## Troubleshooting

1. **Relays not responding**:
   - Check that ESP32 and dashboard can connect to the same MQTT broker
   - Verify MQTT broker is accessible from both devices
   - Check MQTT topic names match between ESP32 and dashboard

2. **MQTT connection issues**:
   - Verify network connectivity to MQTT broker
   - Check firewall settings if using local broker
   - Confirm WebSocket ports are open

3. **Relay state not updating on dashboard**:
   - Check that dashboard is subscribed to `esp32/relays/status` topic
   - Verify ESP32 is publishing status updates

4. **Sensor data not showing**:
   - Confirm dashboard is subscribed to `esp32/sensors/data` topic
   - Check ESP32 is publishing sensor data

## Customization

You can modify the automatic relay control logic in `main.cpp` in the main loop section to implement your own sensor-based control rules.

For local MQTT broker setup, install Mosquitto:
```bash
sudo apt install mosquitto mosquitto-clients  # On Debian/Ubuntu
# Configure WebSocket support on port 9001
```