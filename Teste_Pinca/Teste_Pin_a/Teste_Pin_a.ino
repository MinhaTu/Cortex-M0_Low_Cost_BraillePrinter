/* Teste para o pulso motorizado */
/* melhores confiurações : 12v, 100 ms
 *                         12v, 300 ms
 * ponteira: com fita e mais arredondada
 */
int motor1 = 5;
int motor2 = 6;
int potTime = A4;

int potTRead;

int potTValue;

int motorFrente(int power, int duration);
int motorRe(int power, int duration);

void setup() {
  // put your setup code here, to run once:
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT); 
  pinMode(potTime, INPUT);
  Serial.begin(9600);
    // put your main code here, to run repeatedly:

}

void loop() {
  
  potTRead = analogRead(potTime);
  potTValue = map(potTRead, 0,1023,0,500);

  Serial.print(" Tempo: ");
  Serial.println(potTValue);
  motorRe(potMValue, potTValue);
  motorFrente(potTValue, potTValue);

}

int motorFrente(int power, int duration){
  digitalWrite(motor2, HIGH);
  digitalWrite(motor1, LOW);
  delay(duration);
  digitalWrite(motor2, 0);
  digitalWrite(motor1, 0);
}

int motorRe(int power, int duration){
  digitalWrite(motor2, LOW);
  digitalWrite(motor1, HIGH);
  delay(duration);
  digitalWrite(motor2, 0);
  digitalWrite(motor1, 0);
}
