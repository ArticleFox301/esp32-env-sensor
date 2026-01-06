#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials
#define WIFI_SSID "P 2908"
#define WIFI_PASSWORD "@dimuonnoi@"

// Firebase configuration - REPLACE WITH YOUR OWN CONFIG
#define FIREBASE_HOST "https://iot-classroom-monitor-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "19aFBGNz5LZ8Lw8so1RVlJkIQTVmzleLo9FRLLD5"
#define FIREBASE_API_KEY "AIzaSyAsMj5v3FFUK5F-YVN6tpvUorFbb_HqRDQ"  // Legacy key, for compatibility

/*
 * Secure Service Account Authentication (Recommended for Production)
 *
 * To use secure authentication:
 * 1. Go to Firebase Console -> Project Settings -> Service Accounts
 * 2. Click "Generate new private key"
 * 3. Copy the client_email and private_key values
 * 4. Uncomment and set the following defines:
 *
 * #define FIREBASE_CLIENT_EMAIL "your-service-account-email@your-project.iam.gserviceaccount.com"
 * #define FIREBASE_PRIVATE_KEY "-----BEGIN PRIVATE KEY-----\nYOUR_PRIVATE_KEY_HERE\n-----END PRIVATE KEY-----\n"
 *
 * Note: Keep your private key secure and never commit it to version control.
 * Consider using environment variables or a separate secrets file that is git-ignored.
 */

// Uncomment the following lines and add your service account credentials for production use
// #define FIREBASE_CLIENT_EMAIL "your-service-account-email@your-project.iam.gserviceaccount.com"
// #define FIREBASE_PRIVATE_KEY "-----BEGIN PRIVATE KEY-----\n...\n-----END PRIVATE KEY-----\n"

// For development environments, you can continue using legacy token authentication
// but service account authentication is strongly recommended for production
#define USE_SERVICE_ACCOUNT_AUTH 0  // Set to 1 to use service account, 0 for legacy token

// Device configuration
#define DEVICE_ID WiFi.macAddress()

// Update intervals (in milliseconds)
#define SENSOR_UPDATE_INTERVAL 5000    // 5 seconds
#define THINGSPEAK_UPDATE_INTERVAL 20000 // 20 seconds

// Sensor thresholds
#define TEMP_THRESHOLD_HIGH 30.0
#define TEMP_THRESHOLD_LOW 18.0
#define HUMIDITY_THRESHOLD_HIGH 70.0
#define LIGHT_THRESHOLD_LOW 30

// Device location and name
#define DEVICE_NAME "ESP32-Sensor-001"
#define DEVICE_LOCATION "Classroom M.503"
#define DEVICE_ROOM_ID "classroom-M503"  // The room ID this device belongs to

#endif