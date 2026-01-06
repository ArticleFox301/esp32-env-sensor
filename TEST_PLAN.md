# Test Plan for ESP32 → Firebase → Dashboard

## Test 1: ESP32 to Firebase Connection
- [ ] ESP32 connects to WiFi successfully
- [ ] ESP32 connects to Firebase successfully
- [ ] ESP32 publishes sensor data to Firebase every 5 seconds
- [ ] Sensor data appears in Firebase RTDB under `/sensors/[deviceId]/data`
- [ ] Data includes temperature, humidity, light, air quality, and timestamp

## Test 2: Data Format Verification
- [ ] Firebase RTDB shows correct data structure as per FIREBASE_DATABASE_STRUCTURE.md
- [ ] Each ESP32 device uses its MAC address as unique device ID
- [ ] JSON format includes all required fields: temperature, humidity, light, airQuality, timestamp, device_id

## Test 3: Dashboard Verification
- [ ] Dashboard connects to same Firebase project
- [ ] Dashboard shows live sensor data from ESP32
- [ ] Data updates in real-time on dashboard
- [ ] All sensor values (temp, hum, light, air) display correctly

## Test 4: System Health Check
- [ ] ESP32 continues sending data when WiFi/Firebase connection is stable
- [ ] Serial output shows successful data transmission messages
- [ ] No errors in ESP32 operation during testing
- [ ] Dashboard refreshes data regularly without interruption

## Success Criteria
- ESP32 sends sensor data to Firebase Realtime Database
- Data flows from ESP32 → Firebase RTDB → Dashboard
- All sensor values appear correctly on dashboard
- System operates continuously without errors