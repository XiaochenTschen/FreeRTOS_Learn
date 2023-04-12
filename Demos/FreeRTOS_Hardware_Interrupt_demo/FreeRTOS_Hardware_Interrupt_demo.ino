// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint16_t timer_divider = 8; // Clock ticks at 1 MHz now
static const uint64_t timer_max_count = 1000000;
static const TickType_t task_delay = 2000 / portTICK_PERIOD_MS;

#define LED_BUILTIN 22;
static const int led_pin = LED_BUILTIN;

// Globals
static hw_timer_t *timer = NULL;
static volatile int isr_counter;
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

//*****************************************************************************
// ISRs

// This function executes when timer reached max (and reset)
void IRAM_ATTR onTimer() {

  // ESP-IDF version of a critical section (in an ISR)
  portENTER_CRITICAL_ISR(&spinlock);
  isr_counter++;
  portEXIT_CRITICAL_ISR(&spinlock);
}

//*****************************************************************************
// Task

// Wait for semaphore and print out ADC value when received
void printValues(void *parameters) {

  while(1) {

    // count down and print out counter value
    while (isr_counter > 0) {

      // Print value of counter
      Serial.println(isr_counter);

      // ESP-IDF version of a critical section (in a task)
      portENTER_CRITICAL_ISR(&spinlock);
      isr_counter--;
      portEXIT_CRITICAL_ISR(&spinlock);
    }

    // Wait 2 seconds while ISR increments counter a few times
    vTaskDelay(task_delay);
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

  // Start command line interface (CLI) task
  xTaskCreatePinnedToCore(printValues,
                          "Print Values",
                          1024,
                          NULL,
                          1,
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

  // Delete "setup and loop" task
  vTaskDelete(NULL); 

}

void loop() {
  // put your main code here, to run repeatedly:

}
