//-----------------------------------------------------------------------------
// Vector.h      
//
// Copyright (c) 2013-2020 Joep Suijs - All rights reserved.
//
// RobotLib tags: RED_CORE
//-----------------------------------------------------------------------------
// This file is part of RobotLib.
// 
// RobotLib is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// RobotLib is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with RobotLib.  If not, see <http://www.gnu.org/licenses/>.
// 
// See http://wiki.robotmc.be/index.php?title=RobotLib for more information.
//-----------------------------------------------------------------------------
// Deze file is een RODE CORE file.
// Een RODE file is een standaard file, die voor iedere toepassing gelijk is.
// CORE files behoren tot de kern van RobotLib en worden in iedere toepassing
// gebruikt.
//-----------------------------------------------------------------------------
// tags_end
//-----------------------------------------------------------------------------
#ifndef VECTORH
#define VECTORH

#include <math.h>

class TPoint;

//	A vector can be described either by its Cartesian coordinates (x,y) or 
// by polar coordinates (M, f), where M is the magnitude of the vector, and 
// f is the angle or phase of the vector. The angle is always measured in the 
// counter-clockwise direction, starting from the direction x-axis.
// M = Length
// f = Direction
class TVector 
{ 
   public:
      
      short Length;
      float Angle;    
      
      void  Degrees(int InDegrees);
      int   Degrees();
         
      TVector()                              { Length = 0;        Angle = 0;        }
      TVector(short InLength, float InAngle) { Length = InLength; Angle = InAngle;  };
      TVector(const TPoint  &Point ) { *this = Point;  }
      TVector(const TVector &Vector) { *this = Vector; }

      float X() const { return float(Length) * cos(Angle); }      
      float Y() const { return float(Length) * sin(Angle); }      
      
      //- operators -----------------------------------
      void    operator  = (const TPoint  &Point );
      void    operator  = (const TVector &Vector);

      void    operator += (const TPoint  &Point );
      void    operator += (const TVector &Vector);
   
      TPoint  operator  + (const TVector &Vector);
      TPoint  operator  - (const TVector &Vector);
      TPoint  operator  + (const TPoint  &Point );
      
   private:
      
}; 
 
#endif
