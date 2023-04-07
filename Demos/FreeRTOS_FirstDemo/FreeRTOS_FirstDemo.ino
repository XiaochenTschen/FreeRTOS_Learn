// use only one core due to demo purpose
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

//Pins
static const int led_pin = 22;

// task
void toggelLED(void *parameter){
  while(1){
    digitalWrite(led_pin, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  pinMode(led_pin, OUTPUT);

  // task to run forever
  xTaskCreatePinnedToCore(    // use xTaskCreate() in vanilla FreeRTOS    
                toggelLED,    // Function to be called
                "Toggle LED", // Name of task
                1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
                NULL,         // Parameter to pass to function
                1,            // Task priority (0 to configMAX_PRIORITIES - 1)
                NULL,         // Task handle
                app_cpu);     // Run on one core for demo purpose (ESP32 only)

}

void loop() {

}
