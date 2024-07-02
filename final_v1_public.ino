// ============= OBECNÉ ============= //
  bool tisk = false; // Proměnná pro zapnutí průběžnéh výpisu na seriovou linku
// ============= OBECNÉ ============= //

// ============= NASTAVENÍ MĚŘENÍ PRŮMĚRU ============= //
  unsigned long lastAverageTime = 0; // Čas posledního výpočtu průměru
  const unsigned long MAX_INTERVAL = 300000; // Délka měření průměru - 5 minut
  //const int MAX_READINGS = 5000; // Max. velikost pole pro data (10000 je moc)
  const int RAIN_MAX_READINGS = 100;
  // Odhad je max. 85 pro data pro srážky
  const int SPEED_MAX_READINGS = 5000;
  // 0-26k, 16k je dost pomalé
  // Odhad je max. 26000 pro data rychlosti větru
  const int DIR_MAX_READINGS = 3100;
  // Odhad je max. 3000 pro data směru větru (měření nejdříve po 100 ms)
// ============= NASTAVENÍ MĚŘENÍ PRŮMĚRU ============= //

// ============= MĚŘENÍ SRÁŽEK ============= //
  const uint8_t RAIN_PIN = 2; // Definujte digitální pin připojený k výstupnímu pinu senzoru (OUT)
  const unsigned long RAIN_THRESHOLD = 1000; // Minimální limit na překlopení
  volatile unsigned long rainTimeSinceLastTick = 0; // čas impulzu
  volatile unsigned long rainLastTick = 0; // Čas posledního impulzu
  volatile bool rainTickDetected = false; // Indikátor detekce přerušení
  const float RAIN_AMOUNT = 0.2794; // Množství srážek za jeden impulz
  float rainReadings[RAIN_MAX_READINGS];
  unsigned long rainTimestamps[RAIN_MAX_READINGS];
  int rainNumReadings = 0;
  unsigned long rainLastUpdate = 0; // Časovač pro poslední změnu hodnoty
  const unsigned long RAIN_UPDATE_LIMIT = RAIN_THRESHOLD * 2; // Limit časovače pokud nepřijde nová hodnota
  float rainAveragePerHour = 0;
  int rainMem = 0; // Velikost zaplnění paměti pro data

  void rainTick()
  {
    unsigned long currentTime = millis();
    if ((currentTime - rainLastTick) > RAIN_THRESHOLD)
    {
      rainTimeSinceLastTick = currentTime - rainLastTick;
      rainLastTick = currentTime;
      rainTickDetected = true; // Nastavíme indikátor detekce přerušení
    }
  }

  void rainF()
  {
    unsigned long currentTime = millis();

    // Smazání starších hodnot
      for (int i = 0; i < rainNumReadings; i++)
      {
        if (currentTime - rainTimestamps[i] >= MAX_INTERVAL)
        {
          for (int j = i; j < rainNumReadings - 1; j++)
          {
            rainReadings[j] = rainReadings[j + 1];
            rainTimestamps[j] = rainTimestamps[j + 1];
          }
          rainNumReadings--;
          i--;
        }
      }

    // Uložení impulzu a načtení hodnoty
    if (rainTickDetected)
    {
      if (rainTimeSinceLastTick > RAIN_THRESHOLD)
      {      
        if (rainNumReadings < RAIN_MAX_READINGS) 
        {
          rainReadings[rainNumReadings] = RAIN_AMOUNT;
          rainTimestamps[rainNumReadings] = currentTime;
          if (tisk == true)
          {
            Serial.println("Prší! ");
          }
          rainNumReadings++;

        } 
      }
      rainTickDetected = false; // Reset indikátoru
      rainLastUpdate = currentTime; // Reset časovače
    } 
    else if (currentTime - rainLastUpdate > RAIN_UPDATE_LIMIT)
    {
      if (rainNumReadings < RAIN_MAX_READINGS)
      {
        rainReadings[rainNumReadings] = 0;
        rainTimestamps[rainNumReadings] = currentTime;
        if (tisk == true)
        {
          Serial.println("Neprší! ");
        }
        rainNumReadings++;
      }
      rainLastUpdate = currentTime; // Reset časovače
    }  
  }

  void rainAverageF()
  {
    float rainAverage = rainCalculateAverage();
    Serial.println();
    Serial.println("===== Data srážkoměru =====");
    Serial.print("Paměť pro déšt je plná z ");
    Serial.print(rainMem);
    Serial.print(" %, tj. ");
    Serial.print(rainNumReadings);
    Serial.print(" / ");
    Serial.println(RAIN_MAX_READINGS);
    Serial.print("AVG: ");
    Serial.print(rainAveragePerHour);
    Serial.println(" mm/hod");
  }

  float rainCalculateAverage()
  {
    if (rainNumReadings == 0)
    {
      return 0;
    }

    float total = 0;
    for (int i = 0; i < rainNumReadings; i++)
    {
      total += rainReadings[i];
    }

    // Přepočet na mm/h: (Celkové množství srážek / počet čtení) * (3600000 / maxInterval)
    float rainAveragePerInterval = total / rainNumReadings;
    rainAveragePerHour = (rainAveragePerInterval / MAX_INTERVAL) * 3600000;
  
    rainMem = (float)rainNumReadings / RAIN_MAX_READINGS * 100;
    return rainAveragePerHour;
  }
