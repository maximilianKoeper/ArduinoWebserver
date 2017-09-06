/*                                                           ####################################
                                                             #   ArduServer     Version 3.2-en  #
  #####################~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~####################################~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ## STATUS:  STABLE ##
  #####################

  ___________________________________________________________________________________________________________
  It's possible to set a custom 404 message (Save 404 site as "404.htm" and copy it to the rootdir)
  --> If no custom 404 message is available a standart message will be shown instead.
  ______________________________________________________________________________________________________
  Filenames have to be in hte 8.3 format  --> e.g: index.htm NOT index.html !!
  --> Use .htm instead of .html !!
  SD-card has to be formated in FAT16/FAT32 
*/
#define indexfile "index.htm"
/*
  ___________________________________________________________________________________________________________
  Port can be changed (standart is 80)
*/
#define Port 80
/*
  ___________________________________________________________________________________________________________
  Mac Adresse can be changed
*/
#define macAdresse 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
/*
  ___________________________________________________________________________________________________________
  OnBoard LED shows status of the server
  --> Led on --> Booting --> Waiting for SD card (or no SD card inserted)
  --> Led blinking  --> DHCP unsuccessfully
  --> Led fade --> Server has been shutdown
  --> Led off --> everthing is ok
  ___________________________________________________________________________________________________________

  Root Directory:
*/
#define rootdir "/"
/*
                                                                       ############################
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#  Basic Authentification  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                                       ############################
  ___________________________________________________________________________________________________________
*/
#define securedir "secure/"    // Directory secured by Basic Authentification
/*
  ___________________________________________________________________________________________________________
*/
#define basicPasswort "YWRtaW5pc3RyYXRvcjpwYXNzd29ydA==" // 'administrator' is username and 'passwort' is the password (base64 encoded)
/*
  Zum Ändern des Passwortes muss man zuerst auf diese(oder andere) Website gehen https://www.base64encode.org/ und dort den Nutzername und Passwort folgendermaßen schreiben:

  username:passwort

  dann kodieren und einfügen.
  ___________________________________________________________________________________________________________

                                                                       #####################
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#  Server Manager   #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                                       #####################
  ___________________________________________________________________________________________________________
  With the URL "YOURIP/ServerManager" you are able to access the ServerManager
  ___________________________________________________________________________________________________________
                                                                          ###############
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#  Changelog  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                                          ###############
  ___________________________________________________________________________________________________________
  ## Version 3.0.0-0 ##
  1. Server Manager kann auch ohne SD Karte aufgerufen werden
  2. Bug in printDirectory behoben
  3. removed Internal Server Error
  4. Mac Adresse wird in Diagnose angezeigt
  ___________________________________________________________________________________________________________
  ## Version 3.1-en ##
  1.Translated most parts to English
  ___________________________________________________________________________________________________________
  ## Version 3.2-en ##
  1. drastically improved transfer speed (to client)
  ___________________________________________________________________________________________________________
  ____________________________________________________________________________________________________________________________________________________________
*/
#define chipSelect  4
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <SPI.h>           // required for communication between Arduino and Shield
#include <Ethernet.h>      // Library for Ethernet
#include <SD.h>            // Library for SD card
#define errorLed 13
#define ServerVersion "Version 3.2-en"
#define BUFSIZ 502
#define BUFSIZE 501

uint8_t mac[] = { macAdresse };

EthernetServer server(Port);
char *filename;
File htmlfile;

boolean noSDCard = false;
unsigned int rebootcount = 0;           // global var for counting "reboots"
boolean noindex = false;

EthernetClient client;

