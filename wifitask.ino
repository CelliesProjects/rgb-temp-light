void wifiTask( void * pvParameters )
{
  //setup: connect WiFi
  WiFi.mode( WIFI_STA );
  WiFi.begin();

  while ( !WiFi.isConnected() ) {
    vTaskDelay( 500 / portTICK_PERIOD_MS );
    Serial.print( "." );
  }
  Serial.println( "WiFi connected." );

  xTaskCreatePinnedToCore(
    tempTask,                        /* Function to implement the task */
    "tempTask",                      /* Name of the task */
    3000,                            /* Stack size in words */
    NULL,                            /* Task input parameter */
    1,                               /* Priority of the task */
    &xTempTaskHandle,                            /* Task handle. */
    1);

  xTaskCreatePinnedToCore(
    serverTask,                      /* Function to implement the task */
    "serverTask",                    /* Name of the task */
    2000,                            /* Stack size in words */
    NULL,                            /* Task input parameter */
    1,                               /* Priority of the task */
    NULL,                            /* Task handle. */
    1);

  //loop: keep wifi connected
  while (1)
  {
    if ( !WiFi.isConnected() )
    {
      WiFi.reconnect();
    }
    vTaskDelay( 10000 / portTICK_PERIOD_MS );
  }
}

