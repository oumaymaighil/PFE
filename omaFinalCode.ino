// include de obstacle code 
  #include <Servo.h>          //Servo motor library. This is standard library
  #include <NewPing.h>        //Ultrasonic sensor function library. You must install this library
  #include <LiquidCrystal_I2C.h>
// end include de obstacle code 
/* ---------------------------------------- */
// include de code IOT
  #include <SPI.h> //Serial Peripheral Interface
  #include <Ethernet.h>
// end include de code IOT
/* ---------------------------------------- */
// pin of Moteur with driver
  #define ENA 9
  #define ENB 11
  const int moteurGaucheA = 8;
  const int moteurGaucheR = 7;
  const int moteurDroitA = 6;
  const int moteurDroitR = 5;
// end moteur 
/* ---------------------------------------- */
// pin de suiveur de ligne 
  const int capteurGauche = A2;
  const int capteurDroit = A3;
  bool etatCapteurGauche;
  bool etatCapteurDroit;
// end pin de suiveur de ligne
/* ---------------------------------------- */
// pin de detecteur de coleur 
  #define s0 10 // Module pins wiring
  #define s1 2
  #define s2 12
  #define s3 13
  #define sensorOut 4
  int Red = 0, Blue = 0, Green = 0; // RGB values
  int frequency = 0;
// end pin de detecteur de coleur 
/* ---------------------------------------- */
// pin et initialisation des obstacle code
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  //sensor pins
  #define trig_pin A1 //analog input 1
  #define echo_pin A0 //analog input 2
  #define maximum_distance 250
  boolean goesForward = false;
  int distance = 150;

  NewPing sonar(trig_pin, echo_pin, maximum_distance); //sensor function
// end pin et initialisation des obstacle code
/* ---------------------------------------- */
// pin et initialisation de IOT code

  int speed = 100;

  byte mac[] = {0x20, 0x47, 0x47, 0xBA, 0x57, 0xE6}; // physical mac address
  byte ip[] = {192, 168, 137, 10};                   // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
  byte gateway[] = {192, 168, 1, 1};                 // internet access via router
  byte subnet[] = {255, 255, 255, 0};                // subnet mask

  EthernetServer server(80); // server port
  String readString;
// end pin et initialisation de IOT code
/* ---------------------------------------- */
Servo servo; // commun servo IOT code and obstacle code
#define servo_pin 3
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void setup() {
  Serial.begin(9600);
  // Initialisation des broches des moteurs
    pinMode(moteurGaucheA, OUTPUT);
    pinMode(moteurGaucheR, OUTPUT);
    pinMode(moteurDroitA, OUTPUT);
    pinMode(moteurDroitR, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitA, LOW);
    digitalWrite(moteurDroitR, LOW);
    digitalWrite(ENB, HIGH);
    digitalWrite(ENA, HIGH);
  // end Initialisation des broches des moteurs 
  /* ----------------------------------------------------------------------------- */
  // initialisation de capteur de suiveur de ligne
    pinMode(capteurGauche, INPUT);
    pinMode(capteurDroit, INPUT);   
  // end initialisation de capteur de suiveur de ligne
  /* ----------------------------------------------------------------------------- */
  // initialisation des resource de obstacle code 
    // lcd init 
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Distance:");
    lcd.setCursor(0, 1);
    lcd.print("Mouvement:");
    // end lcd init 
    
    servo.attach(3); //our servo pin

    servo.write(115);
    delay(2000);
    distance = readPing();
    delay(100);
    distance = readPing();
    delay(100);
    distance = readPing();
    delay(100);
    distance = readPing();
    delay(100);
  // end initialisation des resource de obstacle code
  /* ----------------------------------------------------------------------------- */
  // initialisation des pin de detecteur de coleur 
    pinMode(s0, OUTPUT); // pin modes
    pinMode(s1, OUTPUT);
    pinMode(s2, OUTPUT);
    pinMode(s3, OUTPUT);
    pinMode(sensorOut, INPUT);
    digitalWrite(s0, HIGH); // Putting S0/S1 on HIGH/HIGH levels means the output frequency scalling is at 100% (recommended)
    digitalWrite(s1, HIGH); // LOW/LOW is off HIGH/LOW is 20% and LOW/HIGH is  2%
  // end initialisation des pin de detecteur de coleur 
  /* ----------------------------------------------------------------------------- */
  // intialisation de code IOT 
    // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip, gateway, subnet);
    server.begin();
    Serial.begin(9600);
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
    // command servo motor
    servo.attach(servo_pin);
    servo.write(0);  
  // end intialisation de code IOT 
}

