// use only one core due to demo purpose
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Task: Perform some mundane task
void testTask(void *parameter){
  while(1){
    int a = 1;
    int b[100];

    // Do something with array so it's not optmized out by the compiler
    for(int i = 0; i < 100; i++){
      b[i] = a + 1;
    }

    Serial.println(b[0]);

    // Print out remaining stack memory (words)
    Serial.print("High water mark (word)");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));

    // Print out number of free heap memory bytes befor malloc
    Serial.print("Heap before malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());

    int *ptr = (int*)pvPortMalloc(1024 * sizeof(int));

    // one way to prevent heap overflow is to check the malloc output
    if(ptr == NULL){
      Serial.println("Not enough heap.");
    }
    else{
      for(int i = 0; i < 1024; i++){
        ptr[i] = 3;
      }
    }
    
    // Print out number of free heap memory bytes after malloc
    Serial.print("Heap after malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());

    // Free up our allocated memory
    vPortFree(ptr);

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup(){

  // Configure Serial
  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Memory Demo---");

  // Start the only other task
  xTaskCreatePinnedToCore(testTask,
                          "Test Task",
                          1500,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
                          
  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop(){
  
}
