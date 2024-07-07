# Hardware

**Meteostanice založená na desce LaskaKit Meteo Mini** (https://www.laskakit.cz/laskakit-meteo-mini/?variantId=10473) ke které jsou připojena čidla:
- přes I2C: BME 280 (https://www.laskakit.cz/arduino-senzor-tlaku--teploty-a-vlhkosti-bme280/)
- přes RJ 11: Dallas DS18B20 (https://www.laskakit.cz/dallas-ds18b20-orig--digitalni-vodotesne-cidlo-teploty-1m/)
- přes RJ 45: WH-SP-WS01 čidlo rychlosti větru - anemometr, WH-SP-WD čidlo směru větru a MS-WH-SP-RG srážkoměr (https://www.laskakit.cz/sada-cidel-k-meteo-wh1080-a-wh1090/)
<p float="left">
  <img src="https://github.com/kabelkao/Meteostanice/assets/171317176/d07cfb78-6e53-4a3a-86c1-474379223f15" width="33%" />
  <img src="https://github.com/kabelkao/Meteostanice/assets/171317176/72525bd3-fca3-4940-adc2-99ceac15172c" width="33%" />
  <img src="https://github.com/kabelkao/Meteostanice/assets/171317176/a97d262d-6033-47af-af0e-13ac59f18ac1" width="33%" />
</p>

**Na desce je vytvořený shield na kterém je provedeno celé zapojení:**
- odpor 4k7 pro Dallas DS1820 mezi 3V3 a DATA
- odpor 1k pro WH-SP-WS01 čidlo rychlosti větru mezi 3V3 a DATA
- odpor 20k pro WH-SP-WD čidlo směru větru mezi 3V3 a DATA
- odpor 1k pro MS-WH-SP-RG srážkoměr mezi 3V3 a DATA
- kondenzátor 100nF pro WH-SP-WS01 čidlo rychlosti větru mezi GND a DATA
- kondenzátor 100nF pro MS-WH-SP-RG srážkoměr mezi GND a DATA
- data z čidel rychlosti větru, směru větru a srážek jsou z původních kabelů s koncovkou 2xRJ11 svedeny do jednoho kabelu s koncovkou 1xRJ45 která je poté dovedena cca 8m k radiačnímu štítu kde je deska, data z čidla DS18B20 jsou vedena cca 10m pomocí kabelu RJ11 k desce

Napájení je provedeno trvalým připojením přes USB nabíječku 230V s výstupem 5V na desku LaskaKit Meteo Mini skrze BATTERY vstup s připojenou USB-C redukcí.

<p align="center">
  <img src="https://github.com/kabelkao/Meteostanice/assets/171317176/495ffc93-727d-41c8-80c5-8514f2550299" width="33%" />
  <img src="https://github.com/kabelkao/Meteostanice/assets/171317176/bb09052c-4681-4920-92b1-2596aa32fcca" width="33%" />
</p>


# Software

**Princip měření:**
- Na začátku kódu jsem definovány všechny proměnné a jednotlivé funkce které se poté volají.
- Celé měření se opakuje v nekonečné smyčce voláním funkcí na měření
- Měření srážek probíhá kontinuálně a časová značka se uloží pokud je zaznamenána nebo pokud uběhne limit (neprší, měříme ale srážky jsou 0)
- Měření rychlosti větru probíhá pouze pokud je zaznamenán pulz, jinak není ukládána žádná časová značka
- Měření směru větru probíhá kontinuálně s prodlevou min. 100 ms
- Hodnoty výše uvedených měření se ukládají průběžně do pole o přiměřené velikosti
- Jednou za 5 minut se provede výpočet průměrných hodnot větru a deště a jejich odeslání na TMEP.cz společně s aktuálními hodnotami z čidla BME280 a Dallas DS18B20
- Příklad odesílaných dat: _/index.php?teplota=25.45 &vlhkost=39.99 &tlak=986.41 &srazky==0.09 &direction==223.22 &rychlost==2.30 &teplotapotok==24.81 &rainMem=30.40 &speedMem=0.30 &dirMem=85.20 &v=4.97 &rssi=-53_, kde položky _Mem_ jsou % vyjádření zaplnění pole paměti pro danou měřenou proměnnou

**Zdroje/Inspirace:**
- https://www.instructables.com/Solar-Powered-WiFi-Weather-Station-V30/