void loop() {
   modeSuiveur();
  //obstacleMode();
   //detecteurColeurMode();
  // IOTCodeMode();
}

// start +moteur mouvement function+ 
  void avancer()
  {
    digitalWrite(moteurGaucheA, HIGH);
    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitA, HIGH);
    digitalWrite(moteurDroitR, LOW);
    digitalWrite(ENB, HIGH);
    digitalWrite(ENA, HIGH);
  }
  void gauche()
  {
    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitA, HIGH);
    digitalWrite(moteurDroitR, LOW);
    digitalWrite(ENB, HIGH);
    digitalWrite(ENA, HIGH);
  }
  void droite()
  {
    digitalWrite(moteurGaucheA, HIGH);
    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitA, LOW);
    digitalWrite(moteurDroitR, LOW);
    digitalWrite(ENB, HIGH);
    digitalWrite(ENA, HIGH);
  }
  void reculer()
  {
    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(moteurGaucheR, HIGH);
    digitalWrite(moteurDroitA, LOW);
    digitalWrite(moteurDroitR, HIGH);
    digitalWrite(ENB, HIGH);
    digitalWrite(ENA, HIGH);
  }
  void arreter()
  {
    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitA, LOW);
    digitalWrite(moteurDroitR, LOW);
    digitalWrite(ENB, HIGH);
    digitalWrite(ENA, HIGH);
  }
// end +moteur mouvement function+ 
/* ----------------------------------------------------------------------------- */
// main function of ++suiveur de ligne++ 
  void modeSuiveur()
  {
    etatCapteurGauche = digitalRead(capteurGauche);

    etatCapteurDroit = digitalRead(capteurDroit);

    if ((etatCapteurGauche) && (etatCapteurDroit)) // Si le capteur du centre détecte du noir
    {
      if ((etatCapteurGauche) && (!etatCapteurDroit)) // S'il y a du noir à gauche et du blanc à droite, tourner à gauche
        gauche();
      else if ((!etatCapteurGauche) && (etatCapteurDroit)) // S'il y a du blanc à gauche et du noir à droite, tourner à droite
        droite();
      else // Si les conditions plus haut ne s'appliquent pas, continuer tout droit
        avancer();
    }
    else // Si le capteur du centre détecte du blanc
    {
      if ((etatCapteurGauche) && (!etatCapteurDroit)) // S'il y a du noir à gauche et du blanc à droite, tourner à gauche
        gauche();
      else if ((!etatCapteurGauche) && (etatCapteurDroit)) // S'il y a du blanc à gauche et du noir à droite, tourner à droite
        droite();
      else // Si les conditions plus hautdne s'appliquent pas, reculer
        reculer();
    }
  }
// end main function of suiveur de ligne 
/* ----------------------------------------------------------------------------- */
// main fonction de ++obstacle code++
  void obstacleMode(){
    int distanceRight = 0;
    int distanceLeft = 0;
    delay(50);

    if (distance <= 20){
      moveStop();
      delay(300);
      moveBackward();
      delay(400);
      moveStop();
      delay(300);
      distanceRight = lookRight();
      delay(300);
      distanceLeft = lookLeft();
      delay(300);

      if (distance >= distanceLeft){
        turnRight();
        moveStop();
      }
      else{
        turnLeft();
        moveStop();
      }
    }
    else{
      moveForward(); 
    }
      distance = readPing();
      // Affichage de la distance sur l'afficheur LCD
    lcd.setCursor(10, 0);
    lcd.print(distance);
    lcd.print(" cm");
  }
