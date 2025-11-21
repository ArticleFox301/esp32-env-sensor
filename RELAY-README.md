# Relay Module Connection Guide

## Hardware Connection

### For 2x 5V Relay Modules to ESP32:

#### Relay Module 1 (Relay 1):
- **VCC** → 5V (from external power source or ESP32 5V pin if available)
- **GND** → GND (common ground with ESP32)
- **IN1** → GPIO 26 (as defined in pins.h)

#### Relay Module 2 (Relay 2):
- **VCC** → 5V (from external power source or ESP32 5V pin if available)
- **GND** → GND (common ground with ESP32)
- **IN2** → GPIO 27 (as defined in pins.h)

### Important Notes:
- If using 5V relays with 3.3V ESP32, make sure the relay module has a built-in level shifter
- Most relay modules have optocouplers that allow 3.3V logic level control
- The ESP32 GPIO pins output 3.3V but are typically 5V tolerant
- Connect all grounds (ESP32, relay modules, and controlled devices) together
- If controlling high-voltage devices, ensure proper electrical safety measures

## Software Usage

### Web API Endpoints:

1. **Get Current Relay States:**
   - `GET /relay`
   - Response: Current states of both relays

2. **Control Individual Relay:**
   - `GET /relay?relay=1&action=on` - Turn on Relay 1
   - `GET /relay?relay=1&action=off` - Turn off Relay 1
   - `GET /relay?relay=1&action=toggle` - Toggle Relay 1
   - `GET /relay?relay=2&action=on` - Turn on Relay 2
   - `GET /relay?relay=2&action=off` - Turn off Relay 2
   - `GET /relay?relay=2&action=toggle` - Toggle Relay 2

3. **Control Both Relays:**
   - `GET /relay/both?relay1=on&relay2=off` - Set specific states for both relays

### Example:
- To turn on Relay 1: `http://[ESP32-IP]/relay?relay=1&action=on`
- To turn off Relay 2: `http://[ESP32-IP]/relay?relay=2&action=off`
- To toggle Relay 1: `http://[ESP32-IP]/relay?relay=1&action=toggle`
- To set both relays: `http://[ESP32-IP]/relay/both?relay1=on&relay2=off`

## Code Usage in Arduino Sketch:

```cpp
// Initialize relays (done automatically in setup())
relay_init();

// Control relays
relay1_on();      // Turn on relay 1
relay1_off();     // Turn off relay 1
relay1_toggle();  // Toggle relay 1
relay2_on();      // Turn on relay 2
relay2_off();     // Turn off relay 2
relay2_toggle();  // Toggle relay 2

// Check relay states
bool state1 = relay1_state();  // Get current state of relay 1
bool state2 = relay2_state();  // Get current state of relay 2

// Set both relays at once
relay_set_states(true, false);  // Relay 1 ON, Relay 2 OFF
```

## Safety Precautions:
- Always disconnect power before making connections
- Ensure proper isolation when controlling high-voltage equipment
- Use appropriate fuses/circuit protection
- Verify voltage ratings of all components
- Consider using external 5V power supply for relays if controlling high-power devices