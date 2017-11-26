void oledTask( void * pvParameters )
{
  oled.init();

  oled.flipScreenVertically();
  oled.setTextAlignment( TEXT_ALIGN_CENTER );
  oled.display();

  while (1)
  {
    oled.clear();
    oled.setFont( ArialMT_Plain_10 );

    if ( !WiFi.isConnected() )
    {
      oled.drawString( 64, 0, "Connecting WiFi" );
      oled.drawString( 64, 10, String( millis() ) );
    }
    else
    {
      oled.drawString( 64, 0, WiFi.localIP().toString() );

      char buff[10];

      snprintf( buff, sizeof( buff ), "%.1f°", temperature );

      if ( temperature != -1000 )
      {
        oled.setFont( ArialMT_Plain_24 );
        oled.drawString( 64, 20, buff );
      }
    }
    oled.display();
    vTaskDelay( 200 / portTICK_PERIOD_MS );
  }
}
