//-----------------------------------------------------------------------------
// ProgrammTakt.ino
//-----------------------------------------------------------------------------

// prototypes (i.v.m. include uit ander project)
bool Rijden1Takt(bool Init);
bool UmbMark1(bool Init);
bool TTijdTakt(bool Init);
bool HeenEnWeerTakt(bool Init);
bool UmbMark1Takt(bool Init);

// Global vars
int ProgrammaParam1;

void ProgrammaTakt()
{
   static int State, PrevState = -1;
   bool NewState = false;

   // rapporteer status bij state overgang
   if (PrevState != State) {
      PrevState = State;
      printf("Programma: %d\n", State);
      NewState = true;
   }

   char ch = PfKeyGet();
   if (ch) {
      // knop ingedrukt

      //      printf("Key: %d\n", ch);
      if (ch == -1) {
         State = 0;           // reset, stop lopend programma / programma 'stilstaan'.
      } else {
         if (State == 0) {    // andere pfkeys werken alleen als we stil staan
            State = ch;
         }
      }
   }

   // Roep actieve programma 1 t/m 12  aan.
   switch(State) {
      case 0 : { // Programma: stil staan
         Driver.Power(0,0);
         break;
      }

      case 1 : { // Programma: rijden1
         if (Rijden1Takt(NewState)) {
            State = 0;
         }
         break;
      }

      case 2 : { // Programma: max_pwm
         Driver.Power(155, 155);
         break;
      }


      case 3 : { // Programma: speed
         Driver.SpeedLR(300, 300);
         break;
      }

      case 5 : { // Programma: UmbMark CCW           
         if (NewState) {
            ProgrammaParam1 = 1;      // set CCW
         }
         if (UmbMark1Takt(NewState)) {   // call StateMachine
            State = 0;               // Statemachine returns true => program done.
         }
         break;
      }

      case 6 : { // Programma: UmbMark CW 
         if (NewState) {
            ProgrammaParam1 = -1;   // set CW
         }
         if (UmbMark1Takt(NewState)) { // call StateMachine                             
            State = 0;                 // Statemachine returns true => program done.    
         }
         break;
      }

      case 9 : { // Programma: Heen en Weer                   
         if (NewState) {
         }         
         if (HeenEnWeerTakt(NewState)) {    // call StateMachine                             
            State = 0;                 // Statemachine returns true => program done.    
         }
         break;
      }

      case 10 : { // Programma: ttijd
         if (NewState) {
         }         
         if (TTijdTakt(NewState)) {    // call StateMachine                             
            State = 0;                 // Statemachine returns true => program done.    
         }
         break;
      }

      case 11 : { // Programma: 
         break;
      }

      case 12 : { // Programma: test1
         Driver.Rotate(90);
//         State = 0;
         break;
      }
      
      default : {
         printf("ProgrammaTakt: ongeldige state %d\n", State);
         State = 0;
         break;
      }
   } // einde van switch
}



//-----------------------------------------------------------------------------
// Rijden1Takt - pwm trapje, print pwm + toerental
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool Rijden1Takt(bool Init)
{    
   static int Step;  
   static bool Oplopend;
   static int Slow;

   if (Init) {
      Step = 0;
      Oplopend = true;
      Slow = 0;
   }

   if (Slow == 0) {
      if (Oplopend) {
         Step +=2;
      } else {
         Step -=2;
      }
      Slow = 10;
   } else {   
      Slow --;
   }

    if (Step >  255) Oplopend = false;
    if (Step < -255) Oplopend = true;

    Driver.Power(Step, Step);
    printf("Pwm: %d, Speed: %d / %d\n", Step, Position.ActSpeedL, Position.ActSpeedR);
    Position.Print();      
    
    if (Oplopend && (Step == 0)) {
      return true; // Klaar
   }
    
   return false;  // Nog niet klaar.
}



#define UMB_MARK_AFSTAND 800    // mm
#define UMB_MARK_SPEED 300    // mm/sec

