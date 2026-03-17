
//////////////////////////////////////////////////////////
//  TESTWifi.ino                                        //
//  PROGRAMME DE TEST, UTILISER POUR TEST UNIQUEMENT    //
//  NE PAS UTILISER POUR PRESENTATION                   //
//  Prog: Wifi                                          //
//////////////////////////////////////////////////////////

// Code modifié qui vient de la documentation Arduino.




#include <SPI.h>
#include <WiFi.h>

char ssid[] = "yourNetwork";     // SSID
char pass[] = "secretPassword";  // MDP
int status = WL_IDLE_STATUS;     // Statut Wifi

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // Check présence du shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("Shield Wifi non trouvé");
    while (true)
      ;
  }

  while (status != WL_CONNECTED) {
    Serial.print("Connection à : ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.print("Connecté");
  printCurrentNet();
  printWifiData();
}