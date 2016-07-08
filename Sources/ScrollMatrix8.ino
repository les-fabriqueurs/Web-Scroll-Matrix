//////////////////////////////////////////////////////////////////
// Scroll 8 Matrix - Web server
// V2.1 - pierre@frabriqueurs.com
//   - Bip fixed
//
// Inspired by "HTML to LCD Server" v1.1 from Everett Robinson
//
//  Serveur Web permettant de faire defiler un message saisi depuis Inernet
//  sur un afficheur composé de 8 matrices de 8x8 LED (Module d'affichage MAX7219)
//
//
// Matrices nb               : 8 
//
// DIN pin of MAX7219 module : 22
// CS  pin of MAX7219 module : 24
// CLK pin of MAX7219 module : 26
// Buzzer +                  : 9
// Buzzer -                  : GND
//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
// Inclusion des libraries necessaires
//////////////////////////////////////////////////////////////////
#include <MaxMatrix.h>
#include <SPI.h>
#include <Ethernet.h>
#include <string.h>
#include <avr/pgmspace.h>
  
//////////////////////////////////////////////////////////////////
// Parametres du serveur Web (adresse IP, adresse MAC, Passerelle
// et masque sous reseau) 
// Ces parametres dependent de la configuration de votre reseau local
//
// Afin d'acceder au Server Web heberge par l'arduino depuis
// Internet, il est necessaire d'activer le transfert d'adresse (NAT)
// Sur la boxe Internet
//////////////////////////////////////////////////////////////////
byte ip[] = { xxx,xxx,xxx,xxx };
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
byte gateway[] = { xxx,xxx,xxx,xxx };
byte subnet[] = { 255, 255, 255,0 };

// Definition d'une instance de Server Web
EthernetServer server(80);

///////////////////////////////////////////////////////////////////
// Definition des variables de type "chaine de carateres"
///////////////////////////////////////////////////////////////////
String line1 = "                                                   ";
String line2 = "          ";
char   msg1[]= "Yep yep yep ! that's rocks !                       ";
char   msg2[]= "  Signe: ";
char   msg3[]= "          ";
char   msg4[]= "  ...  ";