//-----------------------------------------------------------------------------
// MSM_UmbMark1 - 
//-----------------------------------------------------------------------------
// Bij UmbMark rijdt de robot een vierkant met de klok mee en een vierkant
// tegen de klok in. Bij aankomst wordt de verschuiving in X-richting opgemeten
// en op basis van deze twee getallen (delta-X clockwise & counterclockwise)
// kan worden bepaald wat de afwijking van de wielbasis-calibratie en de 
// afwijking door wielgrootte is.              
//
// var ProgrammaParam1 = 1 (CCW) of -1 (CW)
//                           
//-----------------------------------------------------------------------------
bool UmbMark1Takt(bool Init)
{  static int State, PrevState;
   bool NewState = false;

   if (Init) {
      State  = 0;
      PrevState = -1;
   }

   if (PrevState != State) {
      PrevState = State;
      NewState = true;
      printf("UmbMark1 state %d\n", State);
   }
      
   switch (State) { 
      case 0 :    // Rij naar X, 0    
            if (NewState) {
               Driver.XY(UMB_MARK_AFSTAND, 0, UMB_MARK_SPEED, 0);  // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }                                                          

            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state       
            }
         break;               

      case 1 :    // Draai naar +90 of -90, afhankelijk van MissionHandler.ParamGet(0)
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90 * ProgrammaParam1); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;       
                 
      case 2 :    // Rij naar X, Y of X, -Y, afhankelijk van MissionHandler.ParamGet(0)
            if (NewState) {
               // voor het eerst in deze state
               Driver.XY( UMB_MARK_AFSTAND, 
                        UMB_MARK_AFSTAND * ProgrammaParam1, 
                        UMB_MARK_SPEED, 
                        0); // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;               

      case 3 :    // Draai naar 180 graden
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;       
                 
      case 4 :    // Rij naar 0, Y of 0, -Y, afhankelijk van MissionHandler.ParamGet(0)
            if (NewState) {        
               // voor het eerst in deze state
               Driver.XY( 0, 
                        UMB_MARK_AFSTAND * ProgrammaParam1, 
                        UMB_MARK_SPEED, 
                        0); // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;               

      case 5 :    // Draai naar -90 of +90, afhankelijk van MissionHandler.ParamGet(0)
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(-90 * ProgrammaParam1); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;       
                 
      case 6 :    // Rij naar 0, 0
            if (NewState) {
               // voor het eerst in deze state
               Driver.XY(0, 0, UMB_MARK_SPEED, 0); // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;               

      case 7 :    // Draai naar 0 graden
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(0); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               return true;   // Geef aan dat de missie klaar is.
                              // De Msm zal nu niet meer aangeroepen worden.
            }
         break;       
              
      default :   // error
            return true;   // mission end
         break;
   }           
   return false;  // mission nog niet gereed
} 

#define HW_SNELHEID 500
// HeenEnWeer state machine
bool HeenEnWeerTakt(bool Init)
{  static int State, PrevState;
   bool NewState = false;
   int x;
   
   if (Init) {
      State  = 0;
      PrevState = -1;
   }

   if (PrevState != State) {
      PrevState = State;
      NewState = true;
      printf("TTijd state %d\n", State);
   }

   printf("hw %d %d %d\n", State, Driver.SollSpeedL, Driver.SollSpeedR);
            
   switch (State) { 
      case 0 :    // Volg wand naar vak B 
            x = (200 - SharpRechts)/10;  // wand volgen            
            Driver.SpeedHeading(HW_SNELHEID, x);  // Speed, Heading

            if (SharpVoor < 400) { // Als we de wand voor ons zien
               State++; // naar volgende state       
            }
         break;               

      case 1 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop(); 
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;               

      case 2 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;    
         
      case 3 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;               

      case 4 :    // Terug naar startpunt
            if (NewState) {
               // voor het eerst in deze state
               Driver.XY(0, 0, 600, 0); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               return true; // done
            }         
         break;               
                                                
      default :   // error
            return true;   // mission end
         break;
   }           
   return false;  // mission nog niet gereed
} 


#define TT_SNELHEID 300
// TTijd state machine
bool TTijdTakt(bool Init)
{  static int State, PrevState;
   bool NewState = false;
   int x;
   
   if (Init) {
      State  = 0;
      PrevState = -1;
   }

   if (PrevState != State) {
      PrevState = State;
      NewState = true;
      printf("TTijd state %d\n", State);
   }
      
   switch (State) { 
      case 0 :    // Volg wand naar vak B 
            x = (200 - SharpRechts)/10;  // wand volgen            
            Driver.SpeedHeading(TT_SNELHEID, x);  // Speed, Heading

            if (SharpVoor < 300) { // Als we de wand voor ons zien
               State++; // naar volgende state       
            }
         break;               

      case 1 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop(); 
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;               

      case 2 :    // Draai            
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;               

      case 3 :    // Volg wand in vak B
            x = (200 - SharpRechts)/10;  // wand volgen            
            Driver.SpeedHeading(TT_SNELHEID, x + 90);  // Speed, Heading
            printf("SharpRechts: %d\n", SharpRechts);

            if (SharpVoor < 300) { // Als we de wand voor ons zien
               State++; // naar volgende state       
            }
         break;               

      case 4 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop(); 
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;       
         
      case 5 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;      
           
      case 6 :    // Volg wand uit vak B
            x = (200 - SharpRechts)/10;  // wand volgen            
            Driver.SpeedHeading(TT_SNELHEID, x + 180);  // Speed, Heading

            if (SharpRechts > 400) { // Als we rechts geen wand meer zien
               State++; // naar volgende state       
            }
         break;               


      case 7 :    // Nog klein stukje rechtdoor   
            if (NewState) {
               // voor het eerst in deze state
               Driver.XY(Position.XPos-280, Position.YPos, TT_SNELHEID, 0); 
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }         
         break;               
         
      case 8 :    // draai richting C
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;               
         
      case 9 :    // Volg wand naar vak C       
            x = (200 - SharpRechts)/10;  // wand volgen            
            x = Clip(x, -5, 5);
            Driver.SpeedHeading(TT_SNELHEID, x + 90);  // Speed, Heading
            printf("Sharp: %d, x: %d\n", SharpRechts, x);

            if (SharpVoor < 300) { // Als we de wand voor ons zien
               State++; // naar volgende state       
            }         
         break;               

      case 10 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop(); 
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;       
                  
      case 11 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;               
         
      case 12 :    // Volg wand in vak C               
            x = (200 - SharpRechts)/10;  // wand volgen            
            Driver.SpeedHeading(TT_SNELHEID, x + 180);  // Speed, Heading

            if (SharpVoor < 300) { // Als we de wand voor ons zien
               State++; // naar volgende state       
            }         
         break;               

      case 13 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop(); 
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;       
         
      case 14 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(270); // Heading (in graden)
            }

            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;               
         
      case 15 :    // Volg wand uit vak C      
            x = (200 - SharpRechts)/10;  // wand volgen            
            Driver.SpeedHeading(TT_SNELHEID, x + 270);  // Speed, Heading

            if (SharpRechts > 400) { // Als we rechts geen wand meer zien
               State++; // naar volgende state       
            }
         break;               
         
      case 16 :    // Nog een klein stukje rechtdoor
            if (NewState) {
               // voor het eerst in deze state
               Driver.XY(Position.XPos, Position.YPos-280, TT_SNELHEID, 0); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }         
         break;               
         
      case 17 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }

            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;             

      case 18 :    // Volg wand naar vak A  
            x = (200 - SharpRechts)/10;  // wand volgen            
            Driver.SpeedHeading(TT_SNELHEID, x + 180);  // Speed, Heading

            if (SharpVoor < 300) { // Als we de wand voor ons zien
               return true; // done
            }         
         break;               

      case 19 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop(); 
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;       
                                                
      default :   // error
            return true;   // mission end
         break;
   }           
   return false;  // mission nog niet gereed
} 
