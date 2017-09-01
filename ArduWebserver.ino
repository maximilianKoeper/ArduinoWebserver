/*                                                           ####################################
                                                             #   ArduServer     Version 3.0.0-0 #
  #####################~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~####################################~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ## STATUS:  STABLE ##
  #####################

  ___________________________________________________________________________________________________________
  Man kann eine eigene 404 Meldung auf der SD-Karte speichern (Dateiname "404.htm")
  --> Fals es keine eigene 404 Meldung gibt wird eine Standart 404 Meldung Angezeigt.
  ______________________________________________________________________________________________________
  Dateinamen müssen im 8.3 Format vorliegen (das heißt maximal 8 Buchstaben, dann ein Punkt und dannnach 3 Buchstaben fÃ¼r das Dateiformat) --> z.B: index.htm NICHT index.html !!
  --> Antstatt .html --> .htm benutzten !!
  SD-Karte muss in FAT16/Fat32 formatiert sein
  Eine HTML-Seite/Datei muss "index.htm" genannt werden und im Hauptverzeichnis der SD Karte gespeichert werden.
  --> Kann hier veraendert werden
*/
#define indexfile "index.htm"
/*
  ___________________________________________________________________________________________________________
  Port kann geaendert werden (Standart ist 80)
*/
#define Port 80
/*
  ___________________________________________________________________________________________________________
  Mac Adresse kann geaendert werden
*/
#define macAdresse 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
/*
  ___________________________________________________________________________________________________________
  OnBoard LED zeigt Status des Servers an
  --> Led an --> Bootvorgang --> Warte auf SD Karte
  --> Led blinkt  --> DHCP war nicht erfolgreich
  --> Led fade --> Server heruntergefahren
  --> Led aus --> alles OK
  ___________________________________________________________________________________________________________

  Root Directory:
*/
#define websiteordner "/"
/*
                                                                       ############################
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#  Basic Authentification  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                                       ############################
  ___________________________________________________________________________________________________________
*/
 #define geschuetzterOrdner "secure/"    // Directory secured by Basic Authentification
