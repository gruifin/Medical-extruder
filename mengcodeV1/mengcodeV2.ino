int enablePin = 4;
int resetPin = 10;

int mode0Pin = 5; // use pull up when connecting to driver
int mode1Pin = 6; // use pull up when connecting to driver
int mode2Pin = 7; // use pull up when connecting to driver

int dirPin1 = 2;  // direction pin for motor 1
int dirPin2 = 4;  // direction pin for motor 2
int dirPin3 = 6;  // direction pin for motor 3
int dirPin4 = 8;  // direction pin for motor 4

int stepPin1 = 3;  //stepperpin for motor 1
int stepPin2 = 5; //stepperpin for motor 1
int stepPin3 = 7; //stepperpin for motor 1
int stepPin4 = 9; //stepperpin for motor 1

int steps1;  // global var for steps taken by motor 1
int steps2; // global var for steps taken by motor 1
int steps3; // global var for steps taken by motor 1
int steps4; // global var for steps taken by motor 1

//Ludo rewrite notes: 1
//De code ziet er over het algemeen goed uit
//Maar er word veel te veel herhaald. Alle stap-fucnties kunnen worden verwerkt tot een fucntie.

//new function stepper-controlers
void stepper(int motor, int direction, int steps)
{
    //preset the motor direction
    if(direction)
      digitalRead(motor, HIGH);
    else
      digitalRead(motor, HIGH);

    //perform x out of 400 steps
    for(int x = steps; x < 0; x--)
    {
      
    }

}


void step_down1()// function that let's the motor1 take 1 step downwards
{
  digitalWrite(dirPin1, LOW); //Changes the rotations direction
  // Makes 400 pulses for making one full cycle rotation
  for (int x = 0; x < 400; x++) {
    digitalWrite(stepPin1, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin1, LOW);
    delayMicroseconds(500);
  }
  steps1--;
}

void step_up1()// function that let's the motor1 take 1 step up
{
  digitalWrite(dirPin1, HIGH); //Changes the rotations direction
  // Makes 400 pulses for making one full cycle rotation
  for (int x = 0; x < 400; x++)
  {
    digitalWrite(stepPin1, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin1, LOW);
    delayMicroseconds(500);
  }
  steps1++;

}

void step_down2()// function that let's the motor2 take 1 step downwards
{
  digitalWrite(dirPin2, LOW); //Changes the rotations direction
  // Makes 400 pulses for making one full cycle rotation
  for (int x = 0; x < 400; x++) {
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(500);
  }
  steps2--;
}

void step_up2()// function that let's the motor2 take 1 step up
{
  digitalWrite(dirPin2, HIGH); //Changes the rotations direction
  // Makes 400 pulses for making one full cycle rotation
  for (int x = 0; x < 400; x++)
  {
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(500);
  }
  steps2++;

}

void step_down3()// function that let's the motor3 take 1 step downwards
{
  digitalWrite(dirPin3, LOW); //Changes the rotations direction
  // Makes 400 pulses for making one full cycle rotation
  for (int x = 0; x < 400; x++) {
    digitalWrite(stepPin3, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin3, LOW);
    delayMicroseconds(500);
  }
  steps3--;
}

void step_up31()// function that let's the motor3 take 1 step up
{
  digitalWrite(dirPin3, HIGH); //Changes the rotations direction
  // Makes 400 pulses for making one full cycle rotation
  for (int x = 0; x < 400; x++)
  {
    digitalWrite(stepPin3, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin3, LOW);
    delayMicroseconds(500);
  }
  steps3++;

}

void step_down4()// function that let's the motor4 take 1 step downwards
{
  digitalWrite(dirPin4, LOW); //Changes the rotations direction
  // Makes 400 pulses for making one full cycle rotation
  for (int x = 0; x < 400; x++) {
    digitalWrite(stepPin1, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin4, LOW);
    delayMicroseconds(500);
  }
  steps4--;
}

void step_up4()// function that let's the motor4 take 1 step up
{
  digitalWrite(dirPin4, HIGH); //Changes the rotations direction
  // Makes 400 pulses for making one full cycle rotation
  for (int x = 0; x < 400; x++)
  {
    digitalWrite(stepPin4, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin4, LOW);
    delayMicroseconds(500);
  }
  steps4++;

}

