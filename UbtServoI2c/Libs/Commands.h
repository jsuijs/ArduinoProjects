//-----------------------------------------------------------------------------
// Commands.h - (c) 2016-2021 Karel Dupain & Joep Suijs
//-----------------------------------------------------------------------------
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//-----------------------------------------------------------------------------
#ifndef COMMANDS_H
#define COMMANDS_H

#include "Arduino.h"

#define CMD_BUFFER_SIZE 64          // Command with parameters.
#define CMD_MAX_NR_PARAMETERS 9

void HexDump(const void *Data, int Length);
void HexDump( const void *Data, unsigned int Length, unsigned int Offset);

/*=====================================================================
 TCommand :
 ---------------------------------------------------------------------*/

class TCommand
{

public:

   TCommand(void (*CmdExecFunction)(int Param[]));
   char Takt(Stream &Ser);

   bool Match(const char *Keyword, byte NrParams);   // return true on success (command & parameter count match)

   void Print()
   {
      MyPrintf( "Cnt = %d  A=%u  B=%u  C=%u  Cmd=%s\r\n", ParamCount, Param[0], Param[1], Param[2], Cmd) ;
   }

   // functions below public, just in case you need to build your own takt...
   void Clear()  { State = 0 ; }  // Clear - Signal we want to restart
   int  GetLine(int Value);
   char Execute(int Verbose = 2);
   void Help(const char *Str);

private:
   bool AddParam( int Value );

   void (*Executor)(int Param[]);

   char Cmd[CMD_BUFFER_SIZE] ;

   int  Param[CMD_MAX_NR_PARAMETERS] ;
   int  NrIndex   ;
   int  ParamCount;

   int  State     ;
   int  Position  ;

   bool HexMode   ;
   bool Negative  ;

   int  LastError ; // for Execute &() MatchCommand() only

   bool HelpFlag  ;
};

#ifdef MAIN
//-----------------------------------------------------------------------------
// TCommand::TCommand - constructor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TCommand::TCommand(void (*CmdExecFunction)(int Number[]))
{
   Executor = CmdExecFunction;
   Clear();
}