/*
  ___________________________________________________________________________________________________________
*/
#define basicPasswort "YWRtaW5pc3RyYXRvcjpwYXNzd29ydA==" // 'administrator' ist der username und 'passwort' ist das password (base64 Verschlüsselt)
/*
  Zum Ändern des Passwortes muss man zuerst auf diese(oder andere) Website gehen https://www.base64encode.org/ und dort den Nutzername und Passwort folgendermaßen schreiben:

  username:passwort

  dann kodieren und einfügen.
  ___________________________________________________________________________________________________________

                                                                       #####################
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#  Server Manager   #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                                       #####################
  ___________________________________________________________________________________________________________
  Mit dem URL "/ServerManager/" kommt man in den Manager, von welchem man den Server steuern kann
  Dafür ist allerdings das Passwort für die Basic Authentification erforderlich, wenn nicht schon früher im Browser eingegeben.
  ___________________________________________________________________________________________________________
                                                                          ###############
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#  Changelog  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                                          ###############
  ___________________________________________________________________________________________________________
  ## Version 2.7 ##
  1. Server Manager hinzugefügt [Herunterfahren/Wartung/Neustarten]
  2. Bugfixes
  3. Kleine Geschwindigkeits optimierung durch Verwendung von Register Variablen
  ___________________________________________________________________________________________________________
  ## Version 2.8 ##
  1. Kleine Verbesserungen
  ___________________________________________________________________________________________________________
  ## Version 2.9 ##
  1. EEPROM entfehrnt
  2. Bugfix (Sende keine 200 Ok mehr, wenn Dateiformat nicht implementiert ist)
  3. Bugfix wenn index.htm gesendet wird (Es konnte ein Feheler auftreten, dass "200 OK" nicht gesendet wird)
  4. Wenn DHCP fehlschlägt wird FehlerDHCP() aufgerufen --> Nach 10 durchläufen wird setup() erneut ausgeführt
  5. Sonstige Verbesserungen
  6. Sonstige Bugfixes
  7. Server Wartungsseite wird nun als HTML Datei angezeigt --> Keine komplikationen mit anderen Browsern mehr
  8. Sonstige Verbesserungen an den Serververwaltungsseiten
  9. LED 13 zeigt nun den Boot Vorgang an --> An = Booten Aus = Bootvorgang abgeschlossen
  10. Relative Pfadangaben werden unterstützt
  11. SD Karte kann nun auch später eingesetzt werden
  12. Requests werden schneller verarbeitet --> Auf ServerManager Befehele werden nur 1mal überprüft
  13. Bugfix bei ServerManager Requests behoben
  14. IP anzeigen zu Diagnose (ServerManager) hinzugefügt
  15. Ethernet.maintain() zu Neustart hinzugefügt (DHCP)
  16. Buffer für Requests vergrößert (Auf (effektiv) 500 Chars) --> Bugfix für Basic Authentification
  17. Verbesserungen in Funktion ServerShutdown()
  18. Zeigt nun eine Meldung wenn Server Heruntergefahren oder Neugestartet wurde
  19. No Content Answer wurde entfehrnt --> wird nicht mehr benötigt
  20. Weitere kleinere Geschwindigkeitsoptimierungen
  21. ".ico" Dateien werden unterstützt
  22. Verbesserungen bei diversen Variablen
  23. Bug behoben, bei dem der Server weiterhin Daten sendet obwohl die Verbindung zu Client unterbrochen wurde
  24. TimeOut entfehrnt --> Stabiler + schneller
  25. Fade anstatt Blink wenn Server heruntergefahren wurde
  26. Fals Request länger als Buffer ist wird das auslesen des Requests abgebrochen --> Ein sehr langer Request wird nun schneller verarbeitet
  27. Verbesserungen bei ServerManager --> Redirect auf ServerManager fals falsch eingegeben
  28. Ungenutzte Programmteile entfehrnt
  ___________________________________________________________________________________________________________
  ## Version 2.9.1 ##
  1. Design der Wartungsseite verbessert
  2. Design der Diagnoseseite verbessert
  3. Möglichkeit hinzugefügt, um einfacher ServerManager funktionen einzufügen hinzugefügt
  4. Kleine Verbesserungen bei Fehlercodes
  5. Verbesserungen in Funktion ServerShutdown() + Bugfix + Geschwindigkeitsoptimierung
  6. Bugfix in Serverdiagnose --> HTML Code wurde falsch zusammengebaut + Bugfix in printDirectory()
  7. Weiterer Bugfix in printDirectory()
  8. printDirectory() --> Dateien werden in einem Dateibaum angezeigt + Ihre Größen in bytes
  9. Größe der Dateien wird nun in KiB ODER byte angezeigt (Je nach dem wie groß die Datei ist)
  10. RAM Nutzung etwas verbessert
  11. Das Herunterfahren des Servers legt die CPU in den "Schlafmodus"

  --> BUG: OUT OF MEMORY in Funktion printDirectory()
  ___________________________________________________________________________________________________________
  ## Version 2.9.2 ##
  1. OUT OF MEMORY BUG behoben.
  2. Sonstige Verbesserungen
  ___________________________________________________________________________________________________________
  ## Version 2.9.3 ##
  1. HTTP TRACE Funktion hinzugefügt für bessere Debugging Möglichkeiten
  ___________________________________________________________________________________________________________
  ## Version 2.9.4 ##
  1. UPDATE: Benutze neue Ethernet Library
  2. Überarbeitete GUI
  3. kleinere Bugfixes
  4. InternalErrorHandler() hinzugefügt
  5. Funktionen umbenannt (für bessere Übersicht)
  6. Setup() Ablauf verbessert
  ___________________________________________________________________________________________________________
  ## Version 2.9.4-1 ##
  1. kleinere Verbesserungen
  2. kleinere Verbesserungen in HTML Code für GUI
  ___________________________________________________________________________________________________________
  ## Version 3.0.0-0 ##
  1. Server Manager kann auch ohne SD Karte aufgerufen werden
  2. Bug in printDirectory behoben
  3. removed Internal Server Error 
  4. Mac Adresse wird in Diagnose angezeigt
  ___________________________________________________________________________________________________________
  ____________________________________________________________________________________________________________________________________________________________
*/
#define chipSelect  4
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <SPI.h>           // Wird für die Kommunikation zwischen Shield und Arduino benoetigt
#include <Ethernet.h>      // Library für Ethernet
#include <SD.h>            // Library für SD Karte
#define errorLed 13
#define ServerVersion "Version 3.0.0-0"
#define BUFSIZ 502
#define BUFSIZE 501

uint8_t mac[] = { macAdresse };          // Variable für mac Adresse

