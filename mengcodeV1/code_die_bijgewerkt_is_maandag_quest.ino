//Author: Ludo Teirlinck and Sebastian Smit
//Code: This code will manage a gell extrusion system for a medical 3D printer
//      The system uses the following values:
//      Input:  UART  < for testing purposes the desired values can be introduced to the priter over UART
//              ??    < Main input from the REP RAP DUET module <no communication protocol yet, because it needs to be very fast>
//                      *<the REPRAP DUET seems to have  2-serial busses, not sure yet if we can use this bus>
//      Output: UART  < the UART can also be used for live data output of the printerhead it steps1-vloeistof_oud1

//NEW CODE FLOW DESIGN <1/16/17>
//  Independet components in the code:  -UART-IN reader                     <<Reads UART line, and save values to different arrays
//                                      -Calculation for the 4 extruders    <<Takes the values for the extrudes for a certain DeltaTime and calculates the extrusion
//                                      -Control function for 4 extruders
//                                      -Control function fot blender in mix-chamber
//                                      -Debug out on the UART <Developer-tool>
//                                      -Main output to all motors with Interupt?
//
//
//
//  ____  ____  ____  ____
//   ][    ][    ][    ][
//   ][    ][    ][    ][
//   ][    ][    ][    ][
//  _][_  _][_  _][_  _][_
//  |  |  |  |  |  |  |  |
//  |--|  |--|  |--|  |--|
//  |  |  |  |  |  |  |  |    <4x extruders with lurelock connection
//  |  |  |  |  |  |  |  |    <6cm filled with 3 ml of gel
//  |  |  |  |  |  |  |  |
//  |  |  |  |  |  |  |  |
//   \/    \/    \/    \/
//   <________  ________>
//           |  |
//         __|  |__
//       //        \\
//      ||   <-->   ||  <inside mix chamber mix-blade 0-320 RPM
//      ||  <---->  ||  <mix chamber
//      \\   <-->   //
//       \\__    __//
//           |  |       <extruder
//



//declaring variables
int enablePin = 4;
int resetPin = 10;

int meng_motorPin = 11;

int okPin1 = 12;
int okPin2 = 13;
int okPin3 = 14;
int okPin4 = 15;

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

int Step [4][2];    //2d array for steps taken by the motors
int *pstep = Step;  //pointer to stepp array

int volume [5][3];      //volume of gel compartiments and blend chamber
int *pvolume = volume;  //pointer to vollume array

int StepSize = 32;            //stepsize
int *pstepzize = StepSize;    //

//initiatin system
void setup()
{
  init_steppers();
  mode(0,1,1,1);
}

//main loop system
void loop()
{

//Ludo rewrite notes: 4
//waardes defineren buiten de <void loop>

// int vloeistof1;
// int vloeistof2;
// int vloeistof3;
// int vloeistof4;
// int instel_percentage1;
// int instel_percentage2;
// int instel_percentage3;
// int instel_percentage4;
//
// percentage_mengen(vloeistof1,vloeistof2,vloeistof3,vloeistof4);
// instel_percentage(instel_percentage1,instel_percentage2,instel_percentage3,instel_percentage4,vloeistof1,vloeistof2,vloeistof3,vloeistof4);

//Ludo rewrite notes: 5
//Er is geen enkele aanwezigheid van een Serial-Debug
//Dit is vrij essentieel voor het testen/debuggen van de code

}