//-----------------------------------------------------------------------------
// TCommand::GetLine - Collect & interpret a command line
//-----------------------------------------------------------------------------
// Input:  char value, -1 means no data (ignore).
// return:  0 collecting (no action),
//          1 ok -> ready for execute
//          < 0 error
//-----------------------------------------------------------------------------
int TCommand::GetLine(int Value)
   {
      if (Value < 0 ) return 0;
      if (Value == 0x0a) Value = 0x0d;

//      MyPrintf("GL Value: %d (%c), State: %d, NrIndex: %d, ParamCount: %d\n", Value, Value, State, NrIndex, ParamCount);
//      MyPrintf("GL Value: %d (%c), State: %d\n", Value, Value, State);

      switch( State )   {

         case 0: { // wait for first alpha-char

            if (isspace( Value ) ) return 0;

            if (Value == '`') {
               // repeat previous command
               State = 500;  // process whitespaces until line end
               return 0;
            } else {
               // New input -> clear internal data.
               Position   = 0;
               NrIndex    = 0;
               HexMode    = false;
               Negative   = false;
               ParamCount = 0;

               for (int i=0; i<CMD_MAX_NR_PARAMETERS; i++) {
                  Param[i] = 0;
               }
            }

            if ( Value > ' ' ) {
               AddParam(Value);
               State = 100;
               return 0;
            }
            State = 900;
            return -1;
         }

         case 100: { // wait for other command word chars

            if (isalnum( Value ) == false ) {
               if (isspace( Value )) {
                  if (Value == 0x0d ) break;
                  State = 200;
                  return 0;
               }
               State = 900;
               return -1;
            }

            AddParam(Value);
            return 0;
         }

         case 200: { // First character of parameter/number

            if (Value == 0x0d ) break;

            if (isspace( Value )) return 0;

            if (Value == '-' ) {
               Negative = true;
               Param[ NrIndex ] = 0;
               HexMode = false;
               State = 400;
               return 0;
            }

            if (isdigit( Value ) == false ) return -2;

            if (Value == '0') {
               State = 300; // wait for 'x' ('0x', hex pre-amble), number, whitespace or line-end
               return 0;
            }

            Param[ NrIndex ] = Value - 0x30;
            HexMode = false;
            State = 400;      // Collect number
            return 0;
         }

         case 300: { // wait for 'x' ('0x', hex pre-amble), number, whitespace or line-end

            ParamCount = NrIndex + 1;

            if (( Value | 0x20 ) == 'x' ) {
               HexMode = true;
               State = 400;
               return 0;
            }

            if (Value == 0x0d ) break;

            if (isspace(Value)) {

               NrIndex++;

               if (NrIndex >= CMD_MAX_NR_PARAMETERS) {
                  State = 500;  // allow only whitespaces or line-end
               } else {
                  State = 200;  // Get first char of next param
               }

               return 0;
            }

            if (isdigit(Value)) {
               // '0' followed by digit -> decimal input
               Param[ NrIndex ] = Value - 0x30;
               HexMode = false;
               State = 400;
               return 0;
            }

            State = 900;
            return -3;  // no 'x', number, whitespace or line-end -> bad sequence
         }

         case 400: { // collect number (hex or decimal)

            ParamCount = NrIndex + 1;

            if (Value == 0x0d ) break;

            if (isspace(Value)) {

               NrIndex++;

               if (NrIndex > CMD_MAX_NR_PARAMETERS) {
                  State = 500;  // last parameter (allowed)
               } else {
                  Negative = false;
                  State = 200;  // scan (optional) next parameter
               }

               return 0;
            }

            if (HexMode) {

               if (isxdigit( Value ) == false ) return -5;

               if (isalpha(Value)) {
                  Value = (Value & 0xdf) - 55;
               } else {
                  Value = (Value - 0x30);
               }

               Param[ NrIndex ] = Param[ NrIndex ] * 16 + Value;

            } else {
               if (isdigit( Value )  == false ) return -6;

               Value = Value - 0x30;
               if (Negative) Value *= -1;

               Param[ NrIndex ] = Param[ NrIndex ] * 10 + Value;
            }

            return 0;
         }

         case 500: { // allow only whitespaces / line-end

            if (Value == 0x0d ) break;

            if (isspace( Value ) ) return 0;

            State = 900;
            return -7;
         }

         case 900: { // wait for line-end (error recovery)

            if (Value == 0x0d ) break;
            return 0;
         }
      }
      State = 999;

      return 1; // Command line ready
   }

//-----------------------------------------------------------------------------
// TCommand::Help - Print available commands
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TCommand::Help(const char *Str)
   {
      MyPrintf(Str);
      HelpFlag = true;
      (*Executor)(Param);
      HelpFlag = false;
   }

//-----------------------------------------------------------------------------
// TCommand::Match - Used by external executor function to test for commands.
//-----------------------------------------------------------------------------
// returns true on command & param-count match
//-----------------------------------------------------------------------------
bool TCommand::Match(const char *Keyword, byte NrParams)
   {
      if (HelpFlag) {
         MyPrintf("(%d) %s\n", NrParams, Keyword);
         return false;
      }

      //printf("Match Cmd: '%s' Keyword: '%s', NrParams: %d\n", Cmd, Keyword, NrParams);
      if (LastError == 0)           return false;  // this command is already executed
      if (strcasecmp(Cmd, Keyword)) return false;  // not this command
      LastError = 1;                               // command recognised, but (maybe) incorrect nr of params
      if (ParamCount != NrParams)   return false;  // incorrect # of params (there might be an other entry with correct #)
      LastError = 0;  // success
      return true;    // execute command
   }

