// RcDispach.cpp
#include "MyRobot.h"

static int _PfKey = 0;  // see PfKeyGet()

//-----------------------------------------------------------------------------
// PfKeySet -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void PfKeySet(int InKey)
{
   CSerial.printf("PfKeySet %d\n", InKey);
   _PfKey = InKey;
}

//-----------------------------------------------------------------------------
// PfKeyGet -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int PfKeyGet()
{
   int r = _PfKey;
   _PfKey = 0;  // one shot
   return r;   // PfKey value, 0 = no key, -1 = stop (special code)
}

//-----------------------------------------------------------------------------
// RcDispatch - Verwerk RC5 code
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RcDispatch(int &RcData)
{
   if (!RcData) return; // geen nieuwe code ontvangen

   RcData &= 0xF7FF;   // mask toggle bit
   CSerial.printf("Rc5: 0x%04x\n", RcData);
   int NewCode = RcData;
   RcData = 0; // wis verwerkte code (RcData is referentie)

   switch(NewCode) {

      // Special keys
      case 0x3761 : { // menu -> reset position
         Position.Reset();
      }
      break;

      // PF-keys
      case 0x3775 : { PfKeySet(-1); break; } // STOP (special, but passed to ProgrammaTakt)

      case 0x3770 : { PfKeySet( 1); break; } // mark
      case 0x2745 : { PfKeySet( 2); break; } // audio
      case 0x2741 : { PfKeySet( 3); break; } // title menu
      case 0x2749 : { PfKeySet( 4); break; } // sub title
      case 0x2744 : { PfKeySet( 5); break; } // input
      case 0x2740 : { PfKeySet( 6); break; } // tv
      case 0x2746 : { PfKeySet( 7); break; } // hdd
      case 0x274a : { PfKeySet( 8); break; } // hdd
      case 0x3769 : { PfKeySet( 9); break; } // zoom
      case 0x274c : { PfKeySet(10); break; } // -
      case 0x3751 : { PfKeySet(11); break; } // +
      case 0x3750 : { PfKeySet(12); break; } // store
   }
}
