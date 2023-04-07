// use only one core due to demo purpose
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// settings
static const uint8_t msg_queue_len = 5;

// globals
static QueueHandle_t msg_queue;

//*************************************************************************************************
// Tasks

// Task: wait for item on queue and print it
void printMessages(void *parameters){

  int item;

  while(1){
    // see if these is a message in the queue (do not block)
    if(xQueueReceive(msg_queue, (void *)&item, 0) == pdTRUE){
      Serial.println(item);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

//*************************************************************************************************
// Main

void setup(){
  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Queue Demo---");

  // create queue
  msg_queue = xQueueCreate(msg_queue_len, sizeof(int));

  // start print task
  xTaskCreatePinnedToCore(printMessages,
                          "Print Message",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
}

void loop(){
  
  static int num = 0;

  // try to add item to queue for 10 ticks, fail if queue is full
  if(xQueueSend(msg_queue, (void *)&num, 10) == pdTRUE){
    Serial.println("Queue full");
  }
  
  num++;

  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
