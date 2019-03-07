/*                                                           ####################################
                                                             #  ArduServer    Version 3.4.1-en  #
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
  Mac adress can be changed
*/
#define macAdresse 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
/*
  Ip adress if DHCP fails or you want to set up your ip manually
*/
#define IPADR 192 ,168, 178, 60
#define EnableDHCP
/*
  ___________________________________________________________________________________________________________
  OnBoard LED shows status of the server
  --> Led on --> Booting --> Waiting for SD card (or no SD card inserted)
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
// #define securedir "secure/"    // Directory secured by Basic Authentification (Keep in mind that this is not realy secure!!!)
/*
  ___________________________________________________________________________________________________________
*/
#define basicPasswort "YWRtaW5pc3RyYXRvcjpwYXNzd29ydA==" // 'administrator' is username and 'passwort' is the password (base64 encoded)
/*
  Zum Ändern des Passwortes muss man zuerst auf diese(oder andere) Website gehen https://www.base64encode.org/ und dort den Nutzername und Passwort folgendermaßen schreiben:

  username:passwort

  dann kodieren und einfügen.
  ___________________________________________________________________________________________________________

*/
//#define EnableTRACE
/*
                                                                       #####################
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#  Server Manager   #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                                       #####################
  ___________________________________________________________________________________________________________
  With the URL "YOURIP/ServerManager" you are able to access the ServerManager

  --> Basic Diagnostic Tools and the possibility to "reboot" the server
  --> To prevent data corruption on SD card shut the server down before unplugging it
  ___________________________________________________________________________________________________________
                                                                          ###############
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#  Changelog  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                                          ###############
  ___________________________________________________________________________________________________________
  ## Version 3.2-en ##
  1. drastically improved transfer speed (to client)
  2. removed client.connected() checks to speed up the transfer speed
  3. removed some unnecessary parts
  4. Some changes
  5. Bugfix
  ___________________________________________________________________________________________________________
  ## Version 3.3-en ##
  1. Bugfix --> printDirectory() is now working properly
  2. Minor improvements
  ___________________________________________________________________________________________________________
  ## Version 3.3.1-en ##
  1. Minor changes
  2. Changes in Documentation
  3. Set securedir Default to undefined
  ___________________________________________________________________________________________________________
  ## Version 3.4-en ##
  1. ServerManager shows unsuccessful login attempts
  2. ServerManager shows Version of Ehternet Shield
  3. Minor improvements to reaction speed
  4. Trace Method is now disabled by default
  5. Possibility to set a manual IP
  ___________________________________________________________________________________________________________
  ## Version 3.4.1-en ##
  1. Bugfix 404
  2. Removing some unnecessary SD Checks
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
#define ServerVersion "Version 3.4.1-en"
#define BUFSIZ 502
#define BUFSIZE 501
#define FILEBUF 64

uint8_t mac[] = { macAdresse };

EthernetServer server(Port);
char *filename;

#ifndef EnableDHCP
IPAddress ip(IPADR);
#endif
boolean noSDCard = false;
int rebootcount = 0;           // global var for counting "reboots"
unsigned int attemptedAuth = 0;
boolean noindex = false;

EthernetClient client;

void setup() {
  pinMode(errorLed, OUTPUT);
  digitalWrite(errorLed, HIGH);
  
#ifdef EnableDHCP
  while (Ethernet.begin(mac) <= 0) {}     // triying to get IP via DHCP
#else
  Ethernet.begin(mac, ip);
#endif 

  server.begin();                        // starts server
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

void protokollErrorHandler(byte e) {
  HTTP();
  switch (e) {
    case 1:
      client.println(F("404 Not Found"));
      Content();

      filename = "404.htm";
      if (SD.exists(filename)) {                      //If no "404.htm" is available on the SD card
        sendfile();                                   // Close file
        break;
      }
      break;
      
    case 2:
      client.println(F("401 Access Denied"));
      client.println(F("WWW-Authenticate: Basic realm=''"));
      attemptedAuth++;
      break;

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
  //client.flush();
  client.stop();                           // shutdown connection
}

/////////////////////////////////////////////////////////////

void ServerShutdown(boolean noreboot) {      // Reboot or shutdown the server
  if (noreboot == true) {
    HTTPHeader();
    client.println(F("<h1>Rebooted</h1>"));
  }
  else {
    HTTPHeader();
    client.println(F("<h1>Shutdown</h1>"));
  }
  client.flush();                          // Clean up some stuff
  client.stop();                           // Close connections
  if (noreboot == false) {
    CPUsleep();
  }
#ifdef EnableDHCP
  Ethernet.maintain();                     // Renew DHCP
#endif
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
File currentfile;

void printDirectory(File dir, int numTabs) {
  dir.rewindDirectory();
  while (true) {
    currentfile =  dir.openNextFile();
    if (! currentfile) {
      currentfile.close();
      break;
    }
    for (unsigned int i = 0; i < numTabs; i++) {
      client.print(F("|-->"));
    }
    client.print(currentfile.name());
    if (currentfile.isDirectory()) {
      client.println(" /");
      client.print(F("</br>"));
      printDirectory(currentfile, numTabs + 1);
    }
    else {
      client.print(F(" | "));
      if (currentfile.size() > 1000) {
        client.print(currentfile.size() / 1024);
        client.print(F(" KiB"));
      }
      else {
        client.print(currentfile.size());
        client.print(F(" byte"));
      }
    }
    currentfile.close();
    client.print(F("</br>"));
  }
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
  client.print(F("Ethernet controller: "));
  if (Ethernet.hardwareStatus() == EthernetW5100) {
    client.print(F("W5100"));
  }
  else if (Ethernet.hardwareStatus() == EthernetW5200) {
    client.print(F("W5200"));
  }
  else if (Ethernet.hardwareStatus() == EthernetW5500) {
    client.print(F("W5500"));
  }
  SpZe(2);
  client.print(F("Uptime(minutes): "));
  client.print(time);
  SpZe(2);
  client.print(rebootcount);
  client.print(F(" Reboot(s)"));
  SpZe(2);
  client.print(attemptedAuth);
  client.print(F(" unsuccessful login attmpts"));
  SpZe(2);
  client.print(F("Problems: "));
  if (noindex == true) {
    client.print(F("index.htm missing"));
  }
  else if (time > 1400 && time / 1400 > rebootcount) {
    client.print(F("Reboot recommended"));
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

boolean sendfile(){
  File sendfile = SD.open(filename, FILE_READ);
  byte fileBuffer[FILEBUF];
  int fileCounter = 0;
  while(sendfile.available())
  {
    fileBuffer[fileCounter] = sendfile.read();
    fileCounter++;
    if(fileCounter > (FILEBUF-1))
    {
      client.write(fileBuffer,FILEBUF);
      fileCounter = 0;
    }
  }
  if(fileCounter > 0){
    client.write(fileBuffer,fileCounter);       //final <64 byte cleanup packet
  } 
  sendfile.close();                           // close file   
}
/////////////////////////////////////////////////////////////

void loop() {
  register int i = 0;
  byte bufoverflow = 0;
  char clientline[BUFSIZ];                  // Array, for saving client request temporary
  client = server.available();

  while (client.connected()) {
    if (client.available()) {
      while (client.available()) {
        clientline[i] = client.read();
        i++;
        if (i >= BUFSIZ){
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
        filename = indexfile;
        if (SD.exists(filename)) {               //if "index.htm" exists on SD card
          HTTPHeader();
          sendfile();
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
          sendfile();
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
      }

#ifdef EnableTRACE
      else if (strstr(clientline, "TRACE ") != 0) {
        HTTP200OK();
        ContentType(1);
        client.println(F("plain"));
        client.write(clientline, BUFSIZ);
      }
#endif

      else
      {
        protokollErrorHandler(5);                            // Request incorrect
      }

      ConnectionStop();                              // Close communication to client
    }
    ConnectionStop();                               // Close communication to client
  }
}
