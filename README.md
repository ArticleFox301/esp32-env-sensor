# ESP32 to Firebase Dashboard - IoT Monitoring System

## Project Overview

This project implements an IoT monitoring system using ESP32 microcontroller to collect sensor data and transmit it to a Firebase Realtime Database. A web-based dashboard displays the real-time sensor data and allows for remote control of relay modules connected to the ESP32.

The system includes:
- ESP32 microcontroller with connected sensors (temperature, humidity, light, air quality)
- Firebase Realtime Database for data storage and retrieval
- Web-based dashboard for real-time monitoring and control
- MQTT communication for real-time updates and relay control
- Auto/manual relay control modes

## System Architecture

```
[ESP32 Board]
├── Sensors: DHT22 (Temperature/Humidity), Light Sensor, Air Quality Sensor
├── Relays: 2 relay modules for external device control
├── Communication: WiFi to Firebase Realtime Database and MQTT Broker
└── Data Transmission: Every 5 seconds to Firebase, Real-time via MQTT

[Firebase Realtime Database]
├── Storage Path: /sensors/{deviceId}/data
├── Data Format: JSON with temperature, humidity, light, air quality, timestamp
└── Device Identification: MAC address of ESP32

[Web Dashboard]
├── Real-time Data Display: Temperature, humidity, light, air quality
├── Chart Visualization: Historical data trends
├── Relay Control: Toggle relay modules remotely (Manual/Auto modes)
└── Communication: MQTT over WebSocket
```

## Database Structure (Firebase Realtime Database)

### 1. Users Collection
```
/users: {
  userId: {
    email: "user@example.com",
    displayName: "John Doe",
    createdAt: 1634567890123,
    lastLoginAt: 1634567890123,
    rooms: {
      roomId: true
    }
  }
}
```

### 2. Rooms Collection
```
/rooms: {
  roomId: {
    name: "Classroom M.503",
    description: "Main classroom with IoT sensors",
    icon: "school",
    devices: {
      deviceId: true
    },
    createdAt: 1634567890123,
    createdBy: "userId"
  }
}
```

### 3. Devices Collection
```
/devices: {
  deviceId: {
    name: "ESP32-Sensor-001",
    location: "Classroom M.503",
    macAddress: "24:0A:C4:F1:D2:A6",
    ipAddress: "192.168.1.100",
    status: "online",
    lastSeen: 1634567890123,
    roomId: "roomId",
    firmwareVersion: "1.0.0",
    sensors: {
      temperature: true,
      humidity: true,
      light: true,
      airQuality: true
    },
    relays: {
      relay1: true,
      relay2: true
    }
  }
}
```

### 4. Sensors Collection
```
/sensors: {
  sensorId: {
    deviceId: "deviceId",
    type: "DHT22",
    pin: 15,
    calibrationOffset: 0.5,
    lastReading: {
      temperature: 25.6,
      humidity: 60.2,
      light: 85,
      airQuality: 45,
      timestamp: 1634567890123,
      batteryLevel: 80
    }
  }
}
```

### 5. Sensor History
```
/sensor_history: {
  sensorId: {
    "YYYY-MM-DD": {
      "HH:mm:ss": {
        temperature: 25.6,
        humidity: 60.2,
        light: 85,
        airQuality: 45,
        timestamp: 1634567890123
      }
    }
  }
}
```

### 6. Relays Collection
```
/relays: {
  relayId: {
    deviceId: "deviceId",
    relayNumber: 1,
    name: "AC Unit",
    pin: 26,
    currentState: true,
    targetState: true,
    lastChanged: 1634567890123,
    changeReason: "manual",
    autoRules: {
      ruleId: {
        condition: "temperature > 27",
        action: "turnOn",
        enabled: true,
        priority: 1
      }
    }
  }
}
```

### Current Implementation Note

While the database structure above represents the planned comprehensive schema, the current ESP32 code implementation uses a simpler structure at:
```
/sensors/{deviceId}/data: {
  temperature: 25.6,
  humidity: 60.2,
  light: 85,
  airQuality: 45,
  timestamp: 1634567890123,
  device_id: "ESP32-MAC-ADDRESS"
}
```

Future updates to the ESP32 code will migrate to the more comprehensive schema for better data organization and management.

## Components and Hardware

### ESP32 Board
- WiFi connectivity
- GPIO pins for sensors and relays
- Microcontroller running the sensor monitoring code

### Sensors
- DHT22: Temperature and humidity sensor
- Light sensor: Ambient light detection
- Air quality sensor: Air quality measurement

### Relays
- 2 relay modules for controlling external devices
- Manual/Auto control modes
- Auto mode logic:
  - If temperature > 27°C, turn on relay 1
  - If light < 40, turn on relay 2
  - Otherwise, both relays turn off

### Communication
- WiFi for internet connectivity
- Firebase Realtime Database for data storage
- MQTT for real-time communication

## Setup Instructions

### Prerequisites

- ESP32 development board
- PlatformIO IDE
- Firebase project with Realtime Database enabled
- Web browser for dashboard access

### Step 1: Configure Firebase

1. In Firebase Console, enable Realtime Database
2. Note your Database URL (format: `https://your-project-default-rtdb.region.firebasedatabase.app/`)
3. Get your database secret from Firebase Project Settings
4. Update security rules to allow read/write for testing:
```json
{
  "rules": {
    ".read": "auth != null",
    ".write": "auth != null"
  }
}
```