// ============= MĚŘENÍ SRÁŽEK ============= //

// ============= RYCHLOST VĚTRU ============= //
  const uint8_t SPEED_PIN = 4; // Definujte digitální pin připojený k výstupnímu pinu senzoru (OUT)

  // Nastavení detekce ticku
  const unsigned long SPEED_DEBOUCE_TIME = 20; // Debouce čas v ms
  const unsigned long SPEED_THRESHOLD = SPEED_DEBOUCE_TIME + 11; // Minimální limit na otáčku
  volatile unsigned long speedTimeSinceLastTick = 0; // čas otočky
  volatile unsigned long speedLastTick = 0;
  volatile bool speedTickDetected = false; // Indikátor detekce přerušení

  float speed; // Proměnná pro ukládání rychlost větru
  float speedAverage; // Proměnná pro výpočet průměrného času

  int speedReadings[SPEED_MAX_READINGS];
  unsigned long speedTimestamps[SPEED_MAX_READINGS];
  int speedNumReadings = 0;

  unsigned long speedLastUpdate = 0; // Časovač pro poslední změnu hodnoty
  const unsigned long SPEED_UPDATE_LIMIT = 10000; // Limit časovače pokud nepřijde nová hodnota

  int speedMem = 0; // Velikost zaplnění paměti pro data
 
  void speedTick()
  {
    unsigned long currentTime = millis();
    if ((currentTime - speedLastTick) > SPEED_DEBOUCE_TIME)
     {
       speedTimeSinceLastTick = currentTime - speedLastTick;
       speedLastTick = currentTime;
       speedTickDetected = true; // Nastavíme indikátor detekce přerušení
      }
  }

 void speedF()
  {
    unsigned long currentTime = millis();

    // Smazání starších hodnot
    for (int i = 0; i < speedNumReadings; i++)
    {
      if (currentTime - speedTimestamps[i] >= MAX_INTERVAL)
      {
        for (int j = i; j < speedNumReadings - 1; j++)
        {
          speedReadings[j] = speedReadings[j + 1];
          speedTimestamps[j] = speedTimestamps[j + 1];
        }
        speedNumReadings--;
        i--;
      }
    }

    if (speedTickDetected)
    {
      if (speedTimeSinceLastTick > SPEED_THRESHOLD)
      {
        speed = (1000 / speedTimeSinceLastTick) * 2.4;
        if (tisk == true)
        {
          Serial.print("          Rychlost: ");
          Serial.print(speed);
          Serial.println(" km/h");
        }
        if (speedNumReadings < SPEED_MAX_READINGS) 
        {
          speedReadings[speedNumReadings] = speed;
          speedTimestamps[speedNumReadings] = currentTime;
          speedNumReadings++;
        } 
        
      }
      speedTickDetected = false; // Reset indikátoru
      speedLastUpdate = currentTime; // Reset časovače
    } 
    else if (currentTime - speedLastUpdate > SPEED_UPDATE_LIMIT)
    {
      speed = 0;
      if (speedNumReadings < SPEED_MAX_READINGS)
      {
        speedReadings[speedNumReadings] = speed;
        speedTimestamps[speedNumReadings] = currentTime;
        speedNumReadings++;
      }
      speedLastUpdate = currentTime; // Reset časovače
    }
  }

  void speedAverageF()
  {
    speedAverage = speedCalculateAverage();
    Serial.println();
    Serial.println("===== Data anemometru =====");
    Serial.print("Paměť pro rychlost je plná z ");
    Serial.print(speedMem);
    Serial.print(" %, tj. ");
    Serial.print(speedNumReadings);
    Serial.print(" / ");
    Serial.println(SPEED_MAX_READINGS);
    Serial.print("AVG: ");
    Serial.print(speedAverage);
    Serial.println(" km/h");
  }


  float speedCalculateAverage()
  {
    if (speedNumReadings == 0)
    {
      return 0;
    }

    int total = 0;
    for (int i = 0; i < speedNumReadings; i++)
    {
      total += speedReadings[i];
    }

    return static_cast<float>(total) / speedNumReadings;

    speedMem = (float)speedNumReadings / SPEED_MAX_READINGS * 100;
  }
