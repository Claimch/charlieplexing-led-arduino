/* Charlieplexing example: 3 pins driving 6 LEDs. */

#define SCAN_DELAY 2
#define UPDATE_INTERVAL 1000
#define RESTART_THRESHOLD 3000
#include <avr/wdt.h>

byte LED_PINS[] = { 2, 3, 4 };
const byte LED_PIN_COUNT = sizeof(LED_PINS);
char buffer[255];
int index = 0;
char *kw;
char *arg;
char *arg1;
int led_on[30];
int Maxled_on;
bool allOff = false;
unsigned long previousUpdateTime = 0;
unsigned long previousSeconds = 0;
bool clockRunning = true;

void setup() {
  Serial.begin(115200);
  Maxled_on = 2;
}

void loop() {
  
  char ch;
  ch = Serial.read();
  if (ch != -1) {
    Serial.print('-');
    Serial.println(ch);
    delay(100);
    if (ch != '\n' && ch != '\r') {
      buffer[index++] = ch;
    } else {
      buffer[index] = '\0';
      Serial.println(buffer);
      kw = strtok(buffer, " ");
      Serial.println(kw);
      arg = strtok(NULL, " ");
      arg1= strtok(NULL, " ");
      index = 0;
      Serial.println(kw);
      Serial.println(strcmp(kw, "on"));
      if (strcmp(kw, "on") == 0) {
        Serial.println(arg);
        int ledIndex = atoi(arg);
        if (ledIndex > 0 && ledIndex <= Maxled_on) {
          int ledPin = led_on[ledIndex - 1];
          Pin_on(ledPin);
          Serial.print("LED ");
          Serial.print(ledIndex);
          Serial.println(" turned on.");

        } else {
          Serial.println("Invalid LED index.");
        }
      } else if (strcmp(kw, "off") == 0) {
        allOff = true;
        clear();
        Serial.println("All LEDs turned off.");
      }
      else {Serial.println("commade_inconue");
    }
    buffer[0] = '\0';
  }
 }
  if (!allOff) {
    for (int i = 0; i <= Maxled_on; i++) {
      Pin_on(led_on[i]);
    }
  } else {
    clear();
    allOff = false;
  }
    unsigned long currentTime = millis();

  // Vérifier le redémarrage de la carte
  if (currentTime - previousUpdateTime >= RESTART_THRESHOLD) {
    Serial.println("Redémarrage de la carte...");
    delay(1000); // Attendre 1 seconde avant de redémarrer
    wdt_enable(WDTO_15MS); // Activer le watchdog timer pour redémarrer la carte
    while (true); // Boucle infinie pour permettre au watchdog timer de redémarrer la carte
  }

  // Mettre à jour l'horloge
  if (currentTime - previousUpdateTime >= UPDATE_INTERVAL) {
    previousUpdateTime = currentTime;

    // Récupérer l'heure actuelle
    unsigned long seconds = currentTime / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;

    // Afficher l'heure dans le moniteur série
    Serial.print("Heure : ");
    Serial.print(hours);
    Serial.print(":");
    Serial.print(minutes % 60);
    Serial.print(":");
    Serial.println(seconds % 60);

    // Vérifier si les secondes ont été actualisées
    if (seconds != previousSeconds) {
      // Les secondes ont été actualisées, mettre à jour la valeur précédente
      previousSeconds = seconds;
    } else {
      // Les secondes n'ont pas été actualisées, incrémenter le compteur
      unsigned long nonUpdatedSeconds = currentTime - previousUpdateTime;

      // Vérifier si les secondes non actualisées atteignent le seuil de redémarrage
      if (nonUpdatedSeconds >= RESTART_THRESHOLD) {
        Serial.println("Redémarrage de la carte...");
        delay(1000); // Attendre 1 seconde avant de redémarrer
        wdt_enable(WDTO_15MS); // Activer le watchdog timer pour redémarrer la carte
        while (true); // Boucle infinie pour permettre au watchdog timer de redémarrer la carte
      }
    }
  }
}

void Pin_on(int i) {
  if (i == 1) {
    setpin(3, 2);
  } else if (i == 2) {
    setpin(2, 3);
  } else if (i == 3) {
    setpin(4, 2);
  } else if (i == 4) {
    setpin(2, 4);
  } else if (i == 5) {
    setpin(4, 3);
  } else if (i == 6) {
    setpin(3, 4);
  }
}

void setpin(int i, int j) {
  pinMode(i, OUTPUT);
  pinMode(j, OUTPUT);
  digitalWrite(i, HIGH);
  digitalWrite(j, LOW);
  delay(SCAN_DELAY);
  pinMode(i, INPUT);
  pinMode(j, INPUT);
}

void clear() {
  for (int i = 0; i < LED_PIN_COUNT; i++) {
    pinMode(LED_PINS[i], INPUT);
  }
}
void restartClock() {
  // Réinitialiser les variables de l'horloge et activer l'horloge
  previousUpdateTime = millis();
  previousSeconds = previousUpdateTime / 1000;
  clockRunning = true;
}
