#include <Arduino.h>

constexpr uint8_t RELAY_CONTROL_PIN = 16;
constexpr uint8_t RELAY_CONTACT_PIN = 17;

constexpr uint8_t MEASUREMENTS_COUNT = 10;

constexpr uint32_t START_DELAY_MS = 2000;
constexpr uint32_t PAUSE_BETWEEN_MEASUREMENTS_MS = 1000;

constexpr bool RELAY_ACTIVE_LOW = true;

enum class TestState : uint8_t {
  START_DELAY,
  START_MEASUREMENT,
  WAIT_FOR_CONTACT,
  PAUSE_BETWEEN_MEASUREMENTS,
  FINISHED
};

volatile bool contactDetected = false;
volatile uint32_t contactTriggerTimeMs = 0;

TestState currentState = TestState::START_DELAY;

uint32_t stateStartTimeMs = 0;
uint32_t relayStartTimeMs = 0;

uint32_t measurementsMs[MEASUREMENTS_COUNT] = {};

uint8_t measurementIndex = 0;

bool resultsPrinted = false;

void IRAM_ATTR onRelayContactTriggered() {
  if (!contactDetected) {
    contactTriggerTimeMs = millis();
    contactDetected = true;
  }
}

void setRelay(const bool enabled) {
  const uint8_t signal = RELAY_ACTIVE_LOW
                           ? (enabled ? LOW : HIGH)
                           : (enabled ? HIGH : LOW);

  digitalWrite(RELAY_CONTROL_PIN, signal);
}

bool hasElapsed(
  const uint32_t startTimeMs,
  const uint32_t intervalMs
) {
  return millis() - startTimeMs >= intervalMs;
}

void printResults() {
  uint32_t sumMs = 0;

  Serial.println();
  Serial.println("Results:");

  for (uint8_t i = 0; i < MEASUREMENTS_COUNT; i++) {
    Serial.print("Measurement ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(measurementsMs[i]);
    Serial.println(" ms");

    sumMs += measurementsMs[i];
  }

  const float averageMs =
    static_cast<float>(sumMs) / MEASUREMENTS_COUNT;

  Serial.println();
  Serial.print("Average relay response time: ");
  Serial.print(averageMs, 2);
  Serial.println(" ms");
}

void setup() {
  Serial.begin(9600);

  pinMode(RELAY_CONTROL_PIN, OUTPUT);
  pinMode(RELAY_CONTACT_PIN, INPUT_PULLUP);

  setRelay(false);

  attachInterrupt(
    digitalPinToInterrupt(RELAY_CONTACT_PIN),
    onRelayContactTriggered,
    FALLING
  );

  stateStartTimeMs = millis();

  Serial.println("Relay response time test started");
  Serial.println("Waiting before first measurement...");
}

void loop() {
  switch (currentState) {
    case TestState::START_DELAY:
      if (hasElapsed(stateStartTimeMs, START_DELAY_MS)) {
        currentState = TestState::START_MEASUREMENT;
      }
      break;

    case TestState::START_MEASUREMENT:
      contactDetected = false;
      contactTriggerTimeMs = 0;

      Serial.println();
      Serial.print("Measurement ");
      Serial.print(measurementIndex + 1);
      Serial.println(" started");

      relayStartTimeMs = millis();

      setRelay(true);

      currentState = TestState::WAIT_FOR_CONTACT;
      break;

    case TestState::WAIT_FOR_CONTACT:
      if (contactDetected) {
        const uint32_t responseTimeMs =
          contactTriggerTimeMs - relayStartTimeMs;

        measurementsMs[measurementIndex] = responseTimeMs;

        Serial.print("Relay response time: ");
        Serial.print(responseTimeMs);
        Serial.println(" ms");

        measurementIndex++;

        setRelay(false);

        stateStartTimeMs = millis();

        currentState =
          TestState::PAUSE_BETWEEN_MEASUREMENTS;
      }
      break;

    case TestState::PAUSE_BETWEEN_MEASUREMENTS:
      if (hasElapsed(
            stateStartTimeMs,
            PAUSE_BETWEEN_MEASUREMENTS_MS
          )) {

        if (measurementIndex >= MEASUREMENTS_COUNT) {
          currentState = TestState::FINISHED;
        } else {
          currentState = TestState::START_MEASUREMENT;
        }
      }
      break;

    case TestState::FINISHED:
      if (!resultsPrinted) {
        printResults();
        resultsPrinted = true;
      }
      break;

    default:
      setRelay(false);
      currentState = TestState::FINISHED;
      break;
  }
}