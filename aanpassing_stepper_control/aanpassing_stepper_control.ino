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
//  Code flow chart:  A. Get extrusion data from the UART <Continues> and calculate the extrusion and blend information for the printerhead
//                    B. Load data for extruders and blender into execute buffers
//                    C. Execute all buffers simultaneously with an interrupt function <interrupt><?Hz>
//                            <make sure this interrupt doesn't
//
//  ____  ____  ____  ____
//   ][    ][    ][    ][
//   ][    ][    ][    ][
//   ][    ][    ][    ][
//  _][_  _][_  _][_  _][_
//  |  |  |  |  |  |  |  |
//  |--|  |--|  |--|  |--|
//  |  |  |  |  |  |  |  |    <4x syringes with lurelock connection
//  |  |  |  |  |  |  |  |    <6cm filled with 3 ml of gel
//  |  |  |  |  |  |  |  |
//  |  |  |  |  |  |  |  |
//   \/    \/    \/    \/
//   [________  ________]
//           |  |
//         __|  |__
//       //        \\
//      ||   <-->   ||  <inside mix chamber mix-blade 0-320 RPM
//      ||  <---->  ||  <mix chamber
//      \\   <-->   //
//       \\__    __//
//           |  |       <extruder
//
//CODE REDESIGN <v3> <1/20/16>
//
//OLD:      The goal of the old code was to execute all the calculations required to extrude the gels depending on a serial data stream.
//
//PROBLEM:  1:  the serial data-input-stream uses an interrupt, but the output also uses this interrupt to control all the motors simultaneously
//          2:  the code performs unnecessary actions that should be done by the 3D printer main board instead, like timming steps and calculating volume/mm for the printing processe
//
//          3:
//NEW_CODE: 1


//declaring variables Pin's
int enablePin1 = 2; // pin to enable motor1
int enablePin2 = 3; // pin to enable motor2
int enablePin3 = 4; // pin to enable motor3
int enablePin4 = 5; // pin to enable motoro4

//int resetPin = 10; // connected to +5v not used in this system, can be connected if it is necessary in later use

int mode0Pin; // connected to ground, pin assignment in later progress
int mode1Pin; // connected to ground, pin assignment in later progress
int mode2Pin; // connected to ground, pin assignment in later progress

int dirPin = 6;  // direction pin for motor 1
//int dirPin2 = 4;  // direction pin for motor 2 // not nescessary to give each driver individual direction pulses, can be controlled trough the enable pulses
//int dirPin3 = 6;  // direction pin for motor 3 // not nescessary to give each driver individual direction pulses, can be controlled trough the enable pulses
//int dirPin4 = 8;  // direction pin for motor 4 // not nescessary to give each driver individual direction pulses, can be controlled trough the enable pulses

int stepPin = 7;  //stepperpin for motor 1
//int stepPin2 = 5; //stepperpin for motor 1 // not nescessary to give each driver individual step impulses
//int stepPin3 = 7; //stepperpin for motor 1 // not nescessary to give each driver individual step impulses
//int stepPin4 = 9; //stepperpin for motor 1 // not nescessary to give each driver individual step impulses

int stepsMax; // maximux steps till end of the spindel, has to be tested on monday

//system variables
uint sysvar[6];         //in system variable containing {A%, B%, C%, D%, RPM, dT}
uint *p = sysvar;       //pointer to stepp array
uint output[5];         //
int StepSize = 1;       //stepsize


//initiatin system
void setup()
{
  init_steppers();
}

//main loop system
void loop()
{
  input_stream();
  calc();

}


//Function to control the stepper motors
//Rewrite notes:
//    -make it so that he 4 motors can output at once   <<this will require a splitted function where on the first half runs in the main loop and the other half in the interrupt loop
//    -make a step error detection? or juist reduce the accuarecy of the steps
  // int A for motor 1 int B for motor2 int C for motor3 int D for motor4