/////////////////////////////////////////////////////////////////////////
// Tableau de caracteres stoquee en Flash 
// (afin de limiter l'utilisation de la RAM)
// Ce tableau contient le codage permettant d allumer les pixels necessaire
// a l'affichage d un caractere sur une matrice de Leds (une ligne par caracteres)
//
//  1er  element: largeur du caractere en pixels
//  2eme element: hauteur du caractere en pixels
//  3-5eme element: Codage en binaire des pixels a allumer 
///////////////////////////////////////////////////////////////////////////
PROGMEM prog_uchar CH[] = {
3, 8, B00000000, B00000000, B00000000, B00000000, B00000000, // space
1, 8, B01011111, B00000000, B00000000, B00000000, B00000000, // !
3, 8, B00000011, B00000000, B00000011, B00000000, B00000000, // "
5, 8, B00010100, B00111110, B00010100, B00111110, B00010100, // #
4, 8, B00100100, B01101010, B00101011, B00010010, B00000000, // $
5, 8, B01100011, B00010011, B00001000, B01100100, B01100011, // %
5, 8, B00110110, B01001001, B01010110, B00100000, B01010000, // &
1, 8, B00000011, B00000000, B00000000, B00000000, B00000000, // '
3, 8, B00011100, B00100010, B01000001, B00000000, B00000000, // (
3, 8, B01000001, B00100010, B00011100, B00000000, B00000000, // )
5, 8, B00101000, B00011000, B00001110, B00011000, B00101000, // *
5, 8, B00001000, B00001000, B00111110, B00001000, B00001000, // +
2, 8, B10110000, B01110000, B00000000, B00000000, B00000000, // ,
4, 8, B00001000, B00001000, B00001000, B00001000, B00000000, // -
2, 8, B01100000, B01100000, B00000000, B00000000, B00000000, // .
4, 8, B01100000, B00011000, B00000110, B00000001, B00000000, // /
4, 8, B00111110, B01000001, B01000001, B00111110, B00000000, // 0
3, 8, B01000010, B01111111, B01000000, B00000000, B00000000, // 1
4, 8, B01100010, B01010001, B01001001, B01000110, B00000000, // 2
4, 8, B00100010, B01000001, B01001001, B00110110, B00000000, // 3
4, 8, B00011000, B00010100, B00010010, B01111111, B00000000, // 4
4, 8, B00100111, B01000101, B01000101, B00111001, B00000000, // 5
4, 8, B00111110, B01001001, B01001001, B00110000, B00000000, // 6
4, 8, B01100001, B00010001, B00001001, B00000111, B00000000, // 7
4, 8, B00110110, B01001001, B01001001, B00110110, B00000000, // 8
4, 8, B00000110, B01001001, B01001001, B00111110, B00000000, // 9
2, 8, B01010000, B00000000, B00000000, B00000000, B00000000, // :
2, 8, B10000000, B01010000, B00000000, B00000000, B00000000, // ;
3, 8, B00010000, B00101000, B01000100, B00000000, B00000000, // <
3, 8, B00010100, B00010100, B00010100, B00000000, B00000000, // =
3, 8, B01000100, B00101000, B00010000, B00000000, B00000000, // >
4, 8, B00000010, B01011001, B00001001, B00000110, B00000000, // ?
5, 8, B00111110, B01001001, B01010101, B01011101, B00001110, // @
4, 8, B01111110, B00010001, B00010001, B01111110, B00000000, // A
4, 8, B01111111, B01001001, B01001001, B00110110, B00000000, // B
4, 8, B00111110, B01000001, B01000001, B00100010, B00000000, // C
4, 8, B01111111, B01000001, B01000001, B00111110, B00000000, // D
4, 8, B01111111, B01001001, B01001001, B01000001, B00000000, // E
4, 8, B01111111, B00001001, B00001001, B00000001, B00000000, // F
4, 8, B00111110, B01000001, B01001001, B01111010, B00000000, // G
4, 8, B01111111, B00001000, B00001000, B01111111, B00000000, // H
3, 8, B01000001, B01111111, B01000001, B00000000, B00000000, // I
4, 8, B00110000, B01000000, B01000001, B00111111, B00000000, // J
4, 8, B01111111, B00001000, B00010100, B01100011, B00000000, // K
4, 8, B01111111, B01000000, B01000000, B01000000, B00000000, // L
5, 8, B01111111, B00000010, B00001100, B00000010, B01111111, // M
5, 8, B01111111, B00000100, B00001000, B00010000, B01111111, // N
4, 8, B00111110, B01000001, B01000001, B00111110, B00000000, // O
4, 8, B01111111, B00001001, B00001001, B00000110, B00000000, // P
4, 8, B00111110, B01000001, B01000001, B10111110, B00000000, // Q
4, 8, B01111111, B00001001, B00001001, B01110110, B00000000, // R
4, 8, B01000110, B01001001, B01001001, B00110010, B00000000, // S
5, 8, B00000001, B00000001, B01111111, B00000001, B00000001, // T
4, 8, B00111111, B01000000, B01000000, B00111111, B00000000, // U
5, 8, B00001111, B00110000, B01000000, B00110000, B00001111, // V
5, 8, B00111111, B01000000, B00111000, B01000000, B00111111, // W
5, 8, B01100011, B00010100, B00001000, B00010100, B01100011, // X
5, 8, B00000111, B00001000, B01110000, B00001000, B00000111, // Y
4, 8, B01100001, B01010001, B01001001, B01000111, B00000000, // Z
2, 8, B01111111, B01000001, B00000000, B00000000, B00000000, // [
4, 8, B00000001, B00000110, B00011000, B01100000, B00000000, // \ backslash
2, 8, B01000001, B01111111, B00000000, B00000000, B00000000, // ]
3, 8, B00000010, B00000001, B00000010, B00000000, B00000000, // hat
4, 8, B01000000, B01000000, B01000000, B01000000, B00000000, // _
2, 8, B00000001, B00000010, B00000000, B00000000, B00000000, // `
4, 8, B00100000, B01010100, B01010100, B01111000, B00000000, // a
4, 8, B01111111, B01000100, B01000100, B00111000, B00000000, // b
4, 8, B00111000, B01000100, B01000100, B00101000, B00000000, // c
4, 8, B00111000, B01000100, B01000100, B01111111, B00000000, // d
4, 8, B00111000, B01010100, B01010100, B00011000, B00000000, // e
3, 8, B00000100, B01111110, B00000101, B00000000, B00000000, // f
4, 8, B10011000, B10100100, B10100100, B01111000, B00000000, // g
4, 8, B01111111, B00000100, B00000100, B01111000, B00000000, // h
3, 8, B01000100, B01111101, B01000000, B00000000, B00000000, // i
4, 8, B01000000, B10000000, B10000100, B01111101, B00000000, // j
4, 8, B01111111, B00010000, B00101000, B01000100, B00000000, // k
3, 8, B01000001, B01111111, B01000000, B00000000, B00000000, // l
5, 8, B01111100, B00000100, B01111100, B00000100, B01111000, // m
4, 8, B01111100, B00000100, B00000100, B01111000, B00000000, // n
4, 8, B00111000, B01000100, B01000100, B00111000, B00000000, // o
4, 8, B11111100, B00100100, B00100100, B00011000, B00000000, // p
4, 8, B00011000, B00100100, B00100100, B11111100, B00000000, // q
4, 8, B01111100, B00001000, B00000100, B00000100, B00000000, // r
4, 8, B01001000, B01010100, B01010100, B00100100, B00000000, // s
3, 8, B00000100, B00111111, B01000100, B00000000, B00000000, // t
4, 8, B00111100, B01000000, B01000000, B01111100, B00000000, // u
5, 8, B00011100, B00100000, B01000000, B00100000, B00011100, // v
5, 8, B00111100, B01000000, B00111100, B01000000, B00111100, // w
5, 8, B01000100, B00101000, B00010000, B00101000, B01000100, // x
4, 8, B10011100, B10100000, B10100000, B01111100, B00000000, // y
3, 8, B01100100, B01010100, B01001100, B00000000, B00000000, // z
3, 8, B00001000, B00110110, B01000001, B00000000, B00000000, // {
1, 8, B01111111, B00000000, B00000000, B00000000, B00000000, // |
3, 8, B01000001, B00110110, B00001000, B00000000, B00000000, // }
4, 8, B00001000, B00000100, B00001000, B00000100, B00000000, // ~
1, 8, B00000000, B00000000, B00000000, B00000000, B00000000, // DEL (unused)
4, 8, B00111110, B01000001, B11000001, B00100010, B00000000, // Ç  96
4, 8, B00111100, B01000001, B01000000, B01111100, B00000000, // ü  97
4, 8, B00111000, B01010110, B01010101, B00011000, B00000000, // é  98
4, 8, B00100000, B01010110, B01010101, B01111010, B00000000, // â  99
4, 8, B00100000, B01010101, B01010100, B01111001, B00000000, // ä  100
4, 8, B00100000, B01010101, B01010110, B01111000, B00000000, // à  101
4, 8, B00100000, B01010100, B01010100, B01111000, B00000000, // å  102
4, 8, B00111010, B01010101, B01010101, B00011010, B00000000, // ê  103
4, 8, B00111000, B01010101, B01010100, B00011001, B00000000, // ë  104
4, 8, B00111000, B01010101, B01010110, B00011000, B00000000, // è  105
3, 8, B01000101, B01111100, B01000001, B00000000, B00000000, // ï  106
3, 8, B01000110, B01111101, B01000010, B00000000, B00000000  // î  107
};

