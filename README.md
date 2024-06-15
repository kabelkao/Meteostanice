Meteostanice založená na desce LaskaKit Meteo Mini ke které jsou připojena čidla:
- přes I2C: BME 280,
- přes RJ 11: Dallas DS18B20,
- přes RJ 45: WH-SP-WS01 čidlo rychlosti větru - anemometr, WH-SP-WD čidlo směru větru a MS-WH-SP-RG srážkoměr.

Na desce je vytvořený shield na kterém je provedeno celé zapojení:
- odpor 4k7 pro Dallas DS1820 mezi 3V3 a DATA
- odpor 1k pro WH-SP-WS01 čidlo rychlosti větru mezi 3V3 a DATA
- odpor 20k pro WH-SP-WD čidlo směru větru mezi 3V3 a DATA
- odpor 1k pro MS-WH-SP-RG srážkoměr mezi 3V3 a DATA
- kondenzátor 100nF pro WH-SP-WS01 čidlo rychlosti větru mezi GND a DATA
- kondenzátor 100nF pro MS-WH-SP-RG srážkoměr mezi GND a DATA

Napájení je provedeno trvalým připojením přes USB nabíječku 230V s výstupem 5V na desku LaskaKit Meteo Mini skrze BATTERY vstup s připojenou USB-C redukcí.

Zdroje/Inspirace:
- https://www.instructables.com/Solar-Powered-WiFi-Weather-Station-V30/