// end main fonction de obstacle code
/* ----------------------------------------------------------------------------- */
// fonction suplementaire de ++obstacle Mode++
  int lookRight()
  {
    servo.write(50);
    delay(500);
    int distance = readPing();
    delay(100);
    servo.write(115);
    return distance;
  }
  int lookLeft()
  {
    servo.write(170);
    delay(500);
    int distance = readPing();
    delay(100);
    servo.write(115);
    return distance;
    delay(100);
  }
  int readPing()
  {
    delay(70);
    int cm = sonar.ping_cm();
    if (cm == 0)
    {
      cm = 250;
    }
    return cm;
  }
  void moveStop()
  {

    digitalWrite(moteurDroitA, LOW);
    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitR, LOW);
    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(ENB, HIGH);
    digitalWrite(ENA, HIGH);
    lcd.setCursor(10, 1);
    lcd.print("Stop   ");
  }
  void moveForward()
  {

    if (!goesForward)
    {

      Serial.println("==moveForward==");

      goesForward = true;

      digitalWrite(moteurGaucheR, HIGH);
      digitalWrite(moteurDroitA, HIGH);

      digitalWrite(moteurGaucheA, LOW);
      digitalWrite(moteurDroitR, LOW);
      digitalWrite(ENB, HIGH);
      digitalWrite(ENA, HIGH);
      lcd.setCursor(10, 1);
      lcd.print("Forward   ");
    }
  }
  void moveBackward()
  {

    goesForward = false;

    Serial.println("==moveBackward==");

    digitalWrite(moteurGaucheA, HIGH);
    digitalWrite(moteurDroitR, HIGH);

    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitA, LOW);
    digitalWrite(ENB, HIGH);
    digitalWrite(ENA, HIGH);
    lcd.setCursor(10, 1);
    lcd.print("Backward   ");
  }
  void turnRight()
  {

    Serial.println("==turnRight==");

    digitalWrite(moteurGaucheR, HIGH);
    digitalWrite(moteurDroitR, HIGH);

    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(moteurDroitA, LOW);
    digitalWrite(ENB, HIGH);
    digitalWrite(ENA, HIGH);
    lcd.setCursor(10, 1);
    lcd.print("Right   ");

    delay(500);
    digitalWrite(ENB, HIGH);
    digitalWrite(ENA, HIGH);
    digitalWrite(moteurGaucheR, HIGH);
    digitalWrite(moteurDroitA, HIGH);

    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(moteurDroitR, LOW);
    lcd.setCursor(10, 1);
    lcd.print("Right   ");
  }
  void turnLeft()
  {

    Serial.println("==turnLeft==");

    digitalWrite(moteurGaucheA, HIGH);
    digitalWrite(moteurDroitA, HIGH);

    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitR, LOW);
    lcd.setCursor(10, 1);
    lcd.print("Left   ");
    delay(500);

    digitalWrite(moteurGaucheR, HIGH);
    digitalWrite(moteurDroitA, HIGH);

    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(moteurDroitR, LOW);
    lcd.setCursor(10, 1);
    lcd.print("Left   ");
  }
// end fonction suplementaire de obstacle Mode
/* ----------------------------------------------------------------------------- */
// main fonction de ++detecteur de coleur++ 
  void detecteurColeurMode(){
      frequency = pulseIn(sensorOut, LOW);
    // Printing the value on the serial monitor
    // Serial.print("B= ");//printing name
    // Serial.print(frequency);//printing RED color frequency
    // Serial.println("  ");
    // delay(1200);
    frequency = pulseIn(sensorOut, LOW);
    // Printing the value on the serial monitor
    // Serial.print("R= ");//printing name
    // Serial.print(frequency);//printing RED color frequency
    // Serial.println("  ");
    // delay(1200);
    frequency = pulseIn(sensorOut, LOW);
    // Printing the value on the serial monitor
    // Serial.print("G= ");//printing name
    // Serial.print(frequency);//printing RED color frequency
    // Serial.println("  ");
    // delay(1200);

    GetColors(); // Execute the GetColors function to get the value of each RGB color
                // Depending of the RGB values given by the sensor we can define the color and displays it on the monitor

    if (Red <= 15 && Green <= 15 && Blue <= 15)
    { // If the values are low it's likely the white color (all the colors are present)
      Serial.println("White");
      delay(1200);
    }
    else if (frequency > 320 && frequency < 350)
    { // if Red value is the lowest one and smaller thant 23 it's likely Red
      Serial.println("Red");

      Serial.println("==Backward==");
      digitalWrite(moteurGaucheA, HIGH);
      digitalWrite(moteurDroitA, LOW);
      digitalWrite(moteurGaucheR, LOW);
      digitalWrite(moteurDroitR, HIGH);
      digitalWrite(ENB, HIGH);
      digitalWrite(ENA, HIGH);
      delay(1000);
      digitalWrite(moteurGaucheA, LOW);
      digitalWrite(moteurDroitA, LOW);
      digitalWrite(moteurGaucheR, LOW);
      digitalWrite(moteurDroitR, LOW);
      digitalWrite(ENB, HIGH);
      digitalWrite(ENA, HIGH);
      delay(1200);
    }
    else if (frequency > 199 && frequency < 250)
    { // Same thing for Blue
      Serial.println("Blue");

      Serial.println("==turnLeft==");
      digitalWrite(moteurGaucheA, LOW);
      digitalWrite(moteurDroitA, LOW);
      digitalWrite(moteurGaucheR, HIGH);
      digitalWrite(moteurDroitR, HIGH);
      digitalWrite(ENB, HIGH);
      digitalWrite(ENA, HIGH);
      delay(1000);
      digitalWrite(moteurGaucheA, LOW);
      digitalWrite(moteurDroitA, LOW);
      digitalWrite(moteurGaucheR, LOW);
      digitalWrite(moteurDroitR, LOW);
      digitalWrite(ENB, HIGH);
      digitalWrite(ENA, HIGH);
      delay(1200);
    }
    else if (frequency > 139 && frequency < 150)
    {                          // Green it was a little tricky, you can do it using the same method as above (the lowest), but here I used a reflective object
      Serial.println("Green"); // which means the blue value is very low too, so I decided to check the difference between green and blue and see if it's acceptable
      Serial.println("==turnRight==");

      digitalWrite(moteurGaucheR, LOW);
      digitalWrite(moteurDroitR, LOW);
      digitalWrite(moteurGaucheA, HIGH);
      digitalWrite(moteurDroitA, HIGH);
      digitalWrite(ENB, HIGH);
      digitalWrite(ENA, HIGH);
      delay(1000);
      digitalWrite(moteurGaucheA, LOW);
      digitalWrite(moteurDroitA, LOW);
      digitalWrite(moteurGaucheR, LOW);
      digitalWrite(moteurDroitR, LOW);
      digitalWrite(ENB, HIGH);
      digitalWrite(ENA, HIGH);
      delay(1200);
    }  
  }