EthernetServer server(Port);
char *filename;
File htmlfile;

boolean noSDCard = false;
unsigned int rebootanzahl = 0;           // speichert die Anzahl der Reboots
boolean noindex = false;

EthernetClient client;

void setup() {
  pinMode(errorLed, OUTPUT);
  digitalWrite(errorLed, HIGH);
  byte i = 0;
  while (Ethernet.begin(mac) <= 0) {     // DHCP wird 5 mal versucht
    i++;
    if (i >= 5) {                        // Fals DHCP 5 mal fehlschlaegt wird aufgehuert
      internalErrorHandler(1);
    }
  }
  server.begin();                        // Server startet
  if (SD.begin(chipSelect) == true) {            // SD Karte wird initialisiert
    if (!SD.exists(indexfile)) {
      noindex = true;
    }
    digitalWrite(errorLed, LOW);
  }
  else {
    internalErrorHandler(3);
  }
}

/////////////////////////////////////////////////////////////

void internalErrorHandler(byte e) {
  switch (e) {
    case 1:
      DHCPErrorHandler();
      break;
    case 2:
      break;
    case 3:
      digitalWrite(errorLed, HIGH);
      noSDCard = true;
      break;
    default:
      ServerShutdown(true);
      break;
  }
}

/////////////////////////////////////////////////////////////

void DHCPErrorHandler() {             // Wird ausgefuehrt, fals DHCP 5 mal fehlschlaegt
  for (byte x = 0; x < 10; x++) {
    digitalWrite(errorLed, HIGH);
    delay(500);
    digitalWrite(errorLed, LOW);
    delay(500);
  }
  setup();
}

/////////////////////////////////////////////////////////////

void protokollErrorHandler(byte e) {
  HTTP();
  switch (e) {
    case 1:
      client.println(F("404 Not Found"));
      Content();

      filename = "404.htm";
      if (SD.exists(filename)) {                      //Fals "404.htm" auf der SD Karte existiert
        htmlfile = SD.open(filename, FILE_READ);
        while (htmlfile.available())
        {
          client.write(htmlfile.read());             // Website an Client schicken
        }
        htmlfile.close();                            // Datei wird geschlossen
        break;
      }
      else {
        client.println(F("<h1>404 Not Found</h1>"));
      }
      break;

    case 2:
      client.println(F("401 Access Denied"));
      client.println(F("WWW-Authenticate: Basic realm=''"));
      break;
/*
    case 3:
      client.println(F("500 Internal Server Error"));
      Content();
      client.println(F("<h1>Internal Server Error</h1>"));
      break;
*/
    case 4:
      client.println(F("501 Not Implemented"));
      Content();
      client.println(F("<h1>501 Not Implemented</h1>"));
      break;

    case 5:
      client.println(F("400 Bad Request"));
      Content();
      client.println(F("<h1>400 Bad Request</h1>"));
      break;

    case 7:
      client.println(F("302 Found"));
      client.println(F("Location: /ServerManager/"));
      break;
  }
  ConnectionStop();
}

/////////////////////////////////////////////////////////////

void Content() {                                //Sendet den Content-Type html
  ContentType(1);
  client.println(F("html"));
  client.println(F("Connection: close"));
  client.println();
}

/////////////////////////////////////////////////////////////

void HTTP() {                                    //Sendet HTTP/1.1 am Anfang der Antwort
  client.print(F("HTTP/1.1 "));
}

/////////////////////////////////////////////////////////////

void ContentType(register byte type) {
  client.print(F("Content-Type: "));
  switch (type) {
    case 1:
      client.print(F("text/"));
      break;
    case 2:
      client.print(F("video/"));
      break;
    case 3:
      client.print(F("image/"));
      break;
    case 4:
      client.print(F("application/"));
      break;
    case 5:
      client.print(F("audio/"));
      break;
  }
}

/////////////////////////////////////////////////////////////

void ConnectionStop() {                    // Beendet die Kommunikation zum Client
  client.flush();
  client.stop();                           // Verbindung trennen
  htmlfile.close();
  loop();
}

/////////////////////////////////////////////////////////////

