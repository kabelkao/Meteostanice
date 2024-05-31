// ============= RYCHLOST VĚTRU ============= //
  const int speed_pin = 4; // Definujte digitální pin připojený k výstupnímu pinu senzoru (OUT)

  // Nastavení detekce ticku
  const unsigned long speedDebounceTime = 20; // Debouce čas v ms
  const unsigned long speedThreshold = speedDebounceTime + 11; // Minimální limit na otáčku
  volatile unsigned long speedTimeSinceLastTick = 0; // čas otočky
  volatile unsigned long speedLastTick = 0;
  volatile bool speedTickDetected = false; // Indikátor detekce přerušení

  float speed; // Proměnná pro ukládání rychlost větru
  float speedAverage; // Proměnná pro výpočet průměrného času

  // Nastavení pro průměr
  unsigned long speedDobaMereni = 10000; // Doba měření průměru
  unsigned long speedLastAverageTime = 0; // Čas posledního výpočtu průměru
  const int speedMaxReadings = 10000;
  const unsigned long speedMaxInterval = 60000; // Délka měření průměru
  int speedReadings[speedMaxReadings];
  unsigned long speedTimestamps[speedMaxReadings];
  int speedNumReadings = 0;

  unsigned long speedLastUpdate = 0; // Časovač pro poslední změnu hodnoty
  const unsigned long speedUdpateLimit = 10000; // Limit časovače pokud nepřijde nová hodnota
// ============= RYCHLOST VĚTRU ============= //

void speedTick()
{
  unsigned long currentTime = millis();
  if ((currentTime - speedLastTick) > speedDebounceTime)
  {
    speedTimeSinceLastTick = currentTime - speedLastTick;
    speedLastTick = currentTime;
    speedTickDetected = true; // Nastavíme indikátor detekce přerušení
  }
}

void setup() 
{
  Serial.begin(115200); // vystup do konzole
  delay(100); 
  pinMode(speed_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(speed_pin), speedTick, FALLING);
}

void loop() 
{
  unsigned long currentTime = millis();

  // Smazání starších hodnot
  for (int i = 0; i < speedNumReadings; i++)
  {
    if (currentTime - speedTimestamps[i] >= speedMaxInterval)
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
    if (speedTimeSinceLastTick > speedThreshold)
    {
      speed = (1000 / speedTimeSinceLastTick) * 2.4;
      Serial.print("Akt: ");
      Serial.print(speed);
      Serial.println(" km/h");
      
      if (speedNumReadings < speedMaxReadings) 
      {
        speedReadings[speedNumReadings] = speed;
        speedTimestamps[speedNumReadings] = currentTime;
        speedNumReadings++;
      } 
      
    }
    speedTickDetected = false; // Reset indikátoru
    speedLastUpdate = currentTime; // Reset časovače
  } 
  else if (currentTime - speedLastUpdate > speedUdpateLimit)
  {
    speed = 0;
    if (speedNumReadings < speedMaxReadings)
    {
      speedReadings[speedNumReadings] = speed;
      speedTimestamps[speedNumReadings] = currentTime;
      speedNumReadings++;
    }
    speedLastUpdate = currentTime; // Reset časovače
  }
  
  // Výpočet průměru a jeho výpis na seriovou linku
  
  if (currentTime - speedLastAverageTime >= speedMaxInterval)
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



