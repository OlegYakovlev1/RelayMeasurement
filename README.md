# ESP32 Relay Response Time Measurement

This project measures the response time of a mechanical relay using an ESP32, interrupts, and `millis()` timing.

The ESP32 activates a relay and detects when the relay contact physically switches state.  
The measured response time is printed to the Serial Monitor.

---

## Features

- ESP32-based relay control
- Relay contact state detection
- Interrupt-based signal handling
- Non-blocking state machine implementation
- Timing measurements using `millis()`
- Automatic average calculation after 10 measurements

---

## Hardware Requirements

- ESP32 development board
- Relay module (JQC-3FF-S-Z 5V)
- External 5V power supply
- Breadboard and jumper wires

---

## Wiring

### Relay control

| ESP32 | Relay Module |
|---|---|
| GPIO16 | IN |
| GND | GND |
| 5V PSU | VCC |

### Relay contact detection

| ESP32 | Relay Contact |
|---|---|
| GND | COM |
| GPIO17 | NO |

GPIO17 uses `INPUT_PULLUP`.

When the relay is activated, the NO contact connects GPIO17 to GND, generating a falling edge interrupt.

---

## Serial Monitor

```text
Baud rate: 9600
```

Example output:

```text
Measurement 1: 17 ms
Measurement 2: 17 ms
Measurement 3: 17 ms

Average relay response time: 17.00 ms
```

---

## Notes

- The measured values may appear identical because `millis()` has a resolution of 1 ms.
- For more precise timing measurements, `micros()` can be used instead.
- Typical mechanical relay response time is usually between 5 ms and 20 ms.

---

## Technologies Used

- ESP32
- Arduino Framework
- Interrupts
- State Machine Architecture
- `millis()`
- GPIO Input Pull-up