//-----------------------------------------------------------------------------
// TCommand::Execute - Execute parsed commands, as provided by GetLine
//-----------------------------------------------------------------------------
// return:
// 0 = no command
// 1 = command executed
// 2 = command error (incorrect nr of params, command error)
// >2 = single command character
//-----------------------------------------------------------------------------
char TCommand::Execute(int Verbose)
   {
      LastError = 2;  // unknown command

      // -------------------------
      // *** CALL USER HANDLER ***
      // -------------------------
      (*Executor)(Param);

      switch(LastError) {
         case 0 : {
            if (Verbose > 1) MyPrintf("Cmd '%s' gereed.\n", Cmd);
            return 1;
         }
         case 1 : {
            if (Verbose > 0) MyPrintf("Error: # params - cmd '%s'\n", Cmd);
            return 2;
         }
         case 2 : {  // unknown command
            if (Cmd[1] == 0) {
               // single char keyword => return char
               return Cmd[0];
            } else {
               if (Verbose > 0) MyPrintf("Onbekend cmd '%s' (2)\n", Cmd);
               return 2;
            }
            break;
         }
         default : {
            MyPrintf("Error 0924\n");
            return 2;
         }
      }
   }

//-----------------------------------------------------------------------------
// Takt - Drive command parser
//-----------------------------------------------------------------------------
// Returns 0 or single char commands (legacy of I2c workshop)
//-----------------------------------------------------------------------------
char TCommand::Takt(Stream &Ser)
   {
      static bool First = true;

      if (First) {
         First = false;
         Ser.setTimeout(1);
      }

      while (Ser.available() > 0) {
         int r = GetLine(Ser.read());
         if (r == 0) return 0;  // still reading line


         if (r < 0) {
            MyPrintf("Cmd parse err %d\n", r);
            Clear();
            return 0;
         }

         //Print();
         char ch = Execute();
         Clear();

         if (ch > 2) return ch;
      }
      return 0;
   }

//-----------------------------------------------------------------------------
// AddParam - Add command-param to list
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TCommand::AddParam( int Value )
   {
      if (Position >= ((int) sizeof(Cmd) - 1)) return false;

      Cmd[Position++] = (unsigned char ) Value;
      Cmd[Position  ] = 0;

      return true;
   }

#ifndef HEXDUMP_DEFINED
#define HEXDUMP_DEFINED

//-----------------------------------------------------------------------------
// HexDump - Dump Data in hex format
//-----------------------------------------------------------------------------
// No offset, so address displayed at the start of each line starts at 0.
//-----------------------------------------------------------------------------
void HexDump(const void *Data, int Length)
{
   HexDump(Data, Length, 0);
}

/*=====================================================================
 HexDump -
 ---------------------------------------------------------------------*/
// Parameters:
//    Data   - data to be dumped
//    Length - nr of bytes to be dumped
//    Offset - offset of address (from 0), displayed at the start of each line.
//-----------------------------------------------------------------------------
void HexDump( const void *Data, unsigned int Length, unsigned int Offset)
{
   unsigned char *data  = (unsigned char *)Data;
   unsigned int Track1  = 0;
   unsigned int Track2  = 0;

   // HEX part
   for (unsigned int Index=0; Index < Length; Index = Index+16) {
      MyPrintf( "%04x: ", Offset + Index );
      for(unsigned int j=0; j<16; j++) {
         if (Track1 < Length ) {
            MyPrintf( "%02x", data[ Index+j ] );
         } else {
            MyPrintf( "  " );
         }
         MyPrintf( " " );
         Track1++;
      }

      MyPrintf( " "  );

      // ASCII part
      for(unsigned int j=0; j<16; j++) {
         if (Track2 < Length) {
            if (data[Index+j] < 32 ) {
               MyPrintf( "." );
            } else {
               if (data[Index+j] < 127) MyPrintf("%c", data[Index+j]);
               else MyPrintf(".");
            }
         }
         else MyPrintf( " " );
         Track2++;
      }
      MyPrintf( "\n" );
   }
}
#endif

#endif   // MAIN
#endif   // COMMANDS_H