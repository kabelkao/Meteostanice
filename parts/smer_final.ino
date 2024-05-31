// ============= SMĚR VĚTRU ============= //
 const int direction_pin = 3; // Mezi pinem a 3V3 je za 20kOhm odpor
 int dir; // Proměnná pro uložení čtení z pinu
 float direction = 0; // Proměnná pro uložení azimutu
 float dirAverage; // Proměnná pro uložení průměrného směru
 String dirTxt; // Vlákno pro uložení textové hodnoty směru

 float sumSinDir = 0;
 float sumCosDir = 0;
 int dirNumdirReadings = 0;

 const int dirMaxdirReadings = 10000;
 const unsigned long dirMaxInterval = 60000;
 int dirReadings[dirMaxdirReadings];
 unsigned long dirTimestamps[dirMaxdirReadings];

 unsigned long dirLastAverageTime = 0; // Čas posledního výpočtu průměru

// ============= SMĚR VĚTRU ============= //

void setup() 
{
  Serial.begin(115200);
  delay(100);
  pinMode(direction_pin, INPUT);
}

void loop() 
{
  unsigned long currentTime = millis();

  // Smazání starších hodnot
    for (int i = 0; i < dirNumdirReadings; i++)
    {
      if (currentTime - dirTimestamps[i] >= dirMaxInterval)
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
    dir = analogRead(direction_pin);  
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
    if (dirNumdirReadings < dirMaxdirReadings)
    {
      dirReadings[dirNumdirReadings] = direction;
      dirTimestamps[dirNumdirReadings] = currentTime;
      dirNumdirReadings++;
    }

  // Výpočet průměru
    if (currentTime - dirLastAverageTime >= dirMaxInterval)
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