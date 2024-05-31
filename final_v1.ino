// ============= MĚŘENÍ SRÁŽEK ============= //
  const int RAIN_PIN = 2; // Definujte digitální pin připojený k výstupnímu pinu senzoru (OUT)
  const unsigned long RAIN_THRESHOLD = 1000; // Minimální limit na překlopení
  unsigned long rainLastAverageTime = 0; // Čas posledního výpočtu průměru
  volatile unsigned long rainTimeSinceLastTick = 0; // čas impulzu
  volatile unsigned long rainLastTick = 0; // Čas posledního impulzu
  volatile bool rainTickDetected = false; // Indikátor detekce přerušení
  const float RAIN_AMOUNT = 0.2794; // Množství srážek za jeden impulz
  const int RAIN_MAX_READINGS = 10000; // Max. velikost pole pro data
  const unsigned long RAIN_MAX_INTERVAL = 300000; // Délka měření průměru
  float rainReadings[RAIN_MAX_READINGS];
  unsigned long rainTimestamps[RAIN_MAX_READINGS];
  int rainNumReadings = 0;
  unsigned long rainLastUpdate = 0; // Časovač pro poslední změnu hodnoty
  const unsigned long RAIN_UPDATE_LIMIT = RAIN_THRESHOLD * 2; // Limit časovače pokud nepřijde nová hodnota

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
        if (currentTime - rainTimestamps[i] >= RAIN_MAX_INTERVAL)
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
          Serial.print("Prší! ");
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
        Serial.print("Neprší! ");
        rainNumReadings++;
      }
      rainLastUpdate = currentTime; // Reset časovače
    }
    
    // Výpočet průměru a jeho výpis na seriovou linku
    
    if (currentTime - rainLastAverageTime >= RAIN_MAX_INTERVAL)
    {  
      float rainAverage = rainCalculateAverage();
      Serial.println();
      Serial.print("    AVG: "); // Odsazení řádku aby byli hodnoty vidět při výpisu akt rychlosti
      Serial.print(rainAverage);
      Serial.println(" mm/hod");
      rainLastAverageTime = currentTime;
    }
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
    float rainAveragePerHour = (rainAveragePerInterval / RAIN_MAX_INTERVAL) * 3600000;
    
    /*
    Serial.println();
    Serial.print("    E: ");
    Serial.print(total);
    Serial.print(" / n: ");
    Serial.println(rainNumReadings);
    */

    //return static_cast<float>(total) / rainNumReadings;
    return rainAveragePerHour;
  }
// ============= MĚŘENÍ SRÁŽEK ============= //

// ============= RYCHLOST VĚTRU ============= //
  const int SPEED_PIN = 4; // Definujte digitální pin připojený k výstupnímu pinu senzoru (OUT)

  // Nastavení detekce ticku
  const unsigned long SPEED_DEBOUCE_TIME = 20; // Debouce čas v ms
  const unsigned long SPEED_THRESHOLD = SPEED_DEBOUCE_TIME + 11; // Minimální limit na otáčku
  volatile unsigned long speedTimeSinceLastTick = 0; // čas otočky
  volatile unsigned long speedLastTick = 0;
  volatile bool speedTickDetected = false; // Indikátor detekce přerušení

  float speed; // Proměnná pro ukládání rychlost větru
  float speedAverage; // Proměnná pro výpočet průměrného času

  // Nastavení pro průměr
  unsigned long speedDobaMereni = 10000; // Doba měření průměru
  unsigned long speedLastAverageTime = 0; // Čas posledního výpočtu průměru
  const int SPEED_MAX_READINGS = 10000;
  const unsigned long SPEED_MAX_INTERVAL = 60000; // Délka měření průměru
  int speedReadings[SPEED_MAX_READINGS];
  unsigned long speedTimestamps[SPEED_MAX_READINGS];
  int speedNumReadings = 0;

  unsigned long speedLastUpdate = 0; // Časovač pro poslední změnu hodnoty
  const unsigned long SPEED_UPDATE_LIMIT = 10000; // Limit časovače pokud nepřijde nová hodnota
 
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
      if (currentTime - speedTimestamps[i] >= SPEED_MAX_INTERVAL)
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
        Serial.print("Akt: ");
        Serial.print(speed);
        Serial.println(" km/h");
        
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
    
    // Výpočet průměru a jeho výpis na seriovou linku
    if (currentTime - speedLastAverageTime >= SPEED_MAX_INTERVAL)
    {  
      float speedAverage = speedCalculateAverage();
      Serial.print("    AVG: "); // Odsazení řádku aby byli hodnoty vidět při výpisu akt rychlosti
      Serial.print(speedAverage);
      Serial.println(" km/h");
      speedLastAverageTime = currentTime;
    }
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

    Serial.print("    E: ");
    Serial.print(total);
    Serial.print(" / n: ");
    Serial.print(speedNumReadings);

    return static_cast<float>(total) / speedNumReadings;
  }
// ============= RYCHLOST VĚTRU ============= //

// ============= SMĚR VĚTRU ============= //
 const int DIRECTION_PIN = 3; // Mezi pinem a 3V3 je za 20kOhm odpor
 int dir; // Proměnná pro uložení čtení z pinu
 float direction = 0; // Proměnná pro uložení azimutu
 float dirAverage; // Proměnná pro uložení průměrného směru
 String dirTxt; // Vlákno pro uložení textové hodnoty směru

 float sumSinDir = 0;
 float sumCosDir = 0;
 int dirNumdirReadings = 0;

 const int DIR_MAX_READINGS = 10000;
 const unsigned long DIR_MAX_INTERVAL = 60000;
 int dirReadings[DIR_MAX_READINGS];
 unsigned long dirTimestamps[DIR_MAX_READINGS];

 unsigned long dirLastAverageTime = 0; // Čas posledního výpočtu průměru

  void directionF()
  {
    unsigned long currentTime = millis();

    // Smazání starších hodnot
      for (int i = 0; i < dirNumdirReadings; i++)
      {
        if (currentTime - dirTimestamps[i] >= DIR_MAX_INTERVAL)
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
      Serial.print(direction);
      Serial.print(", ");
      Serial.print(dirTxt);
      Serial.print(", ");
      Serial.println(dir);
      delay(100);
    
    // Uložení čtení a načtení hodnoty
      if (dirNumdirReadings < DIR_MAX_READINGS)
      {
        dirReadings[dirNumdirReadings] = direction;
        dirTimestamps[dirNumdirReadings] = currentTime;
        dirNumdirReadings++;
      }

    // Výpočet průměru
      if (currentTime - dirLastAverageTime >= DIR_MAX_INTERVAL)
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
      // Výpis na linku
      Serial.print("Průměrný směr větru: ");
      Serial.print(dirAverage);
      Serial.println(" °");

      dirLastAverageTime = currentTime;
      }

  }
// ============= SMĚR VĚTRU ============= //



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


}

void loop() 
{
  unsigned long currentTime = millis();

  rainF(); // Volání funkce na změření deště

  speedF(); // Volání funkce na změření rychlosti větru

  directionF(); // Volání funkce na změření směru větru

  
}