### Step 2: Configure ESP32 Code

1. Edit `src/config.h`:
   - Set `FIREBASE_HOST` to your database URL
   - Set `FIREBASE_AUTH` to your database secret
   - Set WiFi credentials (SSID and password)
   - Verify sensor pin assignments in `src/pins.h`

### Step 3: Upload to ESP32

1. Connect ESP32 via USB
2. In PlatformIO, click "Upload"
3. Open Serial Monitor (115200 baud)
4. Confirm "Firebase initialized" and "Data sent to Firebase successfully" messages appear

### Step 4: Dashboard Setup

1. Update `Dashboard/firebase-config.js` with your Firebase project config
2. Deploy dashboard files to web server or serve locally:
   ```bash
   cd Dashboard
   python -m http.server 8000
   # Visit http://localhost:8000
   ```

## Usage Instructions

### ESP32 Operation

- The ESP32 automatically connects to WiFi and Firebase
- Sensor readings are taken every 5 seconds
- Data is sent to Firebase RTDB every 5 seconds
- Data is also sent via MQTT for real-time dashboard updates
- Relays can be controlled via dashboard or automatic mode

### Dashboard Features

- Real-time sensor data display
- Historical data visualization with charts
- Remote relay control (Manual/Auto modes)
- Status indicators for connection and data updates
- Refresh button to request current status

### Auto/Manual Mode

- **Manual Mode**: Relays can be toggled manually via dashboard
- **Auto Mode**: Relays are controlled automatically based on sensor thresholds:
  - Temperature > 27°C → Relay 1 ON
  - Light level < 40 → Relay 2 ON

## Testing Procedures

### Test 1: ESP32 to Firebase Connection
- [ ] ESP32 connects to WiFi successfully
- [ ] ESP32 connects to Firebase successfully
- [ ] ESP32 publishes sensor data to Firebase every 5 seconds
- [ ] Data appears in Firebase RTDB under `/sensors/[MAC-ADDRESS]/data`
- [ ] Data includes temperature, humidity, light, air quality, and timestamp

### Test 2: Data Format Verification
- [ ] Firebase RTDB shows correct data structure
- [ ] Each ESP32 device uses its MAC address as unique device ID
- [ ] JSON format includes all required fields

### Test 3: Dashboard Verification
- [ ] Dashboard connects to same Firebase project
- [ ] Dashboard shows live sensor data from ESP32
- [ ] Data updates in real-time on dashboard
- [ ] All sensor values (temp, hum, light, air) display correctly

### Test 4: System Health Check
- [ ] ESP32 continues sending data when WiFi/Firebase connection is stable
- [ ] Serial output shows successful data transmission messages
- [ ] No errors in ESP32 operation during testing
- [ ] Dashboard refreshes data regularly without interruption

## Configuration Options

### Update Intervals
- `SENSOR_UPDATE_INTERVAL`: 5000ms (5 seconds) - How often sensor data is read
- `THINGSPEAK_UPDATE_INTERVAL`: 20000ms (20 seconds) - How often data is sent to ThingSpeak

### Sensor Thresholds
- `TEMP_THRESHOLD_HIGH`: 30.0°C - High temperature threshold
- `TEMP_THRESHOLD_LOW`: 18.0°C - Low temperature threshold
- `HUMIDITY_THRESHOLD_HIGH`: 70.0% - High humidity threshold
- `LIGHT_THRESHOLD_LOW`: 30 - Low light threshold

### Device Configuration
- `DEVICE_NAME`: "ESP32-Sensor-001" - Name identifier
- `DEVICE_LOCATION`: "Classroom M.503" - Location identifier

## Security Considerations

- Firebase API keys and authentication tokens are defined in `src/config.h`
- For production use, consider using Firebase Service Account authentication instead of legacy tokens
- Keep private keys secure and never commit them to version control
- Use environment variables or separate secrets files for sensitive information

## Troubleshooting

### Common Issues
1. **ESP32 not connecting to WiFi**:
   - Verify WiFi credentials in `config.h`
   - Check if the WiFi network is available

2. **No data in Firebase**:
   - Check Firebase configuration in `config.h`
   - Verify database security rules allow read/write
   - Ensure the ESP32 has a valid internet connection

3. **Dashboard not showing data**:
   - Verify Firebase configuration in `firebase-config.js`
   - Check MQTT connection to the broker
   - Confirm ESP32 is properly sending data to Firebase

4. **Relay control not working**:
   - Verify MQTT connection between dashboard and ESP32
   - Check if relay pins are properly configured in `pins.h`

## Success Criteria

- ✅ ESP32 sends sensor data to Firebase Realtime Database
- ✅ Data flows from ESP32 → Firebase RTDB → Dashboard
- ✅ All sensor values appear correctly on dashboard
- ✅ System operates continuously without errors
- ✅ Dashboard allows remote control of relays
- ✅ Auto mode functions according to defined thresholds

## Technologies Used

- **ESP32**: Microcontroller for sensor reading and data transmission
- **Firebase Realtime Database**: Cloud-based NoSQL database
- **MQTT**: Message protocol for real-time communication
- **PlatformIO**: Development environment for ESP32
- **Chart.js**: Client-side chart visualization
- **Paho MQTT**: JavaScript MQTT client library
- **HTML/CSS/JavaScript**: Dashboard implementation