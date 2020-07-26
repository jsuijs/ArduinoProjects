void setup() {                

  // start serial
  Serial.begin(115200); 

  int a =7, b = 4;

  Serial.print("1                >>> ");  Serial.println(1);            
  Serial.print("0                >>> ");  Serial.println(0);              
  Serial.print("783              >>> ");  Serial.println(783);            
  Serial.print("-1               >>> ");  Serial.println(-1);             
  Serial.print("(2>3)            >>> ");  Serial.println((2>3));          
  Serial.print("(a>=b)           >>> ");  Serial.println((a>=b));         
  Serial.print("(7<=7)           >>> ");  Serial.println((7<=7));         
  Serial.print("(4<a)            >>> ");  Serial.println((4<a));          
  Serial.print("(a+7)            >>> ");  Serial.println((a+7));          
  Serial.print("digitalRead(11)  >>> ");  Serial.println(digitalRead(11));
  Serial.print("true             >>> ");  Serial.println(true);           
  Serial.print("false            >>> ");  Serial.println(false);          
  Serial.print("(a!=123)         >>> ");  Serial.println((a!=123));         
  Serial.print("(a==123)         >>> ");  Serial.println((a==123));         
  Serial.print("(a=123)          >>> ");  Serial.println((a=123));          

}

void loop() {
}



