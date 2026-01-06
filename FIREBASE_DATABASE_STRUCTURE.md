# Firebase Realtime Database Structure for ESP32 to Dashboard

## Overview
Simple Firebase RTDB structure for ESP32 sensor data transmission to dashboard.

## Data Format

### Sensor Data Path
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

### Data Fields
- `temperature`: Temperature reading in Celsius
- `humidity`: Humidity percentage
- `light`: Light level (0-100)
- `airQuality`: Air quality reading
- `timestamp`: Unix timestamp (milliseconds)
- `device_id`: ESP32 device identifier (MAC address)

### Device ID Generation
- Uses ESP32 WiFi MAC address as unique device identifier
- Format: "24:0A:C4:F1:D2:A6" (example)

## Implementation

### ESP32 Side
- Publishes sensor data to `/sensors/{deviceId}/data` every 5 seconds
- Automatically generates device_id from MAC address
- Updates timestamp with each data transmission

### Dashboard Side
- Reads from `/sensors/{deviceId}/data` path
- Updates display in real-time as new data arrives
- Shows all sensor values with timestamp

## Example Data Flow
```
ESP32: /sensors/24:0A:C4:F1:D2:A6/data
→
Firebase RTDB:
{
  "temperature": 26.5,
  "humidity": 58.2,
  "light": 75,
  "airQuality": 50,
  "timestamp": 1634567890123,
  "device_id": "24:0A:C4:F1:D2:A6"
}
→
Dashboard: Displays live sensor readings
```