// ============= RYCHLOST VĚTRU ============= //

// ============= SMĚR VĚTRU ============= //
 const uint8_t DIRECTION_PIN = 3; // Mezi pinem a 3V3 je za 20kOhm odpor
 int dir; // Proměnná pro uložení čtení z pinu
 float direction = 0; // Proměnná pro uložení azimutu
 float dirAverage; // Proměnná pro uložení průměrného směru
 String dirTxt; // Vlákno pro uložení textové hodnoty směru
 int dirDelay = 100; // Směr měříme jednou za 100 milis

 float sumSinDir = 0;
 float sumCosDir = 0;
 int dirNumdirReadings = 0;

 int dirReadings[DIR_MAX_READINGS];
 unsigned long dirTimestamps[DIR_MAX_READINGS];

 unsigned long dirlastAverageTime = 0; // Čas posledního výpočtu průměru

 float dirMem = 0; // Velikost zaplnění paměti pro data

  void directionF()
  {
    unsigned long currentTime = millis();

    // Smazání starších hodnot
      for (int i = 0; i < dirNumdirReadings; i++)
      {
        if (currentTime - dirTimestamps[i] >= MAX_INTERVAL)
        {
          for (int j = i; j < dirNumdirReadings - 1; j++)
          {
            dirReadings[j] = dirReadings[j + 1];
            dirTimestamps[j] = dirTimestamps[j + 1];
          }
          dirNumdirReadings--;
          i--;
        }
      }

    // Určení směru větru
      dir = analogRead(DIRECTION_PIN);  
      if      (dir < 168) {direction = 112.5; dirTxt = "ESE";}
      else if (dir < 200) {direction = 67.5; dirTxt = "ENE";}
      else if (dir < 252) {direction = 90; dirTxt = "E";}
      else if (dir < 366) {direction = 157.5; dirTxt = "SSE";}
      else if (dir < 521) {direction = 135; dirTxt = "SE";}
      else if (dir < 663) {direction = 202.5; dirTxt = "SSW";}
      else if (dir < 910) {direction = 180; dirTxt = "S";}
      else if (dir < 1190) {direction = 22.5; dirTxt = "NNE";}
      else if (dir < 1560) {direction = 45; dirTxt = "NE";}
      else if (dir < 1900) {direction = 247.5; dirTxt = "WSW";}
      else if (dir < 2150) {direction = 225; dirTxt = "SW";}
      else if (dir < 2540) {direction = 337.5; dirTxt = "NNW";}
      else if (dir < 2950) {direction = 0; dirTxt = "N";}
      else if (dir < 3200) {direction = 292.5; dirTxt = "WNW";}
      else if (dir < 3600) {direction = 315; dirTxt = "NW";}
      else if (dir > 3700) {direction = 270; dirTxt = "W";}
      else direction = 0;
    
    // Výpis hodnot 
    
    if (tisk == true)
    {
      Serial.print("                               Směr: ");
      Serial.print(direction);
      Serial.print("° / ");
      Serial.println(dirTxt);
    }
    

    // Uložení čtení a načtení hodnoty
      if (dirNumdirReadings < DIR_MAX_READINGS)
      {
        dirReadings[dirNumdirReadings] = direction;
        dirTimestamps[dirNumdirReadings] = currentTime;
        dirNumdirReadings++;
      }
    delay(dirDelay);
  }

  void directionAverageF()
  {
    sumSinDir = 0;
    sumCosDir = 0;
    for (int i = 0; i < dirNumdirReadings; i++)
    {
      sumSinDir += sin(radians(dirReadings[i]));
      sumCosDir += cos(radians(dirReadings[i]));
    }
    if (dirNumdirReadings > 0)
      {
        dirAverage = atan2(sumSinDir / dirNumdirReadings, sumCosDir / dirNumdirReadings);
        dirAverage = degrees(dirAverage);
        if (dirAverage < 0)
        {
          dirAverage += 360;
        }
      }
 
    dirMem = (float)dirNumdirReadings / DIR_MAX_READINGS * 100;

    // Výpis na linku
    Serial.println();
    Serial.println("===== Data směru =====");
    Serial.print("Paměť pro směr větru je plná z ");
    Serial.print(dirMem);
    Serial.print(" %, tj. ");
    Serial.print(dirNumdirReadings);
    Serial.print(" / ");
    Serial.println(DIR_MAX_READINGS);
    Serial.print("Průměrný směr větru: ");
    Serial.print(dirAverage);
    Serial.println("°");
  }