//////////////////////////////////////////////////////////////////////////
// Declaration d'autres variables globales
////////////////////////////////////////////////////////////////////////
int data = 22;     // Pin Arduino connecte a Pin DIN de la premiere martice de leds
int load = 24;     // Pin Arduino connecte a Pin CS des martices de leds
int clock = 26;    // Pin Arduino connecte a Pin CLK des martices de leds
int maxInUse = 8;  // Nombre de matrices de Led utilisees 

int buzzpin = 9;    // Pin Arduino connecte au buzzer  (l'autre est connecte a la masse)
byte buffer[10];

// Instanciation d'un module MaxMatrix (utilise pour piloter les dites matrices)
MaxMatrix m(data, load, clock, maxInUse); // 




////////////////////////////////////////////////////////////////
// Fonctions d affichage d'un texte deroulant sur les Matrices
//
////////////////////////////////////////////////////////////////
void printCharWithShift(char c, int shift_speed){
  int char_indice = 0;
  Serial.print("c1 = ");
  Serial.println(c);
  Serial.print("c1 hexa = "); 
  Serial.println(c,HEX);
  
// Switch pour gestion des caracteres accentués
  switch (c) {
    case 0XFFFFFFC3:
      break;
    case 0XFFFFFFA0 :
       char_indice= 101; // à
      break;
   case 0XFFFFFFA9:
        char_indice=98; // é
      break;
   case 0XFFFFFFA8 :
        char_indice=105; // è
      break; 
   case 0XFFFFFFAA:
        char_indice=103; // ê
      break;
   case 0XFFFFFFAB :
        char_indice=104; // ë
      break;
   case 0XFFFFFFA4 :
        char_indice=100; // ä
      break; 
   case 0XFFFFFFAE:
        char_indice=107; // î
      break;
   case 0XFFFFFFA2:
        char_indice=99; // â
      break;
   case 0XFFFFFFAF:
        char_indice=106; // ï     
  }
    
  if ((c < 32)&&(char_indice == 0)) return;
  if ((c < 127)&&(char_indice == 0)) char_indice = c - 32;
 
  memcpy_P(buffer, CH + 7*char_indice, 7);
  m.writeSprite(64, 0, buffer);
  m.setColumn(64 + buffer[0], 0);
  
  for (int i=0; i<buffer[0]+1; i++) 
  {
    delay(shift_speed);
    m.shiftLeft(false, false);
  }
}

