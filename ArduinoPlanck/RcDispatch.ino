
int PfKey = 0;
int PfKeyGet()
{
   int r = PfKey;
   PfKey = 0;  // one shot
   return r;   // PfKey value, 0 = no key, -1 = stop (special code)
}



void RcDispatch()
{  int NewCode;

   if (!Rc5Data) return;

   Rc5Data &= 0xF7FF;   // mask toggle bit
   printf("Rc5: 0x%04x\n", Rc5Data);
   NewCode = Rc5Data;
   Rc5Data = 0;

   switch(NewCode) {

      // Numbers:
      case 0x3740 : case 0x3741 : case 0x3742 : case 0x3743 : case 0x3744 :
      case 0x3745 : case 0x3746 : case 0x3747 : case 0x3748 :
      case 0x3749 : { // numbers 0..9
         //NextPrg = NewCode - 0x3740;
         break;
      }

      // Special keys
      case 0x3761 : { // menu -> reset position
         Position.Reset();
         break;
      }
      case 0x3776 : { // list -> toggle second logging on/off
         SecondLoggingOn = !SecondLoggingOn;
         break;
      }


      // PF-keys
      case 0x3775 : { PfKey = -1;   break; } // STOP (special, but passed to ProgrammaTakt)

      case 0x3770 : { PfKey =  1;   break; } // mark
      case 0x2745 : { PfKey =  2;   break; } // audio
      case 0x2741 : { PfKey =  3;   break; } // title menu
      case 0x2749 : { PfKey =  4;   break; } // sub title
      case 0x2744 : { PfKey =  5;   break; } // input
      case 0x2740 : { PfKey =  6;   break; } // tv
      case 0x2746 : { PfKey =  7;   break; } // hdd
      case 0x274a : { PfKey =  8;   break; } // hdd
      case 0x3769 : { PfKey =  9;   break; } // zoom
      case 0x274c : { PfKey = 10;   break; } // -
      case 0x3751 : { PfKey = 11;   break; } // +
      case 0x3750 : { PfKey = 12;   break; } // store

   }

}