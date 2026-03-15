
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
    // S'arreter
    while (true)
      ;
  }

  // Essayer de se connecter:

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connecte au wifi:
    status = WiFi.begin(ssid, pass);
    // Attendre 10 secondes
    delay(10000);
  }

  Serial.print("Connecté");
  printCurrentNet();
  printWifiData();
}

void loop() {
  // Checker la co ttes les 10secondes
  delay(10000);
  printCurrentNet();
}

// void printWifiData() {

  // Imprimer l'Adresse IP du Shield:

  // IPAddress ip = WiFi.localIP();
  // Serial.print("IP Address: ");
  // Serial.println(ip);
  // Serial.println(ip);

  // Imprimer l'adresse MAC de l'appareil :

  // byte mac[6];
  // WiFi.macAddress(mac);
  // Serial.print("MAC address: ");
  // Serial.print(mac[5], HEX);
  // Serial.print(":");
  // Serial.print(mac[4], HEX);
  // Serial.print(":");
  // Serial.print(mac[3], HEX);
  // Serial.print(":");
  // Serial.print(mac[2], HEX);
  // Serial.print(":");
  // Serial.print(mac[1], HEX);
  // Serial.print(":");
  // Serial.println(mac[0], HEX);
// }

// void printCurrentNet() {

  // Imprimer le SSID du réseau:

  // Serial.print("SSID: ");

  // Serial.println(WiFi.SSID());

  // Imprimer L'adresse MAC du routeur :

  // byte bssid[6];
  // WiFi.BSSID(bssid);
  // Serial.print("BSSID: ");
  // Serial.print(bssid[5], HEX);
  // Serial.print(":");
  // Serial.print(bssid[4], HEX);
  // Serial.print(":");
  // Serial.print(bssid[3], HEX);
  // Serial.print(":");
  // Serial.print(bssid[2], HEX);
  // Serial.print(":");
  // Serial.print(bssid[1], HEX);
  // Serial.print(":");
  // Serial.println(bssid[0], HEX);
// }