void ServerShutdown(boolean noreboot) {      // Startet Server neu oder fährt ihn herunter
  if (noreboot == true) {
    HTTPHeader();
    client.println(F("<h1>Server Neugestartet</h1>"));
  }
  else {
    HTTPHeader();
    client.println(F("<h1>Server Heruntergefahren</h1>"));
  }
  client.flush();                          // Alte Daten löschen
  client.stop();                           // Verbindung trennen
  htmlfile.close();                        // SD File schließen
  if (noreboot == false) {
    CPUsleep();
  }
  Ethernet.maintain();                     // Erneuert IP per DHCP
  rebootanzahl++;
}

///////////////////////////////////////////////////////////

void CPUsleep()
{
  wdt_disable();
  set_sleep_mode( SLEEP_MODE_PWR_DOWN );

  while ( true )
  {
    sleep_enable();
    cli();
    sleep_mode();
    sleep_cpu();
  }
}

///////////////////////////////////////////////////////////

void printDirectory(File dir, int numTabs) {
  while (true) {
    htmlfile =  dir.openNextFile();
    if (! htmlfile) {
      break;
    }
    for (unsigned int i = 0; i < numTabs; i++) {
      client.print(F("|-->"));
    }
    client.print(htmlfile.name());
    if (htmlfile.isDirectory()) {
      client.println(" /");
      client.print(F("</br>"));
      printDirectory(htmlfile, numTabs + 1);
    }
    else {
      client.print(F(" | "));
      if (htmlfile.size() > 1000) {
        client.print(htmlfile.size() / 1024);
        client.print(F(" KiB"));
      }
      else {
        client.print(htmlfile.size());
        client.print(F(" byte"));
      }
    }
    htmlfile.close();
    client.print(F("</br>"));
  }
  htmlfile.close();
  dir.close();
  return;
}

/////////////////////////////////////////////////////////////

void ServerManager() {
  HTTPHeader();
  HTMLHeader(1);
  client.print(F("<h1>Server Manager</h1><tr><td style='background-color: #FF7F64;text-align: center;width: 120px; height:120px; border-radius:50%;'><a href='./SH'>Herunterfahren</a>"));
  SpZe(1);
  client.print(F("<a href='./SR'>Neustarten</a>"));
  SpZe(1);
  client.print(F("<a href='./SD'>Diagnose</a>"));
  SpZe(1);
  client.print(F("<a href='../404.htm'>404 Meldung anzeigen</a>"));
  client.print(F("</td></tr></table></body></html>"));
  ConnectionStop();
}

/////////////////////////////////////////////////////////////

void HTMLHeader(byte cas) {
  client.print(F("<!DOCTYPE html><html><body style='font-family: Roboto, sans-serif;'><table border='8' style='"));
  switch (cas) {
    case 1:
      client.print(F("background-color: black; padding: 5px;margin-left: auto;margin-right: auto;"));
      break;
    case 2:
      client.print(F("width:50%; background-color: #28FF4D; padding: 5px;margin-left: auto;margin-right: auto;"));
      break;
  }
  client.print(F("'>"));
}

/////////////////////////////////////////////////////////////

void SpZe(byte cas) {
  switch (cas) {
    case 2:
      client.print(F("</td></tr><tr><td>"));
      break;
    case 1:
      client.print(F("</td><td style='background-color: #F0CB2D;text-align: center;width: 120px; height:120px; border-radius:50%;'>"));
      break;
  }
}
/////////////////////////////////////////////////////////////

void ServerDiagnose() {
  unsigned int time = millis() / 60000;
  HTTPHeader();
  HTMLHeader(2);
  client.print(F("<h1>Wartungsdaten</h1><tr><td>"));
  client.print(F(ServerVersion));
  SpZe(2);
  client.print(F("Betriebszeit(Minuten): "));
  client.print(time);
  SpZe(2);
  client.print(rebootanzahl);
  client.print(F(" Neutstart(s)"));
  SpZe(2);
  client.print(F("Probleme: "));
  if (noindex == true) {
    client.print(F("index.htm fehlt"));
  }
  else if (time > 1400 && time / 1400 > rebootanzahl) {
    client.print(F("Reboot noetig"));
  }
  else if (FreeRam() < 1000) {
    client.print(F("Out of Memory Event"));
  }
  else if (noSDCard == true){
    client.print(F("Keine SD Karte erkannt"));
  }
  else {
    client.print(F("keine"));
  }
  SpZe(2);
  client.print(F("IP:  "));
  for (byte tb = 0; tb < 4; tb++) {
    client.print(Ethernet.localIP()[tb], DEC);
    if (tb < 3) {
      client.print(".");
    }
  }
  SpZe(2);
  client.print(F("MAC-Adresse:  "));
  for (int i=0; i<6; i++){
    client.print(mac[i], HEX);
    if(i<6){
      client.print(F("."));
    }
  }
  SpZe(2);
  client.print(F("Freier Ram: "));
  cli();
  client.print(FreeRam());
  sei();
  client.print(F(" bytes"));
  SpZe(2);
  client.print(F("Dateien auf SD-Karte</br></br>"));
  printDirectory(SD.open(websiteordner, FILE_READ), 0);
  client.print(F("</td></tr></table></body></html>"));
  ConnectionStop();
}

