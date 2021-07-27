//-----------------------------------------------------------------------------
// Flags.h  - Flags class for debug
//-----------------------------------------------------------------------------
// Note: #define MAIN enables acutual code, in adition to prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class TFlags
{
   public:
      TFlags(int NrFlags); // NrFlags 32 means 0..31

      bool IsSet(int Nr);
      void Set(int Nr, bool Value);

      void Dump();

   private:
      int *FlagWords;
      int NrFlagWords;  // WordIx + 1
      int WordIx, BitIx;

      bool SetIx(int Nr);
};
extern TFlags Flags;

#ifdef MAIN

//-----------------------------------------------------------------------------
// TFlags::TFlags - constructor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TFlags::TFlags(int NrFlags)
   {
      SetIx(NrFlags-1); // 32 means 0..31
      NrFlagWords = WordIx + 1;
      FlagWords   = (int *)malloc(NrFlagWords * sizeof(int));
      for (int i=0; i<NrFlagWords; i++) FlagWords[i] = 0;;
   }

//-----------------------------------------------------------------------------
// TFlags::IsSet - Return true is flag is set.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TFlags::IsSet(int Nr)
   {
      if (!SetIx(Nr)) return false;
      return ((FlagWords[WordIx] & (1<<BitIx)) != 0);
   }

//-----------------------------------------------------------------------------
// TFlags::Set - set flag true or false
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TFlags::Set(int Nr, bool Value)
   {
      if (!SetIx(Nr)) {
         CSerial.printf("ERROR setting flag %d\n", Nr);
         return;
      }
      if (Value) {
         FlagWords[WordIx] |= (1<<BitIx);
      } else {
         FlagWords[WordIx] &= 0xFFFFFFFF ^ (1<<BitIx);
      }
      CSerial.printf("Flag %d set to %d (%d %d %d)\n", Nr, Value, WordIx, BitIx, NrFlagWords);
   }

//-----------------------------------------------------------------------------
// TFlags::Dump - All the flags
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TFlags::Dump()
   {
      CSerial.printf("NrFlagWords: %d\n", NrFlagWords);
      for (int i=0; i<NrFlagWords; i++) CSerial.printf("%08x ", FlagWords[i]);
      CSerial.printf("\n");
   }

//-----------------------------------------------------------------------------
// TFlags::SetIx - private function
//-----------------------------------------------------------------------------
// return true if Nr is valid
//-----------------------------------------------------------------------------
bool TFlags::SetIx(int Nr)
   {
      WordIx = Nr / 32;
      BitIx  = Nr - 32 * WordIx;
      if (WordIx >= NrFlagWords) return false;  // out of range
      if (!FlagWords)            return false;  // no vars malloc'd
      return true;
   }

#endif // MAIN
