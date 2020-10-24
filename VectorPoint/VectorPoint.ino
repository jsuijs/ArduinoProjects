#include "Vector.h"
#include "Point.h"

 TVector V1, V2;
 TPoint P1;

void setup() {                
   Serial.begin(115200);   
   
   Serial.println("Demo Vector & Point");
   
   // Stel: sensor ziet een object op een bepaalde afstand, wat zijn de coordinaten van dit object?   
   V1.Length = 500;              // gemeten afstand tot object.
   V1.Degrees(45);               // Sensor hoek is 45 graden (naar links, dus)
   P1 = V1;                      // Converteer vector naar X, Y      
   Serial.print("Berekening 1");
   Print();
   
   // Verdraaing van de robot tellen we de hoek op bij de sensor vector
   int RobotHoek = 60;
   V1.Length = 500;              // gemeten afstand tot object.
   V1.Degrees(45 + RobotHoek);   // Sensor hoek is 45 graden (naar links, dus)
   P1 = V1;                      // Converteer vector naar X, Y
   Serial.print("Berekening 2");
   Print();

   // De verplaatsing van de robot tellen we op de positie bij het berekende punt
   TPoint RobotPositie(300, 400);
   V1.Length = 500;              // gemeten afstand tot object.
   V1.Degrees(45 + RobotHoek);   // Sensor hoek is 45 graden (naar links, dus)
   P1 = V1 + RobotPositie;       // Converteer vector naar X, Y
   Serial.print("Berekening 3");
   Print();

   // Stel: we willen naar een punt rijden, rechts van de robot.   
   V1.Length = 500;              // gemeten afstand tot object.
   V1.Degrees(45 + RobotHoek);   // Sensor hoek is 45 graden (naar links, dus)
   
   V2.Length = 300;              // punt op 300 afstand van object
   V2.Degrees(-90 + RobotHoek);  // 90 graden = rechts van object
   
   P1 = V1 + RobotPositie + V2;  // Converteer vector naar X, Y
   Serial.print("Berekening 4");
   Print();
   
   
}

void loop() {
  //
}

void Print()
{
   Serial.print(" P1 is ");
   Serial.print(P1.X);
   Serial.print(", ");
   Serial.println(P1.Y);

   Serial.print(" V2 is ");
   Serial.println(V2.Length);
   Serial.print(", ");
   Serial.println(V2.Degrees());
   Serial.print(", ");
   Serial.print(V2.X());
   Serial.print(", ");
   Serial.println(V2.Y());


}
