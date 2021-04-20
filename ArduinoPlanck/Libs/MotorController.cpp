//-----------------------------------------------------------------------------
// MotorController.cpp
//-----------------------------------------------------------------------------
// Deze file bevat de snelheidsregeling van de robot.
//-----------------------------------------------------------------------------

// MotorController (Mc) data structure met PID var's
struct tMc {
      int PrevSpeedL, PrevSpeedR; // Vorige snelheid (in mm/sec)
      int IErrorL, IErrorR;
   } Mc;

//-----------------------------------------------------------------------------
// SpeedController - regel motorsnelheid
//-----------------------------------------------------------------------------
// Parameters: SpeedSoll* (in mm/sec)
//-----------------------------------------------------------------------------
void MotorController(int SetpointL, int SetpointR)
{
   int PError = 999;
   int DError = 999;
   int PowerL, PowerR;

   // reken setpoints om van mm/s naar ticks/interval
   volatile int SetpointL_Ticks = (SetpointL * 4L * MAIN_TAKT_INTERVAL) / ODO_TICK_TO_METRIC;
   volatile int SetpointR_Ticks = (SetpointR * 4L * MAIN_TAKT_INTERVAL) / ODO_TICK_TO_METRIC;

  //CSerial.printf("!! %d %d %d %d\n", SetpointL, SetpointL_Ticks, MAIN_TAKT_INTERVAL, ODO_TICK_TO_METRIC);

   if (SetpointL_Ticks != 0) {
      //  PID motor speed control:
      PError = SetpointL_Ticks - Position.ActSpeedL;     // P fout, in odo-pulsen per takt-inteval
      DError = Position.ActSpeedL - Mc.PrevSpeedL;       // D fout
      Mc.PrevSpeedL = Position.ActSpeedL;
      Mc.IErrorL += PError;                              // I fout

      if (Mc.IErrorL >  MPID_I_MAX) {
         Mc.IErrorL =  MPID_I_MAX;   //anti windup
      }
      if (Mc.IErrorL < -MPID_I_MAX) {
         Mc.IErrorL = -MPID_I_MAX;
      }

      //PWM aansturing
      PowerL = MPID_OFFSET       +                             // PID regeling !!
               SetpointL_Ticks   * MPID_SP_GAIN +
               PError            * MPID_P_GAIN  +
               Mc.IErrorL        * MPID_I_GAIN  +
               DError            * MPID_D_GAIN  ;
   } else {
      Mc.IErrorL = 0;
      PowerL     = 0;
   }

   //CSerial.printf("PID_L Perr %d, Ierr %d, Derr %d, Soll %d, Act: %d\n", PError, Mc.IErrorL, DError, SetpointL_Ticks, Position.ActSpeedL);

   if (SetpointR_Ticks != 0) {
      //  PID motor speed control:
      PError = SetpointR_Ticks - Position.ActSpeedR;     // P fout, in odo-pulsen per takt-inteval
      DError = Position.ActSpeedR - Mc.PrevSpeedR;       // D fout
      Mc.PrevSpeedR = Position.ActSpeedR;
      Mc.IErrorR += PError;                              // I fout

      if (Mc.IErrorR >  MPID_I_MAX) {
         Mc.IErrorR =  MPID_I_MAX;   //anti windup
      }
      if (Mc.IErrorR < -MPID_I_MAX) {
         Mc.IErrorR = -MPID_I_MAX;
      }

      //PWM aansturing
      PowerR = MPID_OFFSET       +                             //PID regeling !!
               SetpointR_Ticks   * MPID_SP_GAIN +
               PError            * MPID_P_GAIN  +
               Mc.IErrorR        * MPID_I_GAIN  +
               DError            * MPID_D_GAIN  ;
   } else {
      Mc.IErrorR = 0;
      PowerR     = 0;
   }
   Motors(PowerL, PowerR);
//   CSerial.printf("PID Soll: %d / %d, Ist: %d / %d, I: %d / %d, Power: %d / %d\n",
//         SetpointL_Ticks, SetpointR_Ticks, Position.ActSpeedL, Position.ActSpeedR, Mc.IErrorL, Mc.IErrorR, PowerL, PowerR);
}

