const int touchPin = 0;// the input pin where touch sensor is connected
const int touchDelay = 100;//millisecond delay between each touch

int count=0;// variable holding the count number
int Bstate;
void setup() {
  Serial.begin(9600);// initialize serial monitor with 9600 baud
  pinMode(touchPin,INPUT);// define a pin for touch module
  
}

void loop() {
   int touchValue = digitalRead(touchPin);
   if(touchValue == HIGH)  {
   count++;
   if (count < 4)
    {
     Bstate=count;
     }
   else 
   {
    count = 0;
   }
 }
 Serial.print(" ; count "); 
  Serial.print(count);//print count
  Serial.print(" ; Bstate "); 
  Serial.println(Bstate);//print count
   delay(touchDelay);   
}



      
