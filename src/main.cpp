#include <Arduino.h>
/* Arduino uno :
    SDA : A4,
    SCL : A5
*/

#include "MAX30105.h"
#include "heartRate.h"

// ------------------- Debug série ----------------------------
#define DEBUG

#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// ------------------------ Sensor ----------------------------
MAX30105 Sensor;

// ------------------------ Config  ---------------------------
constexpr uint8_t BASELINE_SAMPLES = 32;
constexpr uint32_t FINGER_DELTA_ON = 8000;
constexpr uint32_t FINGER_DELTA_OFF = 4000;

constexpr uint8_t BPM_WINDOW_SIZE = 7; 
constexpr uint8_t MAX_CONSECUTIVE_REJECTS = 5;

constexpr unsigned long NO_FINGER_RESET_MS = 5000;
constexpr unsigned long MIN_INTERVAL_MS = 240;
constexpr unsigned long MAX_INTERVAL_MS = 3000;

constexpr float BPM_TOLERANCE = 10.0f;  

// ------------------------ Variable  -------------------------
uint32_t IR = 0;
uint32_t irBaseline = 0;
uint32_t deltaIR = 0;

uint8_t bpmCount = 0;
uint8_t consecutiveRejects = 0;

unsigned long beat = 0;
unsigned long lastBeat = 0;
unsigned long interval = 0;
unsigned long noFingerSince = 0;

float rawBPM = 0.0f;
float displayedBPM = 0.0f;
float bpmWindow[BPM_WINDOW_SIZE];

bool isInitialized = false;
bool fingerPresent = false;

// ------------------------ Tools  ---------------------------
void calibrateIRBaseline() 
{
  uint64_t sum = 0;

  for (uint8_t i = 0; i < BASELINE_SAMPLES; i++) 
  {
    sum += Sensor.getIR();
    delay(10);
  }

  irBaseline = sum / BASELINE_SAMPLES;

  DEBUG_PRINT("IR baseline: ");
  DEBUG_PRINTLN(irBaseline);
}

void resetMeasurementState()
{
  beat = 0;
  lastBeat = 0;
  interval = 0;

  rawBPM = 0.0f;
  displayedBPM = 0.0f;

  bpmCount = 0;
  consecutiveRejects = 0;
  isInitialized = false;

  for (uint8_t i = 0; i < BPM_WINDOW_SIZE; i++)
  {
    bpmWindow[i] = 0.0f;
  }

  DEBUG_PRINTLN("Complete reset of the measurement state");
}

// ------------------------
void copyArray(const float* source, float* destination, uint8_t size)
{
  for (uint8_t i = 0; i < size; i++) 
  {
    destination[i] = source[i];
  }
}