// end main fonction de detecteur de coleur 
/* ----------------------------------------------------------------------------- */
// fonction suplementaire de ++detecteur de coleur++
  void GetColors()
  {
    digitalWrite(s2, LOW); // S2/S3 levels define which set of photodiodes we are using LOW/LOW is for RED LOW/HIGH is for Blue and HIGH/HIGH is for green
    digitalWrite(s3, LOW);
    Red = pulseIn(sensorOut, digitalRead(sensorOut) == HIGH ? LOW : HIGH); // here we wait until "out" go LOW, we start measuring the duration and stops when "out" is HIGH again, if you have trouble with this expression check the bottom of the code
    delay(20);
    digitalWrite(s3, HIGH); // Here we select the other color (set of photodiodes) and measure the other colors value using the same techinque
    Blue = pulseIn(sensorOut, digitalRead(sensorOut) == HIGH ? LOW : HIGH);
    delay(20);
    digitalWrite(s2, HIGH);
    Green = pulseIn(sensorOut, digitalRead(sensorOut) == HIGH ? LOW : HIGH);
    delay(20);
  }
// end fonction suplementaire de detecteur de coleur
/* ----------------------------------------------------------------------------- */
// main function de ++IOT code++
  void IOTCodeMode(){
      // Create a client connection

    EthernetClient client = server.available();
    if (client)
    {
      while (client.connected())
      {
        if (client.available())
        {
          char c = client.read();

          // read char by char HTTP request
          if (readString.length() < 100)
          {
            // store characters to string
            readString += c;
            // Serial.print(c);
          }

          // if HTTP request has ended
          if (c == '\n')
          {
            Serial.println(readString); // print to serial monitor for debuging

            client.println("HTTP/1.1 200 OK"); // send new page
            client.println("Content-Type: text/html");
            client.println();
            // Style
            client.println("<style>");
            client.println("div {");
            client.println("display: flex;flex-direction: row;justify-content: space-around;align-items: center; margin: 0px;");
            client.println("}");
            client.println("button{");
            client.println("background-color: #ffffff;color: #333333; border: none;border-radius: 8px;cursor: pointer; font-size: 24px; padding: 0px; width: 150px; text-align: center;");
            client.println("}");
            client.println(" body {");
            client.println("background-color: #37474f;   font-family: Helvetica Neue, Helvetica, Arial, sans-serif; font-size: 16px;margin: 0; padding: 0;color: #ffffff;");
            client.println("}");
            client.println("h1{");
            client.println("  font-size: 48px;color: #ffffff;text-align: center; margin: 0px 0px;");
            client.println("}");
            client.println("img{");
            client.println("  width: 25% ; margin-bottom:10px;border-radius: 5px;  ");
            client.println("}");
            client.println("</style>");
            // end Style
            client.println("</HEAD>");
            client.println("<BODY>");
            client.println("<center>");

            client.println("<H1>PROJET DE FIN D'ETUDE</H1>");
            client.println("<hr/>"); // horizontal line

            client.println("<H2>ROBOT CAR</H2>");
            client.println("<h3>Use the buttons to control the robot car:</h3>");
            // img part
            client.println("<img src=https://th.bing.com/th/id/R.21cc4be2e7f2f9bcbe97893e2dfc4b06?rik=0HM6aSEsEQV0lA&pid=ImgRaw&r=0 alt=robot>");
            // end img part
            // start command section
            client.println("<table>");
            client.println("<tr>");
            client.println("<td>");
            client.println("<td>");
            client.println("<button><a href=\"/?forward\"\">forward</a></button>");
            client.println("<td>");
            client.println("</td>");
            client.println("</td>");
            client.println("</td>");
            client.println("</tr>");
            client.println("<tr>");
            client.println("<td>");
            client.println("<button><a href=\"/?right\"\">right</a></button>");
            client.println("</td>");
            client.println("<td>");
            client.println("<button><a href=\"/?stop\"\">stop</a></button>");
            client.println("</td>");
            client.println("<td>");
            client.println("<button><a href=\"/?left\"\">left</a></button> ");
            client.println("</td>");
            client.println("</tr>");
            client.println("<tr>");
            client.println("<td>");
            client.println("<td>");
            client.println("<button><a href=\"/?backward\"\">backward</a></button>");
            client.println("<td>");
            client.println("</td>");
            client.println("</td>");
            client.println("</td>");
            client.println("</tr>");
            // end command section
            client.println("</BODY>");
            //*******
            delay(1);
            // stopping client
            client.stop();
            // controls the Arduino if you press the buttons
            if (readString.indexOf("?forward") > 0)
            {
              Serial.println("forward");
              forward(speed);
            }
            else if (readString.indexOf("?backward") > 0)
            {
              Serial.println("back");
              backward(speed);
            }
            else if (readString.indexOf("?left") > 0)
            {
              Serial.println("left");
              left(speed);
            }
            else if (readString.indexOf("?right") > 0)
            {
              Serial.println("right");
              right(speed);
            }
            else if (readString.indexOf("?stop") > 0)
            {
              Serial.println("stop");
              stop();
            }

            // clearing string for next read
            readString = "";
            // *********************
          } // if (c == '\n')
        }   // if (client.available())
      }     // while (client.connected())
    }       // if client
  }
