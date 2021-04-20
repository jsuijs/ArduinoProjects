//-----------------------------------------------------------------------------
// RcDispach.cpp
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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
      case RC_STOP   : { PfKeySet(-1); break; } // STOP (special, but passed to ProgrammaTakt)

      case RC_F01    : { PfKeySet( 1); break; } // mark
      case RC_F02    : { PfKeySet( 2); break; } // audio
      case RC_F03    : { PfKeySet( 3); break; } // title menu
      case RC_F04    : { PfKeySet( 4); break; } // sub title
      case RC_F05    : { PfKeySet( 5); break; } // input
      case RC_F06    : { PfKeySet( 6); break; } // tv
      case RC_F07    : { PfKeySet( 7); break; } // hdd
      case RC_F08    : { PfKeySet( 8); break; } // hdd
      case RC_F09    : { PfKeySet( 9); break; } // zoom
      case RC_F10    : { PfKeySet(10); break; } // -
      case RC_F11    : { PfKeySet(11); break; } // +
      case RC_F12    : { PfKeySet(12); break; } // store
   }
}
