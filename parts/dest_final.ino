// ============= MĚŘENÍ SRÁŽEK ============= //
  const int rain_pin = 2; // Definujte digitální pin připojený k výstupnímu pinu senzoru (OUT)

  // Nastavení detekce ticku
  const unsigned long rainThreshold = 1000; // Minimální limit na překlopení
  
  unsigned long rainLastAverageTime = 0; // Čas posledního výpočtu průměru

  volatile unsigned long rainTimeSinceLastTick = 0; // čas impulzu
  volatile unsigned long rainLastTick = 0; // Čas posledního impulzu

  volatile bool rainTickDetected = false; // Indikátor detekce přerušení

  const float rainAmount = 0.2794; // Množství srážek za jeden impulz

  const int rainMaxReadings = 10000; // Max. velikost pole pro data
  const unsigned long rainMaxInterval = 300000; // Délka měření průměru
  float rainReadings[rainMaxReadings];
  unsigned long rainTimestamps[rainMaxReadings];
  int rainNumReadings = 0;

  unsigned long rainLastUpdate = 0; // Časovač pro poslední změnu hodnoty
  const unsigned long rainUdpateLimit = rainThreshold * 2; // Limit časovače pokud nepřijde nová hodnota
// ============= MĚŘENÍ SRÁŽEK ============= //

void rainTick()
{
  unsigned long currentTime = millis();
  if ((currentTime - rainLastTick) > rainThreshold)
  {
    rainTimeSinceLastTick = currentTime - rainLastTick;
    rainLastTick = currentTime;
    rainTickDetected = true; // Nastavíme indikátor detekce přerušení
  }
}

void setup() 
{
  Serial.begin(115200); // vystup do konzole
  delay(100); 
  pinMode(rain_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(rain_pin), rainTick, FALLING);
}

void loop() 
{
  unsigned long currentTime = millis();

  // Smazání starších hodnot
    for (int i = 0; i < rainNumReadings; i++)
    {
      if (currentTime - rainTimestamps[i] >= rainMaxInterval)
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
    if (rainTimeSinceLastTick > rainThreshold)
    {      
      if (rainNumReadings < rainMaxReadings) 
      {
        rainReadings[rainNumReadings] = rainAmount;
        rainTimestamps[rainNumReadings] = currentTime;
        Serial.print("Prší! ");
        rainNumReadings++;
      } 
    }
    rainTickDetected = false; // Reset indikátoru
    rainLastUpdate = currentTime; // Reset časovače
  } 
  else if (currentTime - rainLastUpdate > rainUdpateLimit)
  {
    if (rainNumReadings < rainMaxReadings)
    {
      rainReadings[rainNumReadings] = 0;
      rainTimestamps[rainNumReadings] = currentTime;
      Serial.print("Neprší! ");
      rainNumReadings++;
    }
    rainLastUpdate = currentTime; // Reset časovače
  }
  
  // Výpočet průměru a jeho výpis na seriovou linku
  
  if (currentTime - rainLastAverageTime >= rainMaxInterval)
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
  float rainAveragePerHour = (rainAveragePerInterval / rainMaxInterval) * 3600000;
  
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