void stepper(int A,int B,int C, int D, int Direction, int StepSize) //void stepper(motor: 1-4, Direction: 0-1, Steps: infinite, StepSize: 2,4,8,16,32)
{
    //local variables
//  int DirPin; // is al als global var gedefinieerd niet meer nodig
//    int StepPin; // is alredy a global variable
    int mostSteps;
    int Wait_time = 300; //microseconds delay between
                     //to perform steps the delay has to be altered: 300ms = 1 step; 600ms = 1/2 step; 900ms = 1/4 step; 1200ms = 1/8; 1500ms = 1/16 step

    //stepsize set        // not in use
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

//    //define pints to be used
//    switch(motor)
//    {
//        case 1: DirPin = 2; StepPin = 3; break;
//        case 2: DirPin = 4; StepPin = 5; break;
//        case 3: DirPin = 6; StepPin = 7; break;
//        case 4: DirPin = 8; StepPin = 9; break;
//        default: Serial.println("Error: motor does not exist"); break;
//    }

    //preset the motor direction
    if(Direction)
      digitalWrite(dirpin, HIGH);
    else
      digitalWrite(dirPin, LOW);

      if(A>B||A>C||A>D)
      {
        mostSteps=A;
      }
      else
      {
        mostSteps=mostSteps;
      }
      if(B>A||B>C||B>D)
      {
        mostSteps=B;
      }
      else
      {
        mostSteps=mostSteps;
      }
       if(C>A||C>B||C>D)
      {
        mostSteps=C;
      }
      else
      {
        mostSteps=mostSteps;
      }
      if(D>A||D>C||D>C)
      {
        mostSteps=D;
      }
      else
      {
        mostSteps=mostSteps;
      }

    for(int x = mostSteps; x < 0; x--)
    {
      if (A<0)
      {
      digitalWrite(enablePin1, LOW); // puts motor1 in use
      }
      else
      {
      digitalWrite(enablePin1, HIGH); // disables motor1
      }

      if (B<0)
      {
      digitalWrite(enablePin1, LOW); // puts motor2 in use
      }
      else
      {
      digitalWrite(enablePin1, HIGH); // disables motor2
      }
      if (C<0)
      {
      digitalWrite(enablePin1, LOW); // puts motor3 in use
      }
      else
      {
      digitalWrite(enablePin1, HIGH); // disables motor3
      }
      if (D<0)
      {
      digitalWrite(enablePin1, LOW); // puts motor4 in use
      }
      else
      {
      digitalWrite(enablePin1, HIGH); // disables motor4
      }
      digitalWrite(stepPin, HIGH); // makes one step pulse to the DRV8825
      delayMicroseconds(Wait_time);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(Wait_time);

      A=A-1; B=B-1; C=C-1; D=D-1; // counter of steps to make 
    }

}

//this function will receive the data over the serial port and load this data into an buffers
//after this, the function will sort the data in the input buffer and distribute this data over the in-system-buffers
void input_stream()
{
  if(Serial.avaliable())
  {
    SerialIn = Serial.Read();
  }
  SerialInArray = toCharArray(SerialIn);

}


//this function will perform all the calculations neded between the Serial-input and the final motor-control-actions
void calc(char array_in_raw[80], char array_steppers[4][80] ,char array_system_settings[80])
{
  uint array_in[5]; //the separated values of the AURT-input, internal variable

  int a;    //counter 1 //input array position
  int b;    //counter 2 //char-lenght of integer values
  int c;    //counter 3 //counter for construction integers <<<eliminated <<<replaced by B!!
  int d;    //counter 4 //counter of processed sigits of the imput array, used to store each vallue at the desired location

  //split all the data
  for(a=0; a<sizeof(array_in); a++)
  {
    while(array_in_raw[a]=!0)
    {
      b++;
    }else{
      for(b; b>0; c--)
      {
        array_in[d] = array_in_raw[a-b]*pow(10, b-1); <still working on the corret input data splitting><tonight>
      }
      d++; //counter to indicate vallues of inpunt, will increase after each blank space between 2 values.
    }
  }

  //assign all the variables to the system buffers, peforms a shift and add on all the buffers

}

//BufferShift
//Used in: Calc
//function for shifting an filling the buffers
//note1: need to use pointers as c++ can't return arrays

void BufferShift(uint input_vall, uint *fill_arr,, uint array_size)
{
  for(uint i=array_size; i>0; i--)
  {
    *()=*();
  }
}

//Outpust side, this function controls al the motors simultaneously
void output()
{

}

//this function manages the fluid composition that will exit the printhead on time interval <time - delay>
//optimizing by using 1-byte sized intergers
void blend(int8_t v1, int8_t v2, int8_t v3, int8_t v4, int8_t flow, int8_t speed)
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


///need to combine these 2 functions
///need to clean the code, and make them compatible with
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
///

void init_steppers(){
  mode(0,1,1,1);
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
}
