/*
* sketch adapted by Nicu FLORICA (niq_ro) - http://www.tehnic.go.ro
* using info from 
* http://eeenthusiast.com/arduino-i2c-adxl-345-robot/
* https://learn.adafruit.com/adxl345-digital-accelerometer/assembly-and-wiring
* https://www.carnetdumaker.net/articles/faire-une-barre-de-progression-avec-arduino-et-liquidcrystal/
* ver.2.1 - 22.11.2017, Craiova - Romania
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// https://www.dfrobot.com/wiki/index.php/Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009)
//Sample using LiquidCrystal library
#include <LiquidCrystal.h>

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void displaySensorDetails(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void displayDataRate(void)
{
  Serial.print  ("Data Rate:    "); 
  
  switch(accel.getDataRate())
  {
    case ADXL345_DATARATE_3200_HZ:
      Serial.print  ("3200 "); 
      break;
    case ADXL345_DATARATE_1600_HZ:
      Serial.print  ("1600 "); 
      break;
    case ADXL345_DATARATE_800_HZ:
      Serial.print  ("800 "); 
      break;
    case ADXL345_DATARATE_400_HZ:
      Serial.print  ("400 "); 
      break;
    case ADXL345_DATARATE_200_HZ:
      Serial.print  ("200 "); 
      break;
    case ADXL345_DATARATE_100_HZ:
      Serial.print  ("100 "); 
      break;
    case ADXL345_DATARATE_50_HZ:
      Serial.print  ("50 "); 
      break;
    case ADXL345_DATARATE_25_HZ:
      Serial.print  ("25 "); 
      break;
    case ADXL345_DATARATE_12_5_HZ:
      Serial.print  ("12.5 "); 
      break;
    case ADXL345_DATARATE_6_25HZ:
      Serial.print  ("6.25 "); 
      break;
    case ADXL345_DATARATE_3_13_HZ:
      Serial.print  ("3.13 "); 
      break;
    case ADXL345_DATARATE_1_56_HZ:
      Serial.print  ("1.56 "); 
      break;
    case ADXL345_DATARATE_0_78_HZ:
      Serial.print  ("0.78 "); 
      break;
    case ADXL345_DATARATE_0_39_HZ:
      Serial.print  ("0.39 "); 
      break;
    case ADXL345_DATARATE_0_20_HZ:
      Serial.print  ("0.20 "); 
      break;
    case ADXL345_DATARATE_0_10_HZ:
      Serial.print  ("0.10 "); 
      break;
    default:
      Serial.print  ("???? "); 
      break;
  }  
  Serial.println(" Hz");  
}

//https://www.carnetdumaker.net/articles/faire-une-barre-de-progression-avec-arduino-et-liquidcrystal/
//https://www.carnetdumaker.net/snippets/1/
/* Constantes pour la taille de l'écran LCD */
const int LCD_NB_ROWS = 2;
const int LCD_NB_COLUMNS = 16;

void displayRange(void)
{
  Serial.print  ("Range:         +/- "); 
  
  switch(accel.getRange())
  {
    case ADXL345_RANGE_16_G:
      Serial.print  ("16 "); 
      break;
    case ADXL345_RANGE_8_G:
      Serial.print  ("8 "); 
      break;
    case ADXL345_RANGE_4_G:
      Serial.print  ("4 "); 
      break;
    case ADXL345_RANGE_2_G:
      Serial.print  ("2 "); 
      break;
    default:
      Serial.print  ("?? "); 
      break;
  }  
  Serial.println(" g");  
}

float gx, gy, gz;
double geu, geu0;
float lovitura = 1.;
float maxim = 1.;
unsigned long nou;
unsigned long gata = 3000;

/* Caractères personnalisés */
byte DIV_0_OF_5[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
}; // 0 / 5

byte DIV_1_OF_5[8] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
}; // 1 / 5

byte DIV_2_OF_5[8] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
}; // 2 / 5

byte DIV_3_OF_5[8] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
}; // 3 / 5

byte DIV_4_OF_5[8] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
}; // 4 / 5

byte DIV_5_OF_5[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
}; // 5 / 5


/**
 * Fonction de configuration de l'écran LCD pour la barre de progression.
 * Utilise les caractéres personnalisés de 0 à 5 (6 et 7 restent disponibles).
 */
void setup_progressbar() {

  /* Enregistre les caractères personnalisés dans la mémoire de l'écran LCD */
  lcd.createChar(0, DIV_0_OF_5);
  lcd.createChar(1, DIV_1_OF_5);
  lcd.createChar(2, DIV_2_OF_5);
  lcd.createChar(3, DIV_3_OF_5);
  lcd.createChar(4, DIV_4_OF_5);
  lcd.createChar(5, DIV_5_OF_5);
}


