
int quad, color; // Define variables to indicate quadrant # and color #
char a = 'E'; // Define character E to indicate new set of data transfer

void setup()
{
  Serial.begin(9600); // Your code must operate that this speed.
}
void loop()
{ quad = 2; 
  color = 2;
  
  /* ***** The following 5 lines of code are IMPORTANT and must be executed in this EXACT order ***** */ 

  Serial.write(a); //FIRST: Send 'E' which indicates a new set of data is being transmitted
                   
  Serial.write(quad/256); // SECOND: Send the quadrant number as two bytes
  Serial.write(quad%256); 
  
  Serial.write(color/256); // THIRD: Send the color number as two bytes
  Serial.write(color%256);
  
  delay(1000);
}

 
