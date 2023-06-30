/*
 * FreeRTOS Esempio 13: Software Timer Demo
 *
 * Creazione di un timer software one-shot e di un timer periodico
 * entrambi i timers impiegano la stessa funzione callback
 * che si comporta in modo diverso in base a quale timer l'ha richiamata
 * (tramite l'interrogazione del parametro TimerID di ciascun timer)
 *
 * Nota: nel file soc.h sono definiti i riferimenti ai due core della ESP32:
 *   #define PRO_CPU_NUM (0)
 *   #define APP_CPU_NUM (1)
 *
 * Qui viene adoperata la APP_CPU
 * riga aggiunta al commento per capire le fasi dello staging
 *
 */

#include <Arduino.h>

// pin driver del Led
#define pinLed GPIO_NUM_23

// Timer ID del timer One-shot
#define TimerID_One_Shot 0
// Timer ID del timer Auto-Reload
#define TimerID_Auto_Reload 1

// variabili globali
// dichiarazione degli handler per due timer software
static TimerHandle_t one_shot_timer = NULL;
static TimerHandle_t auto_reload_timer = NULL;

//*****************************************************************************
// Callbacks

// Viene richiamata quando scade uno dei timers
void myTimerCallback(TimerHandle_t xTimer)
{

  // messaggio stampato se scade il timer 0
  if ((uint32_t)pvTimerGetTimerID(xTimer) == TimerID_One_Shot)
  {
    Serial.println("Il timer One-shot è scaduto");
    // spegnimento ritardato del led
    digitalWrite(pinLed, LOW);
  }

  // messaggio stampato se scade il timer 1
  if ((uint32_t)pvTimerGetTimerID(xTimer) == TimerID_Auto_Reload)
  {
    Serial.println("Il timer Auto-reload è scaduto");
  }
}

//**********************************************************************
// Main (sul core 1, con priorità 1)

// configurazione del sistema
void setup()
{
  // variabile locale da passare al task
  long int delay_arg;

  // Configurazione della seriale
  Serial.begin(115200);

  // breve pausa
  vTaskDelay(pdMS_TO_TICKS(1000));
  Serial.println();
  Serial.println("FreeRTOS Software Timer Demo");
  Serial.println("Un timer one-shot ed un timer auto-reload");

  // Crea il timer one-shot, con Timer ID = 0
  one_shot_timer = xTimerCreate(
      "One-shot timer",             // Nome del timer
      pdMS_TO_TICKS(4000),          // Periodo del timer (4000ms)
      pdFALSE,                      // One-shot
      (void *)TimerID_One_Shot,     // Timer ID
      myTimerCallback);             // Callback function

  // Crea il timer con auto-reload, con Timer ID = 1
  auto_reload_timer = xTimerCreate(
      "Auto-reload timer",          // Nome del timer
      pdMS_TO_TICKS(1000),          // Periodo del timer (1000ms)
      pdTRUE,                       // Auto-reload
      (void *)TimerID_Auto_Reload,  // Timer ID
      myTimerCallback);             // Callback function

  // Controllo della corretta creazione dei due timers
  if (one_shot_timer == NULL || auto_reload_timer == NULL)
  {
    Serial.println("Errore nella creazione dei due timers");
  }
  else
  {
    // pausa di 1 secondo e poi stampa il messaggio di avvio
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.println("Avvio dei timers...");

    // Funzione di sistema per l'avvio dei due timers 
    // (max block time se la command queue è piena)
    xTimerStart(one_shot_timer, portMAX_DELAY);
    xTimerStart(auto_reload_timer, portMAX_DELAY);
  }

  // accensione del led sul pin 23
  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, HIGH);

  // Elimina il task della setup() per dimostrare che i timers 
  // lavorano anche in assenza di task utente
  vTaskDelete(NULL);
}

void loop()
{
  // qui il programma non dovrebe mai arrivare
}