void setup() {
  pinMode(errorLed, OUTPUT);
  digitalWrite(errorLed, HIGH);
  byte i = 0;
  while (Ethernet.begin(mac) <= 0) {     // triying DHCP 5 times
    i++;
    if (i >= 5) {                        // If DHCP is not successful
      internalErrorHandler(1);
    }
  }
  server.begin();                        // start server
  if (SD.begin(chipSelect) == true) {    // try to initialize SD card  
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

void DHCPErrorHandler() {             // Executed if DHCP is not successfully 
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
      if (SD.exists(filename)) {                      //If no "404.htm" is available on the SD card
        htmlfile = SD.open(filename, FILE_READ);
        while (htmlfile.available())
        {
          client.write(htmlfile.read());             // send website to client
        }
        htmlfile.close();                            // Close file
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

void Content() {                                //send Content-Type html
  ContentType(1);
  client.println(F("html"));
  client.println(F("Connection: close"));
  client.println();
}

/////////////////////////////////////////////////////////////

void HTTP() {                                    //Send "HTTP/1.1" at every begin of the answer to client
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

void ConnectionStop() {                    // Closes the connection to client
  client.flush();
  client.stop();                           // shutdown connection
  htmlfile.close();
  loop();
}

/////////////////////////////////////////////////////////////

void ServerShutdown(boolean noreboot) {      // Reboot or shutdown the server
  if (noreboot == true) {
    HTTPHeader();
    client.println(F("<h1>Server Neugestartet</h1>"));
  }
  else {
    HTTPHeader();
    client.println(F("<h1>Server Heruntergefahren</h1>"));
  }
  client.flush();                          // Clean up some stuff
  client.stop();                           // Close connections
  htmlfile.close();                        // close files
  if (noreboot == false) {
    CPUsleep();
  }
  Ethernet.maintain();                     // Renew DHCP
  rebootcount++;
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
  client.print(F("<h1>Server Manager</h1><tr><td style='background-color: #FF7F64;text-align: center;width: 120px; height:120px; border-radius:50%;'><a href='./SH'>Shutdown</a>"));
  SpZe(1);
  client.print(F("<a href='./SR'>Reboot</a>"));
  SpZe(1);
  client.print(F("<a href='./SD'>Diagnosis</a>"));
  SpZe(1);
  client.print(F("<a href='../404.htm'>Show 404 site</a>"));
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
  client.print(F("<h1>Diagnosis data</h1><tr><td>"));
  client.print(F(ServerVersion));
  SpZe(2);
  client.print(F("Uptime(minutes): "));
  client.print(time);
  SpZe(2);
  client.print(rebootcount);
  client.print(F(" Reboot(s)"));
  SpZe(2);
  client.print(F("Problems: "));
  if (noindex == true) {
    client.print(F("index.htm missing"));
  }
  else if (time > 1400 && time / 1400 > rebootcount) {
    client.print(F("Reboot necessary"));
  }
  else if (FreeRam() < 1000) {
    client.print(F("Out of Memory"));
  }
  else if (noSDCard == true) {
    client.print(F("SD card not recognized"));
  }
  else {
    client.print(F("none"));
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
  client.print(F("MAC-adress:  "));
  for (int i = 0; i < 6; i++) {
    client.print(mac[i], HEX);
    if (i < 6) {
      client.print(F("."));
    }
  }
  SpZe(2);
  client.print(F("Available RAM: "));
  cli();
  client.print(FreeRam());
  sei();
  client.print(F(" bytes"));
  SpZe(2);
  client.print(F("Files on SD-card</br></br>"));
  printDirectory(SD.open(rootdir, FILE_READ), 0);
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
  if (SD.begin(chipSelect) == false) {            // Initialize SD card
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
  char clientline[BUFSIZ];                  // Array, for saving client request temporary
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

      if (noSDCard == true) {                // Gets executed if no SD card is available
        SDCheck();
      }

      if (strstr(clientline, "GET / ") != 0)     // nothing after "GET /" --> index.htm 
      {
        if (noSDCard == true) {                // Gets executed if no SD card is available
          SDCheck();
        }
        filename = indexfile;
        if (SD.exists(filename)) {               //if "index.htm" exists on SD card
          HTTPHeader();
          htmlfile = SD.open(filename, FILE_READ);     //open index.htm in READ_ONLY mode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          byte fileBuffer[64];
          int fileCounter = 0;
          
          while(htmlfile.available() && client.connected())
          {
            fileBuffer[fileCounter] = htmlfile.read();
            fileCounter++;
            if(fileCounter > 63)
            {
              client.write(fileBuffer,64);
              fileCounter = 0;
            }
          }

          if(fileCounter > 0) client.write(fileBuffer,fileCounter);       //final <64 byte cleanup packet     
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          htmlfile.close();                           // close file
        }
        else                                           // if "index.htm" does not exist
        {
          protokollErrorHandler(1);                            // 404
        }
        ConnectionStop();
      }
      else if (strstr(clientline, "GET /") != 0)
      {
        /////////////////////////////////////////////////////////////
#ifdef securedir

        if (strstr(clientline, securedir ) != 0) {    // Passwort required
          if (noSDCard == true) {                // Gets executed if no SD card is available
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
            if (strstr(clientline, basicPasswort) != 0) {        //Ceck wether client has required permissions
              ServerManager();
            }
            else {
              protokollErrorHandler(2);
            }
          }
          else if (strstr(clientline, "/SR " ) != 0) {      // Server reboot
            if (strstr(clientline, basicPasswort) != 0) {        //Ceck wether client has required permissions
              ServerShutdown(true);
              //continue;
            }
            else {
              protokollErrorHandler(2);
            }
          }
          else if (strstr(clientline, "/SH " ) != 0) {      // Server shutdown
            if (strstr(clientline, basicPasswort) != 0) {        //Ceck wether client has required permissions
              ServerShutdown(false);
            }
            else {
              protokollErrorHandler(2);
            }
          }
          else if (strstr(clientline, "/SD " ) != 0) {      // Server diagnosis
            if (strstr(clientline, basicPasswort) != 0) {        //Ceck wether client has required permissionst
              ServerDiagnose();
            }
            else {
              protokollErrorHandler(2);
            }
          }
          else {
            if (strstr(clientline, basicPasswort) != 0) {        //Ceck wether client has required permissions
              protokollErrorHandler(7);
            }
            else {
              protokollErrorHandler(2);
            }
          }
        }
        if (noSDCard == true) {                // Gets executed if no SD card is available
          SDCheck();
        }
        filename = clientline + 5;                     // look after "GET /"
        (strstr(clientline, " HTTP"))[0] = 0;

        if (SD.exists(filename)) {                             // If requested file is on SD card

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
            protokollErrorHandler(4);                   // Requested file is not supported
          }
          client.println(F("Connection: close"));
          client.println();
          htmlfile = SD.open(filename, FILE_READ);    //open file in READ_ONLY mode

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          byte fileBuffer[64];
          int fileCounter = 0;
          
          while(htmlfile.available() && client.connected())
          {
            fileBuffer[fileCounter] = htmlfile.read();
            fileCounter++;
            if(fileCounter > 63)
            {
              client.write(fileBuffer,64);
              fileCounter = 0;
            }
          }

          if(fileCounter > 0) client.write(fileBuffer,fileCounter);       //final <64 byte cleanup packet     
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          //htmlfile.close();                           // close file
        }
        else                                          // If requested file is not on SD card
        {
          if (strstr(clientline, "/ServerManager") != 0) {        //Ceck wether client has required permissions
            protokollErrorHandler(7);
          }
          else
          {
            protokollErrorHandler(1);                           // 404
          }
        }
        htmlfile.close();                           // close file
      }

      else if (strstr(clientline, "TRACE ") != 0) {
        HTTP200OK();
        ContentType(1);
        client.println(F("plain"));
        client.write(clientline, BUFSIZ);
      }

      else
      {
        protokollErrorHandler(5);                            // Request incorrect
      }

      ConnectionStop();                              // Close communication to client
    }
    ConnectionStop();                               // Close communication to client
  }
}