//Ludo rewrite notes: 2
//De mengfucntie ziet er niet heel overzichtelijk uit
//Het is netter om een tweedimensionaal array te gerbruiken voor deze gehel berekening!!!
//
// array 2 demensionaal
// [0] mengkamer [*] [buffer]
// [1] Extruder  [A] [buffer]
// [2] Extruder  [B] [buffer]
// [3] Extruder  [C] [buffer]
// [4] Extruder  [D] [buffer]
//
//Verder lijkt het mij ook handig om wat simpelere benoemingen te gebruiken

int percentage_mengen(int vloeistof1,int vloeistof2,int vloeistof3,int vloeistof4)
{
  int steps_total;
  int uitloop_nozzle;
  int vloeistof_oud1;
  int vloeistof_oud2;
  int vloeistof_oud3;
  int vloeistof_oud4;
  int inhoud_mengkamer;

  steps_total=steps1+steps2+steps3+steps4;

  if (steps_total>inhoud_mengkamer)
  {
    uitloop_nozzle=1;
  }
  if (steps_total<inhoud_mengkamer)
  {
    uitloop_nozzle=0;
  }
  vloeistof1=steps1-vloeistof_oud1*uitloop_nozzle/(steps_total-uitloop_nozzle*steps_total);
  vloeistof2=steps2-vloeistof_oud2*uitloop_nozzle/(steps_total-uitloop_nozzle*steps_total);
  vloeistof3=steps3-vloeistof_oud3*uitloop_nozzle/(steps_total-uitloop_nozzle*steps_total);
  vloeistof4=steps4-vloeistof_oud4*uitloop_nozzle/(steps_total-uitloop_nozzle*steps_total);

  return vloeistof1; return vloeistof2; return vloeistof3; return vloeistof4;

  vloeistof_oud1=vloeistof1;
  vloeistof_oud2=vloeistof2;
  vloeistof_oud3=vloeistof3;
  vloeistof_oud4=vloeistof4;

}

void instel_percentage(int gewenst_percentage_1, int gewenst_percentage_2, int gewenst_percentage_3, int gewenst_percentage_4, int percentage_v1, int percentage_v2, int percentage_v3, int percentage_v4)
{
  int steps_to_make1=0;
  int steps_to_make2=0;
  int steps_to_make3=0;
  int steps_to_make4=0;


  if(percentage_v1!=gewenst_percentage_1)
  {
    steps_to_make1++;
  }
    if(percentage_v2!=gewenst_percentage_2)
  {
    steps_to_make2++;
  }
    if(percentage_v3!=gewenst_percentage_3)
  {
    steps_to_make3++;
  }
    if(percentage_v3!=gewenst_percentage_3)
  {
    steps_to_make4++;
  }

   for(int i=0; i<steps_to_make1; i++)
   {
    step_down1();
   }


   for(int i=0; i<steps_to_make2; i++)
   {
    step_down2();
   }


   for(int i=0; i<steps_to_make3; i++)
   {
    step_down3();
   }

   for(int i=0; i<steps_to_make4; i++)
   {
    step_down4();
   }

}


void mode(int enable, int mode0, int mode1, int mode2)
{
  digitalWrite(enablePin, enable);
  digitalWrite(mode0Pin, mode0);
  digitalWrite(mode1Pin, mode1);
  digitalWrite(mode2Pin, mode2);
}


void setup() {

//Ludo rewrite notes: 3
//maak een apparte intiatie fucntie voor de pinnen van de stepper-controlers

  pinMode(enablePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(mode0Pin, OUTPUT);
  pinMode(mode1Pin, OUTPUT);
  pinMode(mode2Pin, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  pinMode(dirPin4, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(stepPin3, OUTPUT);
  pinMode(stepPin4, OUTPUT);



mode(0,1,1,1);
}

void loop()
{

//Ludo rewrite notes: 4
//waardes defineren buiten de <void loop>

int vloeistof1;
int vloeistof2;
int vloeistof3;
int vloeistof4;
int instel_percentage1;
int instel_percentage2;
int instel_percentage3;
int instel_percentage4;

percentage_mengen(vloeistof1,vloeistof2,vloeistof3,vloeistof4);
instel_percentage(instel_percentage1,instel_percentage2,instel_percentage3,instel_percentage4,vloeistof1,vloeistof2,vloeistof3,vloeistof4);

//Ludo rewrite notes: 5
//Er is geen enkele aanwezigheid van een Serial-Debuig
//Dit is vrij essentieel voor het testen/debuggen van de code

}
