void tempTask( void * pvParameters )
{
  //setup: setup client and url
  const char*    host    = "192.168.0.198";
  const char* sensorName = "kamer";
  const uint16_t port    = 80;

  statusText = "Find host...";

  while (1)
  {
    //loop: get temp from vissen and set colors
    WiFiClient client;
    bool connectionOK = false;

    if ( client.connect( host, port ) )
    {
      connectionOK = true;
    }
    else
    {
      Serial.println( " No host." );
      statusText = "No host.";
    }

    if ( connectionOK )
    {
      String url = "/api/getdevice?status=";

      client.print( String("GET ") + url + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "Connection: close\r\n\r\n");

      unsigned long timeout = millis();
      bool timeout_error = false;

      while ( client.available() == 0 && !timeout_error )
      {
        if ( millis() - timeout < 1000 )
        {
          vTaskDelay( 1 / portTICK_PERIOD_MS );
        }
        else
        {
          Serial.println( ">>> Client Timeout !" );
          client.stop();
          timeout_error = true;
        }
      }

      String line;
      bool headersDone = false;

      while ( client.available() && !headersDone )
      {
        line = client.readStringUntil( '\r' );
        line.trim();
        if ( line == "" )
        {
          headersDone = true;
        }
      }

      uint8_t currentLine = 0;

      while ( client.available() )
      {
        line = client.readStringUntil( '\n' );
        if ( currentLine > 7  )
        {
          String currentSensorName = line.substring( 0, line.indexOf( ",") );
          if ( currentSensorName == sensorName )
          {
            statusText = line.substring( line.indexOf( ",") + 1 );
            temperature = strtof( statusText.c_str(), NULL );
            ledcWrite( redChannel, redValue( temperature ) );
            ledcWrite( greenChannel, greenValue( temperature ) );
            ledcWrite( blueChannel, blueValue( temperature ) );
          }
        }
        currentLine++;
      }
      client.stop();
    }

    vTaskDelay( 1000 / portTICK_PERIOD_MS );
  }
}

static inline __attribute__((always_inline)) int blueValue( const float temp ) {
  if ( temp <= coldTemp ) return PWMdepth;
  if ( temp >= niceTemp ) return 0;
  return mapFloat( temp, coldTemp, niceTemp, PWMdepth, 0 );
}

static inline __attribute__((always_inline)) int greenValue( const float temp ) {
  if ( temp <= coldTemp || temp >= warmTemp ) return 0;
  if ( temp > coldTemp && temp < niceTemp ) return mapFloat( temp, coldTemp, niceTemp, 0, PWMdepth );
  return mapFloat( temp, niceTemp, warmTemp, PWMdepth, 0 );
}

static inline __attribute__((always_inline)) int redValue( const float temp ) {
  if ( temp <= niceTemp ) return 0;
  if ( temp >= warmTemp ) return PWMdepth;
  return mapFloat( temp, niceTemp, warmTemp, 0, PWMdepth );
}

static inline __attribute__((always_inline)) float mapFloat( float x, const float in_min, const float in_max, const float out_min, const float out_max)
{
  return ( x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min;
}
