
void setup() {                
  // start serial
  Serial.begin(115200); 
  Serial.println("START");

  int t = 7;
  if (t < 0) 
     Serial.println("aap");
     Serial.println("noot");
  Serial.println("mies");
}

void loop() {

}