// ------------------------
void sortArray(float* arr, uint8_t size)
{
  if (size < 2) return;

  for (uint8_t i = 0; i < size - 1; i++) 
  {
    for (uint8_t j = i + 1; j < size; j++) 
    {
      if (arr[j] < arr[i]) 
      {
        float temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
  }
}

// ------------------------
float computeMedian(const float* arr, uint8_t size)
{
  if (size == 0 || size > BPM_WINDOW_SIZE) return 0.0f;

  float temp[BPM_WINDOW_SIZE];
  copyArray(arr, temp, size);
  sortArray(temp, size);

  if (size % 2 == 1) 
  {
    return temp[size / 2];
  } 
  else 
  {
    return (temp[(size / 2) - 1] + temp[size / 2]) / 2.0f;
  }
}

// ------------------------
float computeWeightedAverage(const float* arr, uint8_t size) 
{ 
  if (size == 0 || size > BPM_WINDOW_SIZE) return 0.0f;
  if (size == 1) return arr[0];

  float weightedSum = 0.0f;
  float weightTotal = 0.0f;

  for (uint8_t i = 0; i < size; i++) 
  {
    float t = (float)i / (float)(size - 1);
    float weight = 1.0f + t * (1.7f - 1.0f);

    weightedSum += arr[i] * weight;
    weightTotal += weight;
  }

  return weightedSum / weightTotal;
}

// ------------------------
void pushBPM(float newBPM)
{
  if (bpmCount < BPM_WINDOW_SIZE) 
  {
    bpmWindow[bpmCount] = newBPM;
    bpmCount++;
  } 
  else 
  {
    for (uint8_t i = 0; i < BPM_WINDOW_SIZE - 1; i++) 
    {
      bpmWindow[i] = bpmWindow[i + 1];
    }
    bpmWindow[BPM_WINDOW_SIZE - 1] = newBPM;
  }
}

// ------------------------
void printWindow()
{
  DEBUG_PRINT("debug BPM: [");
  for (uint8_t i = 0; i < bpmCount; i++) 
  {
    DEBUG_PRINT(bpmWindow[i]);
    if (i < bpmCount - 1) DEBUG_PRINT(", ");
  }
  DEBUG_PRINTLN("]");
}

// -------------------- Setup -------------------------
void setup()
{
  Serial.begin(115200);
  DEBUG_PRINTLN("Serial initialization completed");
  
  if (Sensor.begin()) 
  {
    DEBUG_PRINTLN("Sensor is initialized");
    Sensor.setup();
    calibrateIRBaseline();
  }
  else 
  {
    DEBUG_PRINTLN("Sensor is not initialized");
    while (1);
  }

  resetMeasurementState();
}

// -------------------- Loop --------------------------
void loop()
{
  IR = Sensor.getIR();
  deltaIR = (IR > irBaseline) ? (IR - irBaseline) : 0;
  
  // 1) Finger presence management
  if (!fingerPresent)
  {
    if (deltaIR > FINGER_DELTA_ON)
    {
      DEBUG_PRINTLN("Finger detected");
      fingerPresent = true;
      noFingerSince = 0;
    }
    else
    {
      if (noFingerSince == 0)
      {
        noFingerSince = millis();
        DEBUG_PRINTLN("No finger");
      }

      if (millis() - noFingerSince >= NO_FINGER_RESET_MS)
      {
        resetMeasurementState();
        noFingerSince = millis();
      }
      delay(100);
      return;
    }
  }
  else
  {
    if (deltaIR < FINGER_DELTA_OFF)
    {
      fingerPresent = false;
      noFingerSince = millis();
      DEBUG_PRINTLN("No finger");

      delay(100);
      return;
    }
  }

  // 2) BPM detection
  if (checkForBeat(IR))
  {
    beat = millis();

    if (lastBeat != 0)
    {
      interval = beat - lastBeat;

      // 3) Raw filter
      if (interval > MIN_INTERVAL_MS && interval < MAX_INTERVAL_MS )
      {
        rawBPM = 60000.0f / interval;

        DEBUG_PRINT("IR : ");
        DEBUG_PRINT(IR);
        DEBUG_PRINT(" | interval : ");
        DEBUG_PRINT(interval);
        DEBUG_PRINT(" ms | raw BPM : ");
        DEBUG_PRINTLN(rawBPM);


        // 4) Initialisation phase
        if (!isInitialized)
        {
          pushBPM(rawBPM);

          DEBUG_PRINT("Initialisation.. : ");
          DEBUG_PRINT(bpmCount);
          DEBUG_PRINT("/");
          DEBUG_PRINTLN(BPM_WINDOW_SIZE);

          printWindow();

          if (bpmCount == BPM_WINDOW_SIZE)
          {
            isInitialized = true;
            consecutiveRejects = 0;
            displayedBPM = computeWeightedAverage(bpmWindow, bpmCount);

            DEBUG_PRINTLN("Initial window complete -> active stabilized mode");
            DEBUG_PRINT("BPM displays initial : ");
            DEBUG_PRINTLN(displayedBPM);
          }
        }

        // 5) Nominal operating conditions: median + filter
        else
        {
          float medianBPM = computeMedian(bpmWindow, BPM_WINDOW_SIZE);

          DEBUG_PRINT("Actual median : ");
          DEBUG_PRINTLN(medianBPM);

          if (rawBPM >= (medianBPM - BPM_TOLERANCE) && rawBPM <= (medianBPM + BPM_TOLERANCE)) 
          {
            pushBPM(rawBPM);
            consecutiveRejects = 0;

            displayedBPM = computeWeightedAverage(bpmWindow, BPM_WINDOW_SIZE);

            DEBUG_PRINTLN("BPM accepted");
            printWindow();

            DEBUG_PRINT("BPM display : ");
            DEBUG_PRINTLN(displayedBPM);
          }
          else
          {
            consecutiveRejects++;

            DEBUG_PRINTLN("BPM rejected (outside median ± tolerance)");
            DEBUG_PRINT("Consecutive rejections : ");
            DEBUG_PRINTLN(consecutiveRejects);

            DEBUG_PRINT("BPM displays keeps : ");
            DEBUG_PRINTLN(displayedBPM);

            if (consecutiveRejects >= MAX_CONSECUTIVE_REJECTS) 
            {
              DEBUG_PRINTLN("Too many consecutive rejections -> window reset");

              resetMeasurementState(); 
            }
          }
        }
      }
    }
    lastBeat = beat;
  }
}