void printStringWithShift(char* s, int shift_speed){
  while (*s != 0){
    printCharWithShift(*s, shift_speed);
    s++;
  }
}


//////////////////////////////////////////////////////////////////
////////////          Fonction bip        /////////////////////
//////////////////////////////////////////////////////////////////
void bip(int targetPin, long length) {
  digitalWrite(targetPin,HIGH);
  delay(length);
  digitalWrite(targetPin,LOW);
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Code specifique au serveur Web
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

// Affichage de l entete HTML
//////////////////////////////////////
void HtmlHeader(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html;charset=UTF-8");
  client.println("");
  client.println("<HTML>\n<HEAD>");
  client.println("</HEAD><BODY>");
}

// Affichage du pied de page de la page HTML
/////////////////////////////////////////////
void HtmlFooter(EthernetClient client) {
  client.println("</BODY></HTML>");
}

// Cette fonction tranforme, les caracteres particulier que le HTML formate en 
// chaine non lisible, en caractere lisible 
/////////////////////////////////////////////////////////////////
void htmlToHuman(String URLstring) {
  int indexOfDelim = 0;  //This variable stores the location of our delimiter so we can find where line1 ends and line2 begins
  
  // The following array stores a list of ugly html codes, and the special charaters they represent (for changing them back)
  const String CHAR_CONVERSIONS[40][2] = {{"+"," "},{"%40","@"},{"%23","#"},{"%24","$"},{"%2B","+"},{"%21","!"},{"%7E","~"},
  {"%3A",":"},{"%3B",";"},{"%2C",","},{"%3F","?"},{"%2F","/"},{"%7C","|"},{"%5E","^"},
  {"%5C","\\"},{"%7B","{"},{"%7D","}"},{"%5B","["},{"%5D","]"},{"%3C","<"},{"%3E",">"},
  {"%28","("},{"%29",")"},{"%27","'"},{"%22","\""},{"%3F","?"},{"%26","&"},{"%3D","="},
  {"%25","%"},{"%C3%A9","é"},{"%C3%A8","è"},{"%C3%AA","ê"},{"%C3%A7","ç"},{"%C3%A0","à"},
  {"%27","'"},{"%C3%A4","ä"},{"%C3%AB","ë"},{"%C3%AE","î"},{"%C3%AF","ï"},{"%C3%A2","â"}};
  
  URLstring.replace("L1=","");     // remove the unnecessary field variable names
  URLstring.replace("&L2=","`");   // and turn this one into our delimiter character ` (The one on the ~ key)
  
  //A for loop that replaces all the html codes with the right symbols
  for (int i=0 ; i < 40; i++) {
    URLstring.replace(CHAR_CONVERSIONS[i][0],CHAR_CONVERSIONS[i][1]);
  }
  
  indexOfDelim = URLstring.indexOf("`");       // find the index of that delimiter
  
  line1 = URLstring.substring(0,indexOfDelim); // set line1 and line 2 using that knowledge
  line2 = URLstring.substring(indexOfDelim+1,URLstring.length());

  return;
}


// Parse an HTTP request header one character at a time, seeking string variables (modified from Kevin Haw's code)
void ParseHttpHeader(EthernetClient &client) {
  char c;
  int i = 0;    //An integer use to limit the size of rawUrlText (prevents crashing due to running out of memory)
  String rawUrlText = "";
  
  // Skip through until we hit a question mark (first one)
  while((c = client.read()) != '?' && client.available()) {
    // Debug - print data
    Serial.print(c);
  }
  
  // Are we here for a question mark or did we run out of data?
  if(client.available() > 2) {  
    // Read the data and add it to our unmodified string rawURLText!
    // the incrementer limits the input to about 1 line of plain text and 1/2 line of symbols, more causes line2 to truncate
    while((c = client.read()) != ' ' && client.available() && i < 70) {
      rawUrlText = rawUrlText + c;
      Serial.print(c);
      i++;
    }
  
    htmlToHuman(rawUrlText);            // Make it readable
  
    // Conv string 2 char*
    line1.toCharArray(msg1,50);
    line2.toCharArray(msg3,10);
    
    bip(buzzpin, 1000); // bip  1 sec  
  }
  
  return;
}



// Set up webserver functionality (from Kevin Haw)
void WebServerSetup() {
      Ethernet.begin(mac, ip);
      server.begin();
}

//
// Web server loop (modified from Kevin Haw's code)
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void WebServerLoop() {
  EthernetClient client = server.available();
  boolean bPendingHttpResponse = false; // True when we've received a whole HTTP request and need to output the webpage
  boolean bPreventSecondParse = false;  //Stops a second successive run of parseHttpHeader(), this prevents some nasty bugs.

  if (client) {
    // Loop as long as there's a connection
    while (client.connected()) {
      // Do we have pending data (an HTTP request) and is this the first parse?
      if (client.available() && !bPreventSecondParse) {
        // Indicate we need to respond to the HTTP request as soon as we're done processing it
        bPendingHttpResponse = true;
        bPreventSecondParse = true;

        ParseHttpHeader(client);
      }
      else {
        // There's no data waiting to be read in on the client socket.  Do we have a pending HTTP request?
        if(bPendingHttpResponse) {
          // Yes, we have a pending request.  Clear the flag and then send the webpage to the client
          bPendingHttpResponse = false;
          bPreventSecondParse = false;

          // send a standard http response header and HTML header
          HtmlHeader(client);

          client.println("<H2>La Matrice LED Arduino du Salon:</H2>");
          client.println("<b>Texte affiché:</b> " + line1 + "<br />");
          client.println("<b>Signé:</b> " + line2 + "<br /><br />");
          client.println("<H2>Afficher autre chose!</H2>");
          client.println("<form action=\"/?\" method=get>");
          client.println("<b>Message: </b><input type=\"text\" name=\"L1\" maxlength=\"50\" size=\"50\" /><br />");
          client.println("<b>From: </b><input type=\"text\" name=\"L2\" maxlength=\"10\" size=\"10\" /><br />");
          client.println("<input type=\"submit\" value=\"Submit\" /></form>");

          // send HTML footer
          HtmlFooter(client);

          // give the web browser time to receive the data
          delay(1);
          client.stop();
        }
      }
    }  // End while(connected)
  }
}


//-------------------------------------------------------------
//---- Main ---------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------

void setup(){  
  // declare pin 9 to be an output for Buzzer:
  pinMode(buzzpin, OUTPUT);
  
  // open the serial port at 9600 bps:
  Serial.begin(9600);
  
  // Web server init
  WebServerSetup();
  
  // Matrix init
  m.init(); // module initialize
  m.setIntensity(7); // dot matix intensity 0-15
}


void loop(){  

  WebServerLoop();
  
  // print the active sentences on Matrix
  printStringWithShift(msg1, 100);
  printStringWithShift(msg2, 100);
  printStringWithShift(msg3, 100);
  printStringWithShift(msg4, 100);      
}


