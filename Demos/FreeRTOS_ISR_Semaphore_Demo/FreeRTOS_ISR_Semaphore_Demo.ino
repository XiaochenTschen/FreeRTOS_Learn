// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint16_t timer_divider = 80; // Clock ticks at 1 MHz now
static const uint64_t timer_max_count = 1000000;
static const TickType_t task_delay = 2000 / portTICK_PERIOD_MS;

#define LED_BUILTIN 22;
static const int led_pin = LED_BUILTIN;

// Pins
static const int adc_pin = A0;

// Globals
static hw_timer_t *timer = NULL;
static volatile uint16_t val;
static SemaphoreHandle_t bin_sem = NULL;

//*****************************************************************************
// ISRs

// This function executes when timer reached max (and reset)
void IRAM_ATTR onTimer() {

  BaseType_t task_woken = pdFALSE;

  // Perform action (read from ADC)
  val = analogRead(adc_pin);

  // Give semaphore to tell task that new value is ready
  xSemaphoreGiveFromISR(bin_sem, &task_woken);

  // Exit from ISR
  if(task_woken) {
    portYIELD_FROM_ISR();
  }
}

//*****************************************************************************
// Task

// Wait for semaphore and print out ADC value when received
void printValues(void *parameters) {

  while(1) {
    xSemaphoreTake(bin_sem, portMAX_DELAY);
    Serial.println(val);
  }
}


//*****************************************************************************
// Main

void setup() {

  // Configure Serial
  Serial.begin(115200);

  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS ISR Critical Section Demo---");

  // Create semaphore before it is used
  bin_sem = xSemaphoreCreateBinary();

  // Force reboot if we can't create the semaphore
  if(bin_sem == NULL) {
    Serial.println("Could not create semaphore");
    ESP.restart();
  }
  
  // Start command line interface (CLI) task
  xTaskCreatePinnedToCore(printValues,
                          "Print Values",
                          1024,
                          NULL,
                          2,
                          NULL,
                          app_cpu);
                          

  // Create and start timer (num, divider, countUp)
  timer = timerBegin(0, timer_divider, true);

  // Provide ISR to timer (timer, function, edge)
  timerAttachInterrupt(timer, &onTimer, true);

  // At what count should ISR trigger (timer, count, autoload)
  timerAlarmWrite(timer, timer_max_count, true);

  // Allow ISR to trigger
  timerAlarmEnable(timer);
}

void loop() {
  // put your main code here, to run repeatedly:

}
