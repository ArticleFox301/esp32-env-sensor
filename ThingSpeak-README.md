# ThingSpeak Integration Setup

## Configuration

To use ThingSpeak with your ESP32 project, you need to update the credentials in `src/thingspeak.cpp`:

1. Open `src/thingspeak.cpp`
2. Replace `YOUR_WRITE_API_KEY_HERE` with your actual ThingSpeak Write API Key
3. Replace `0000000` with your actual Channel ID

## ThingSpeak Channel Setup

1. Create a new channel on ThingSpeak.com
2. Configure at least 4 fields:
   - Field 1: Temperature
   - Field 2: Humidity
   - Field 3: Light Level
   - Field 4: Air Quality
3. Note down your Write API Key and Channel ID
4. Update the credentials in the code as mentioned above

## API Limits

The code is configured to send data to ThingSpeak every 20 seconds to respect the free API limits (15 seconds is the minimum for free accounts).

## Fields Mapping

- Field 1: Temperature (°C)
- Field 2: Humidity (%)
- Field 3: Light Level (%)
- Field 4: Air Quality (%)