// ============= SMĚR VĚTRU ============= //

// ============= NAPĚTÍ BATERKY ============= //
  #include <Wire.h>
  #include <SPI.h>
  #include <ESP32AnalogRead.h>
  ESP32AnalogRead adc;
  #define ADCpin 0
  #define DeviderRatio 1.7693877551  // Delic napeti z akumulatoru 1MOhm + 1.3MOhm
  float bat_voltage = 0;

  void baterkaF()
  {
    bat_voltage = adc.readVoltage() * DeviderRatio;
    Serial.println();
    Serial.print("Napětí na baterii = " );
    Serial.print(bat_voltage);
    Serial.println(" V");
  }
// ============= NAPĚTÍ BATERKY ============= //

// ============= TEPLOTA DALLAS DS18B20 ============= //
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #define pinTeplota 5 //adresa externí teploty 10

  OneWire oneWire(pinTeplota); // nastavení komunikace senzoru přes pin          
  DallasTemperature sensors(&oneWire); // převedeme onewire do Dallasu  
  float teplotaPotok = 0; // Proměnná pro teplotu Dallas v potoce

  void teplotaPotokF()
  {
    Serial.println();
    Serial.print("Teplota v potoce = ");
    sensors.requestTemperatures();
    teplotaPotok = sensors.getTempCByIndex(0);
    Serial.print(teplotaPotok);
    Serial.println(" °C");
  }
// ============= TEPLOTA DALLAS DS18B20 ============= //

// ============= BME280 ============= //
  #include <Adafruit_Sensor.h>
  #include <Adafruit_BME280.h>
  #define BME280_ADDRESS (0x77)   // (0x76) cut left and solder left pad on board
  #define SEALEVELPRESSURE_HPA (1013.25)
  Adafruit_BME280 bme; // I2C
  float temp = 0; // Proměnná pro teplotu BME280
  float press = 0; // Proměnná pro tlak BME280
  float hum = 0; // Proměnná pro vlhkost BME280

  void BME280F()
  {
    bme.takeForcedMeasurement();

    Serial.println();
    Serial.print("Teplota = ");
    temp = bme.readTemperature();
    Serial.print(temp);
    Serial.println(" °C");

    Serial.print("Tlak = ");
    press = bme.readPressure() / 100.0F;
    Serial.print(press);
    Serial.println(" hPa");

    Serial.print("Vlhkost = ");
    hum = bme.readHumidity();
    Serial.print(hum);
    Serial.println(" %");
  }
// ============= BME280 ============= //

