// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Globals
static TimerHandle_t one_shot_timer = NULL;
static TimerHandle_t auto_reload_timer = NULL;

//*****************************************************************************
// Callbacks

// Called when one of the timers expires
void myTimerCallback(TimerHandle_t xTimer) {
  
  // Print message if timer 0 expired
  if((uint32_t)pvTimerGetTimerID(xTimer) == 0) {
    Serial.println("One-shot timer expired");
  }

  // Print message if timer 1 expired
  if((uint32_t)pvTimerGetTimerID(xTimer) == 1) {
    Serial.println("Auto-reload timer expired");
  }
}

//*****************************************************************************
// Main

void setup() {
  // Configure Serial
  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Timer Demo---");

  // Create a one-shot timer
  one_shot_timer = xTimerCreate(
                                "One-shot timer",
                                2000 / portTICK_PERIOD_MS,
                                pdFALSE,
                                (void *)0,
                                myTimerCallback);
  // Create a auto-reload timer
  auto_reload_timer = xTimerCreate(
                                "Auto-reload timer",
                                1000 / portTICK_PERIOD_MS,
                                pdTRUE,
                                (void *)1,
                                myTimerCallback);

  // Check to make sure timers were created
  if(one_shot_timer == NULL || auto_reload_timer == NULL) {
    Serial.println("COuld not create one of the timers");
  } else {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("Starting timers ... ");

    xTimerStart(one_shot_timer, portMAX_DELAY);
    xTimerStart(auto_reload_timer, portMAX_DELAY);
  }
  
  vTaskDelete(NULL);
}

void loop() {

}