// end main function de IOT code
// fonction suplementaire de ++IOT code++ 
  // define Robot move function
  // speed :   0 --> 255
  void forward(int speed)
  {
    // move direction control
    digitalWrite(moteurGaucheA, HIGH);
    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitA, HIGH);
    digitalWrite(moteurDroitR, LOW);
    // speed control
    analogWrite(ENB, speed);
    analogWrite(ENA, speed);
  }
  void backward(int speed)
  {
    // move direction control
    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(moteurGaucheR, HIGH);
    digitalWrite(moteurDroitA, LOW);
    digitalWrite(moteurDroitR, HIGH);
    // speed control
    analogWrite(ENB, speed);
    analogWrite(ENA, speed);
  }
  void right(int speed)
  {
    // move direction control
    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(moteurGaucheR, HIGH);
    digitalWrite(moteurDroitA, LOW);
    digitalWrite(moteurDroitR, HIGH);
    // speed control
    analogWrite(ENB, speed);
    analogWrite(ENA, speed);
  }
  void left(int speed)
  {
    // move direction control
    digitalWrite(moteurGaucheA, HIGH);
    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitA, HIGH);
    digitalWrite(moteurDroitR, LOW);
    // speed control
    analogWrite(ENB, speed);
    analogWrite(ENA, speed);
  }
  void stop()
  {
    // move direction control
    digitalWrite(moteurGaucheA, LOW);
    digitalWrite(moteurGaucheR, LOW);
    digitalWrite(moteurDroitA, LOW);
    digitalWrite(moteurDroitR, LOW);
    // speed control
    analogWrite(ENB, HIGH);
    analogWrite(ENA, HIGH);
  }
// end fonction suplementaire de IOT code 