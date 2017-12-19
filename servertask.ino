#include "index_htm.h"

void serverTask( void * pvParameters )
{
  server.on( "/", HTTP_GET, []( AsyncWebServerRequest * request)
  {
   request->send( 200, "text/html", index_htm );    
  });

  server.on( "/api/setcolor", HTTP_GET, []( AsyncWebServerRequest * request)
  {
    if ( !request->hasArg( "color" ) )
    {
      return request->send( 400, "text/plain", "Invalid request" );
    }
  
    char content[50];

    uint32_t color = strtol( request->arg( "color").c_str(), NULL, 16 );

    uint8_t redVal   = ( color & 0xFF0000 ) >> 16;
    uint8_t greenVal = ( color & 0x00FF00 ) >> 8;
    uint8_t blueVal  =   color & 0x0000FF ;

    vTaskSuspend( xTempTaskHandle );
    vTaskSuspend( xOledTempTaskHandle );
    //Serial.printf( content );

    oled.clear();
    oled.setFont( ArialMT_Plain_10 );
    oled.drawString( 64, 0, WiFi.localIP().toString() );

    oled.drawString( 64, 10, "Manual color mode" );
    snprintf( content, sizeof( content ), "Red: %i", redVal );
    oled.drawString( 64, 20, content );
    snprintf( content, sizeof( content ), "Green: %i", greenVal );
    oled.drawString( 64, 30, content );
    snprintf( content, sizeof( content ), "Blue: %i", blueVal );
    oled.drawString( 64, 40, content );

    oled.display();

    //set led colors
    ledcWrite( redChannel,   map( redVal, 0, 255, 0, PWMdepth ) );
    ledcWrite( greenChannel, map( greenVal, 0, 255, 0, PWMdepth ) );
    ledcWrite( blueChannel,  map( blueVal, 0, 255, 0, PWMdepth ) );

    snprintf( content, sizeof( content ), "Red: %i Green: %i Blue: %i\n", redVal, greenVal, blueVal );
    request->send( 200, "text/plain", content );
  });

  server.on( "/api/tempmode", HTTP_GET, []( AsyncWebServerRequest * request)
  {
    vTaskResume( xTempTaskHandle );
    vTaskResume( xOledTempTaskHandle );    
    request->send( 200 );
  });
  
  server.onNotFound( []( AsyncWebServerRequest * request )
  {
    request->send( 404 , "text/plain", "Not found" );
  });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  server.begin();

  vTaskDelete( NULL );
}