int okSignal() // function that detects if the pusher is making contact whith the syringe
{
  int oksignal[4];
  
  for (i=0; i<3; i++)
  {
  if (digitalRead(okPin[i]) == HIGH)
    {
      okSignal[i]=1;
    }
  if (digitalRead(resetPin == HIGH)
    {
      okSignal[i]=0;
    }
    return okSignal[i];
  } 
}


//new function stepper-controlers
void stepper(int Motor, int Direction, int Steps, int StepSize) //void stepper(motor: 1-4, Direction: 0-1, Steps: infinite, StepSize: 2,4,8,16,32)
{
    //local variables
    int DirPin;
    int StepPin;
    int delay = 500; //microseconds delay between
                     //to perform steps the delay has to be altered: 300ms = 1 step; 600ms = 1/2 step; 900ms = 1/4 step; 1200ms = 1/8; 1500ms = 1/16 step

    //stepsize set
    switch(StepSize)
    {
      case 1:   digitalWrite(mode0Pin, LOW); digitalWrite(mode1Pin, LOW); digitalWrite(mode2Pin, LOW);    // full step
      case 2:   digitalWrite(mode0Pin, HIGH); digitalWrite(mode1Pin, LOW); digitalWrite(mode2Pin, LOW);   // 1/2  step
      case 4:   digitalWrite(mode0Pin, LOW); digitalWrite(mode1Pin, HIGH); digitalWrite(mode2Pin, LOW);   // 1/4  step
      case 8:   digitalWrite(mode0Pin, HIGH); digitalWrite(mode1Pin, HIGH); digitalWrite(mode2Pin, LOW);  // 1/8  step
      case 16:  digitalWrite(mode0Pin, LOW); digitalWrite(mode1Pin, LOW); digitalWrite(mode2Pin, HIGH);   // 1/16 step
      case 32:  digitalWrite(mode0Pin, HIGH); digitalWrite(mode1Pin, LOW); digitalWrite(mode2Pin, HIGH);  // 1/32 step
      default:  digitalWrite(mode0Pin, LOW); digitalWrite(mode1Pin, LOW); digitalWrite(mode2Pin, LOW);    // full step
                Serial.println("Error: wrong step size"); break;
    }

    //define pints to be used
    switch(motor)
    {
        case 1: DirPin = 2; StepPin = 3; break;
        case 2: DirPin = 4; StepPin = 5; break;
        case 3: DirPin = 6; StepPin = 7; break;
        case 4: DirPin = 8; StepPin = 9; break;
        default: Serial.println("Error: motor does not exist"); break;
    }

    //preset the motor direction
    if(direction)
      digitalRead(motor, HIGH);
    else
      digitalRead(motor, HIGH);

    //perform x out of 400 steps
    //<<make this better!!!
    for(int x = steps; x < 0; x--)
    {
      digitalWrite(StepPin, HIGH);
      delayMicroseconds(delay);
      digitalWrite(StepPin, LOW);
      delayMicroseconds(delay);
    }

}

//PWN control for the blender motor
blender(int rpm)
{
  int voltage;
  voltage=6/320*255; // bij 6v is het rpm 320, deze regel zet gewenste rpm om in voltage
  analogWrite(meng_motorPin,voltage);
}

//this function manages the fluid composition that will exit the printhead on time interval <time - delay>
//optimizing by using 1-byte sized intergers
void blend(int8_t v1, int8_t v2, int8_t v3, int8_t v4, int8_t flow, int8_t Speed)// speed gebruik je niet eens in de void, en het is een comanndo in arduino
{
  volume_setp_ml = 0,01;  //6ml/600steps=100steps/ml
  //control the blend motor
  blender();
  //controll all the stepper motors
  for(i=0; i<4; i++)
  {
    steps =
    stepper(i, 1, steps, *SetpSize);
  }
}


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

void init_steppers(){
  pinMode(enablePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(mode0Pin, OUTPUT);
  pinMode(mode1Pin, OUTPUT);
  pinMode(mode2Pin, OUTPUT);
  pinMode(meg_motorPin, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  pinMode(dirPin4, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(stepPin3, OUTPUT);
  pinMode(stepPin4, OUTPUT);
  pinMode(okPin1, INPUT);
  pinMode(okPin2, INPUT);
  pinMode(okPin3, INPUT);
  pinMode(okPin4, INPUT);
}