/////////////////////////////////////////////////////////////

void HTTPHeader() {
  HTTP200OK();
  Content();
}

/////////////////////////////////////////////////////////////

void HTTP200OK() {
  HTTP();
  client.println(F("200 OK"));
}

/////////////////////////////////////////////////////////////

void SDCheck() {
  if (SD.begin(chipSelect) == false) {            // SD Karte wird initialisiert
    digitalWrite(errorLed, HIGH);
    //protokollErrorHandler(3);
  }
  else {
    digitalWrite(errorLed, LOW);
    noSDCard = false;
    if (!SD.exists(indexfile)) {
      noindex = true;
    }
    else {
      noindex = false;
    }
  }
}

/////////////////////////////////////////////////////////////

void loop() {
  register int i = 0;
  byte bufoverflow = 0;
  char clientline[BUFSIZ];                  // Array, in der die Anfrage von Client gespeichert wird
  register char c;
  client = server.available();

  while (client.connected()) {
    if (client.available()) {
      while (client.available()) {
        c = client.read();
        clientline[i] = c;
        i++;
        if (i >= BUFSIZ)
          i = BUFSIZE;
        bufoverflow++;
        if (bufoverflow > 200) {
          break;
        }
      }
      clientline[i] = 0;
      i = 0;

      if (noSDCard == true) {                // Wird nur ausgeführt, wenn keine SD Karte eingelegt ist
        SDCheck();
      }

      if (strstr(clientline, "GET / ") != 0)     // nichts nach GET / --> index.htm wird angezeigt
      {
        if (noSDCard == true) {                // Wird nur ausgeführt, wenn keine SD Karte eingelegt ist
          SDCheck();
        }
        filename = indexfile;
        if (SD.exists(filename)) {               //wenn "index.htm" auf der SD Karte existiert
          HTTPHeader();
          htmlfile = SD.open(filename, FILE_READ);     //index.htm Datei wird im NUR LESEN Modus geoeffnet
          while (htmlfile.available() && client.connected())
          {
            client.write(htmlfile.read());             // Website an Client schicken
          }
          htmlfile.close();                            //schliesst "index.htm"
        }
        else                                           // Fals "index.htm" Nicht existiert
        {
          protokollErrorHandler(1);                            // 404
        }
        ConnectionStop();
      }
      else if (strstr(clientline, "GET /") != 0)
      {
        /////////////////////////////////////////////////////////////
#ifdef geschuetzterOrdner

        if (strstr(clientline, geschuetzterOrdner ) != 0) {    // Passwort benötigt
          if (noSDCard == true) {                // Wird nur ausgeführt, wenn keine SD Karte eingelegt ist
            SDCheck();
          }
          if (strstr(clientline, basicPasswort) != 0) {
          }
          else {
            protokollErrorHandler(2);
          }
        }
#endif
        /////////////////////////////////////////////////////////////

        if (strstr(clientline, " /ServerManager/") != 0) {
          if (strstr(clientline, "Manager/ " ) != 0) {      // Server Manager
            if (strstr(clientline, basicPasswort) != 0) {        //Prüft ob Client Authorisiert ist
              ServerManager();
            }
            else {
              protokollErrorHandler(2);
            }
          }
          else if (strstr(clientline, "/SR " ) != 0) {      // Server neustarten
            if (strstr(clientline, basicPasswort) != 0) {        //Prüft ob Client Authorisiert ist
              ServerShutdown(true);
              //continue;
            }
            else {
              protokollErrorHandler(2);
            }
          }
          else if (strstr(clientline, "/SH " ) != 0) {      // Server herunterfahren
            if (strstr(clientline, basicPasswort) != 0) {        //Prüft ob Client Authorisiert ist
              ServerShutdown(false);
            }
            else {
              protokollErrorHandler(2);
            }
          }
          else if (strstr(clientline, "/SD " ) != 0) {      // Server Wartung
            if (strstr(clientline, basicPasswort) != 0) {        //Prüft ob Client Authorisiert ist
              ServerDiagnose();
            }
            else {
              protokollErrorHandler(2);
            }
          }
          else {
            if (strstr(clientline, basicPasswort) != 0) {        //Prüft ob Client Authorisiert ist
              protokollErrorHandler(7);
            }
            else {
              protokollErrorHandler(2);
            }
          }
        }
        if (noSDCard == true) {                // Wird nur ausgeführt, wenn keine SD Karte eingelegt ist
          SDCheck();
        }
        filename = clientline + 5;                     // schaue nach "GET /"
        (strstr(clientline, " HTTP"))[0] = 0;

        if (SD.exists(filename)) {                             // Wenn Angefragte Datei vorhanden ist

          if (strstr(clientline, ".htm") != 0) {
            HTTP200OK();
            ContentType(1);
            client.println(F("html"));
          }
          else if (strstr(clientline, ".png") != 0) {
            HTTP200OK();
            ContentType(3);
            client.println(F("png"));
          }
          else if (strstr(clientline, ".pdf") != 0) {
            HTTP200OK();
            ContentType(4);
            client.println(F("pdf"));
          }
          else if (strstr(clientline, ".jpg") != 0) {
            HTTP200OK();
            ContentType(3);
            client.println(F("jpeg"));
          }
          else if (strstr(clientline, ".swf") != 0) {
            HTTP200OK();
            ContentType(4);
            client.println(F("x-shockwave-flash"));
          }
          else if (strstr(clientline, ".bmp") != 0) {
            HTTP200OK();
            ContentType(3);
            client.println(F("bmp"));
          }
          else if (strstr(clientline, ".ico") != 0) {
            HTTP200OK();
            ContentType(3);
            client.println(F("vnd.microsoft.icon"));
          }
          else if (strstr(clientline, ".gif") != 0) {
            HTTP200OK();
            ContentType(3);
            client.println(F("gif"));
          }
          else if (strstr(clientline, ".wma") != 0) {
            HTTP200OK();
            ContentType(5);
            client.println(F("x-ms-wma"));
          }
          else if (strstr(clientline, ".mp4") != 0) {
            HTTP200OK();
            ContentType(2);
            client.println(F("mp4"));
          }
          else if (strstr(clientline, ".avi") != 0) {
            HTTP200OK();
            ContentType(2);
            client.println(F("x-msvideo"));
          }
          else if (strstr(clientline, ".css") != 0) {
            HTTP200OK();
            ContentType(1);
            client.println(F("css"));
          }
          else if (strstr(clientline, ".txt") != 0) {
            HTTP200OK();
            ContentType(1);
            client.println(F("plain"));
          }
          else if (strstr(clientline, ".js") != 0) {
            HTTP200OK();
            ContentType(1);
            client.println(F("javascript"));
          }
          else if (strstr(clientline, ".xml") != 0) {
            HTTP200OK();
            ContentType(1);
            client.println(F("xml"));
          }
          else {
            protokollErrorHandler(4);                   // Angefragte Datei wird nicht unterstuetzt
          }
          client.println(F("Connection: close"));
          client.println();
          htmlfile = SD.open(filename, FILE_READ);    // Angefragte Datei wird im NUR LESEN Modus geoeffnet
          while (htmlfile.available() && client.connected())
          {
            client.write(htmlfile.read());            // Website an Client schicken
          }
          htmlfile.close();                           // Datei wird geschlossen
        }
        else                                          // Fals es die Angefragte Datei nicht gibt
        {
          if (strstr(clientline, "/ServerManager") != 0) {        //Prüft ob Client Authorisiert ist
            protokollErrorHandler(7);
          }
          else
          {
            protokollErrorHandler(1);                           // 404
          }
        }
      }
      
      else if (strstr(clientline, "TRACE ") != 0) {
        HTTP200OK();
        ContentType(1);
        client.println(F("plain"));
        client.write(clientline, BUFSIZ);
      }
      
      else
      {
        protokollErrorHandler(5);                            // Anfrage von Client war Fehlerhaft
      }
      
      ConnectionStop();                              // Beendet Kommunikation zu Client
    }
    ConnectionStop();                               // Beendet Kommunikation zu Client
  }
}