// ============= WIFI+TMEP ============= //
  #include <WiFi.h>
  #include <esp_wifi.h>
  #include <HTTPClient.h>

  const char* ssid = "***";
  const char* password = "***";

  String serverName = "http://****.tmep.cz/index.php?";

  void wifiF()
  {
    Serial.println();
    Serial.print("WiFi = ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dB");

    if(WiFi.status()== WL_CONNECTED)
    {
        HTTPClient http;

        //GUID
        String serverPath = serverName 
        + "teplota=" + temp 
        + "&vlhkost=" + hum
        + "&tlak=" + press
        + "&srazky=" + rainAveragePerHour
        + "&direction=" + dirAverage
        + "&rychlost=" + speedAverage
        + "&teplotapotok=" + teplotaPotok
        + "&rainMem=" + rainMem
        + "&speedMem=" + speedMem
        + "&dirMem=" + dirMem
        + "&v=" + bat_voltage
        + "&rssi=" + WiFi.RSSI();

        Serial.println(serverPath);
        
        // zacatek http spojeni
        http.begin(serverPath.c_str());
        
        // http get request
        int httpResponseCode = http.GET();
        
        if (httpResponseCode>0) 
        {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
        }
        else 
        {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
        // Uvolneni
        http.end();
    }
    else 
    {
      Serial.println("Wi-Fi odpojeno");
    }
  }
// ============= WIFI+TMEP ============= //

void setup() 
{
  Serial.begin(115200); // vystup do konzole
  delay(100); 

  // ============= MĚŘENÍ SRÁŽEK ============= //
    pinMode(RAIN_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(RAIN_PIN), rainTick, FALLING);
  // ============= MĚŘENÍ SRÁŽEK ============= //

  // ============= RYCHLOST VĚTRU ============= //
    pinMode(SPEED_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(SPEED_PIN), speedTick, FALLING);
  // ============= RYCHLOST VĚTRU ============= //

  // ============= SMĚR VĚTRU ============= //
   pinMode(DIRECTION_PIN, INPUT);
  // ============= SMĚR VĚTRU ============= //

  // ============= WIFI+TMEP ============= //
    int pokus = 0; // promena pro pocitani pokusu o pripojeni

    WiFi.begin(ssid, password);
    Serial.println("Pripojovani");
    while(WiFi.status() != WL_CONNECTED) 
    {
    delay(500);
    Serial.print(".");
    if(pokus > 20) // Pokud se behem 10s nepripoji, uspi se na 300s = 5min
    {
      esp_sleep_enable_timer_wakeup(300 * 1000000);
      esp_deep_sleep_start();
    }
    pokus++;
    }

    Serial.println();
    Serial.print("Pripojeno do site, IP adresa zarizeni: ");
    Serial.println(WiFi.localIP());
  // ============= WIFI+TMEP ============= //

  // ============= BME280 ============= //
    Serial.println("BME280 test");
    Wire.begin (19, 18); // for ESP32 + uŠup
    if (! bme.begin(BME280_ADDRESS)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("        ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
    Serial.println("-- Weather Station Scenario --");
    Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
    Serial.println("filter off");
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );
    Serial.println(); 
  // ============= BME280 ============= //

  // ============= NAPĚTÍ BATERKY ============= //
   adc.attach(ADCpin); // NASTAVENI MERENI NAPETI AKUMULATORU
  // ============= NAPĚTÍ BATERKY ============= //

  // ============= TEPLOTA DALLAS DS18B20 ============= //
    sensors.begin();
  // ============= TEPLOTA DALLAS DS18B20 ============= //

}

void loop() 
{
  unsigned long currentTime = millis();
  rainF(); // Volání funkce na změření deště
  speedF(); // Volání funkce na změření rychlosti větru
  directionF(); // Volání funkce na změření směru větru

  // Kod bude průběžně měřit a každých MaxInterval udělá průměry a pošle hodnoty
  if (currentTime - lastAverageTime >= MAX_INTERVAL)
  //if (currentTime - lastAverageTime >= 60000) // Testovací interval 1 minuta
  {
    rainAverageF();
    speedAverageF();
    directionAverageF();
    baterkaF();
    teplotaPotokF();
    BME280F();
    wifiF();
    lastAverageTime = currentTime;
  } 
}