/**
 * Fonction dessinant la barre de progression.
 *
 * @param percent Le pourcentage à afficher.
 */
void draw_progressbar(byte percent) {

  /* Affiche la nouvelle valeur sous forme numérique sur la premiére ligne */
  lcd.setCursor(12, 0);
  if (percent < 100)  lcd.print(" ");
  if (percent < 10)  lcd.print(" ");
  lcd.print(percent);
  lcd.print(F("% "));
  // N.B. Les deux espaces en fin de ligne permettent d'effacer les chiffres du pourcentage
  // précédent quand on passe d'une valeur à deux ou trois chiffres à une valeur à deux ou un chiffre.

  /* Déplace le curseur sur la seconde ligne */
  lcd.setCursor(0, 1);

  /* Map la plage (0 ~ 100) vers la plage (0 ~ LCD_NB_COLUMNS * 5) */
  byte nb_columns = map(percent, 0, 100, 0, LCD_NB_COLUMNS * 5);

  /* Dessine chaque caractére de la ligne */
  for (byte i = 0; i < LCD_NB_COLUMNS; ++i) {

    /* En fonction du nombre de colonnes restant à afficher */
    if (nb_columns == 0) { // Case vide
      lcd.write((byte) 0);

    } else if (nb_columns >= 5) { // Case pleine
      lcd.write(5);
      nb_columns -= 5;

    } else { // Derniére case non vide
      lcd.write(nb_columns);
      nb_columns = 0;
    }
  }
}




void setup(void) 
{
 lcd.begin(16, 2);              // start the library
 setup_progressbar();
 lcd.setCursor(2,0);
 lcd.print("ADXL345 test"); // print a simple message
 lcd.setCursor(4,1);
 lcd.print("by niq_ro"); // print a simple message
  
#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif
  Serial.begin(9600);
  Serial.println("Accelerometer Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
  // displaySetRange(ADXL345_RANGE_8_G);
  // displaySetRange(ADXL345_RANGE_4_G);
  // displaySetRange(ADXL345_RANGE_2_G);
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Display additional settings (outside the scope of sensor_t) */
  displayDataRate();
  displayRange();
  Serial.println("");

delay(2000);
lcd.clear();
  /* Get a new sensor event */ 
  sensors_event_t event; 
  accel.getEvent(&event);
 
  /* Display the results (acceleration is measured in m/s^2) */
gx = event.acceleration.x; 
gy = event.acceleration.y; 
gz = event.acceleration.z; 
geu0 = sqrt (gx*gx + gy*gy + gz*gz);
  
//  Serial.print("X: "); Serial.print(event.acceleration.g); Serial.print("  ");
  Serial.print("X0: "); Serial.print(gx); Serial.print("  ");
  Serial.print("Y0: "); Serial.print(gy); Serial.print("  ");
  Serial.print("Z0: "); Serial.print(gz); Serial.print("  ");Serial.println("m/s^2 ");
  Serial.print("G0: "); Serial.print(geu0); Serial.print("  ");Serial.println("m/s^2 ");
  Serial.println("----------------");
  
}

void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event; 
  accel.getEvent(&event);
 
  /* Display the results (acceleration is measured in m/s^2) */
gx = event.acceleration.x; 
gy = event.acceleration.y; 
gz = event.acceleration.z; 
geu = sqrt (gx*gx + gy*gy + gz*gz);
lovitura = geu/geu0;
if (lovitura > maxim)
{
  maxim = lovitura;
  nou = millis();
}
 if ((millis() - nou) > gata)
 {
  maxim = 1.;
//  lcd.clear(); 
 }
 
//  Serial.print("X: "); Serial.print(event.acceleration.g); Serial.print("  ");
/*
  Serial.print("X: "); Serial.print(gx); Serial.print("  ");
  Serial.print("Y: "); Serial.print(gy); Serial.print("  ");
  Serial.print("Z: "); Serial.print(gz); Serial.print("  ");Serial.println("m/s^2 ");
  Serial.print("G: "); Serial.print(geu); Serial.print("  ");Serial.println("m/s^2 ");
  */
  Serial.print("G: "); Serial.print(lovitura); Serial.print("  ");Serial.println(" !!!");
  Serial.println("----------------");
 
 lcd.setCursor(0,0);
 lcd.print(lovitura); // print a simple message
 lcd.print("  "); // print a simple message

 lcd.setCursor(6,0);
 lcd.print(maxim); // print a simple message
 lcd.print("      "); // print a simple message

 /* Affiche la valeur */
float percent1 = float(maxim * 100./16.-6.);
byte percent = float(100.*percent1/96.);
 Serial.print("procent: "); Serial.print(percent);Serial.println(" %");
if (percent > 100) percent = 100;

  draw_progressbar(percent);
 delay(50);
}
