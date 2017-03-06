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
//          3:
//
//NEW_CODE: 1:  writing a new communication protocol between the printhead controller and the 3D-printer main borad <receive/transmit-function>
//          2:  writing a <info-channel-fucntion> that displays the information of the system on UART or a OLED-Display
//          3:  still arranging the processing functions
//          4:  create all the needed pointer arrays

//declaring variables Pin's
int modePin[3] = {5, 6, 7};       //[Mode-Pin's]     [motors 1,2,3,4] [control the precision of the stepper drivers]
int dirPin[4] = {2, 4, 6, 8};     //[Direcion-Pin's] [motors 1,2,3,4] [control the direction of each motor]
int stepPin[4] = {3, 5, 7, 9};       //[Step-Pin's]     [motors 1,2,3,4] [control the amount of steps]
int enablePin[4] = {4, 4, 4, 4};  //[Enable-Pin's]   [motors 1,2,3,4] [control on/off state of the stepper-driver]
int resetPin = 10;                   //[resetPin]       [resets all the stepper drivers]
int allP

//system variables
uint sysvar[6];         // {Write-OK, A%, B%, C%, D%, RPM}  <<Array to
uint *p = sysvar;       // pointer to stepp array
uint execution[6];      // {Write-OK, A-step, B-step, C-step, D-step, RPM}  <<Arrar to set the motor pins on each output-interrupt
int StepSize = 1;       // stepsize
uint SYS_STAT;          // status of the system [0:not-running 1:running 2-5: aborted fot refil of extruder liquid, else?]

//initiatin system
void setup()
{
  //TIMER_2 setten voor system output naar de motor controllers
  cli();
     //sets register to 0
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    // match register preset
    OCR2A = 300;  //16MHZ/f*8=compare value
    //CTC-mode
    TCCR2A |= (1 << WGM21);
    //set CS21 bit for 8 prescaler
    TCCR2B |= (1 << CS21);
    //enable timer compare interrupt
    TIMSK2 |= (1 << OCIE2A);
  sei():

  init_display();
  init_steppers();
}

//interrupt for handling the stepper drivers
//  -controls the IO ports
//  -manages the registers for extrusion 0-100% x4
//  -uses the system safety value, know if extractions is allowed

ISR(TIMER_2_COMPA_vect)
{
  while(SYS_STAT)
  {
    //loop loads values of of execution register to system IO's
    for(a=0;a<sizeof(execution),a++)
      output_pins[a] = execution[a];
    i++
  }
}

//this function will be used to display system information and errors
ISR(TIMER_1_COMPA_vect)
{

}

//main loop system
void loop()
{
  //write new function to check all buttons and stops of system
  input(); //write new function with protocol input
  //write function that looks for data-input-ok
  //aleter function of stepper control, <add register
}

input()
{

}

//Function to control the stepper motors
//Rewrite notes:
//    -make it so that he 4 motors can output at once   <<this will require a splitted function where on the first half runs in the main loop and the other half in the interrupt loop
//    -make a step error detection? or juist reduce the accuarecy of the steps
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
      case 1:   digitalWrite(modePin[0], LOW); digitalWrite(modePin[1], LOW); digitalWrite(modePin[2], LOW);    // full step
      case 2:   digitalWrite(modePin[0], HIGH); digitalWrite(modePin[1], LOW); digitalWrite(modePin[2], LOW);   // 1/2  step
      case 4:   digitalWrite(modePin[0], LOW); digitalWrite(modePin[1], HIGH); digitalWrite(modePin[2], LOW);   // 1/4  step
      case 8:   digitalWrite(modePin[0], HIGH); digitalWrite(modePin[1], HIGH); digitalWrite(modePin[2], LOW);  // 1/8  step
      case 16:  digitalWrite(modePin[0], LOW); digitalWrite(modePin[1], LOW); digitalWrite(modePin[2], HIGH);   // 1/16 step
      case 32:  digitalWrite(modePin[0], HIGH); digitalWrite(modePin[1], LOW); digitalWrite(modePin[2], HIGH);  // 1/32 step
      default:  digitalWrite(modePin[0], LOW); digitalWrite(modePin[1], LOW); digitalWrite(modePin[2], LOW);    // full step
                Serial.println("Error: wrong step size"); break;
    }

    //preset the motor direction
    if(direction)
      digitalRead(motor, HIGH);
    else
      digitalRead(motor, HIGH);

    //put everything in the output array
    //the
    // for(int x = steps; x < 0; x--)
    // {
    //   digitalWrite(StepPin, HIGH);
    //   delayMicroseconds(delay);
    //   digitalWrite(StepPin, LOW);
    //   delayMicroseconds(delay);
    }
}


  //assign all the variables to the system buffers, peforms a shift and add on all the buffers

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

void init_steppers(int pinsOut[], int pinsIn[], int pinsUart[])
{
  int a = sizeof(pinsOut), b = sizeof(pinsIn), c = sizeof(pinsUart);
    for(i=0; i<(a+b+c); i++)
    {
      if(i>a){
        pinMode(pins[i], OUTPUT);
      }else if(i>(a+b)){
        pinMode(pins[i], INPUT);
      }
    }
}

int modePin[3] = {5, 6, 7};       //[Mode-Pin's]     [motors 1,2,3,4] [control the precision of the stepper drivers]
int dirPin[4] = {2, 4, 6, 8};     //[Direcion-Pin's] [motors 1,2,3,4] [control the direction of each motor]
int step[4] = {3, 5, 7, 9};       //[Step-Pin's]     [motors 1,2,3,4] [control the amount of steps]
int enablePin[4] = {4, 4, 4, 4};  //[Enable-Pin's]   [motors 1,2,3,4] [control on/off state of the stepper-driver]
int reset = 10;                   //[resetPin]       [resets all the stepper drivers]

void init_display
