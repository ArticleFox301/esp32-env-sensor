# ESP32 to Firebase Dashboard - Running Guide

## Prerequisites
- ESP32 development board
- PlatformIO IDE
- Firebase project with Realtime Database enabled

## Step 1: Configure Firebase
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

## Step 2: Configure ESP32 Code
1. Edit `src/config.h`:
   - Set `FIREBASE_HOST` to your database URL
   - Set `FIREBASE_AUTH` to your database secret
   - Set WiFi credentials (SSID and password)
   - Verify sensor pin assignments in `src/pins.h`

## Step 3: Upload to ESP32
1. Connect ESP32 via USB
2. In PlatformIO, click "Upload"
3. Open Serial Monitor (115200 baud)
4. Confirm "Firebase initialized" and "Data sent to Firebase successfully" messages appear

## Step 4: Verify Data Flow
1. In Firebase Console > Realtime Database, check `/sensors/[MAC-ADDRESS]/data` path
2. Data should update every 5 seconds with temperature, humidity, light, air quality
3. Verify data structure matches `FIREBASE_DATABASE_STRUCTURE.md`

## Step 5: Dashboard Setup
1. Update `Dashboard/firebase-config.js` with your Firebase project config
2. Deploy dashboard files to web server or serve locally:
   ```bash
   cd Dashboard
   python -m http.server 8000
   # Visit http://localhost:8000
   ```

## Step 6: Complete System Test
- ESP32 sends sensor data to Firebase RTDB every 5 seconds
- Dashboard displays real-time data from Firebase
- System operates continuously without errors

## Expected Results
- ✅ ESP32 uploads sensor data to Firebase every 5 seconds
- ✅ Data appears under `/sensors/[device-id]/data` in Firebase RTDB
- ✅ Dashboard shows live sensor readings matching ESP32 measurements
- ✅ Data format follows structure defined in FIREBASE_DATABASE_STRUCTURE.md