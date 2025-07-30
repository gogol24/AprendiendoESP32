#define led 4


void setup() {

  pinMode(led,OUTPUT);
}
//OUTPUT E INPUT PARA LOS PINES

void loop(){
  digitalWrite(led,HIGH);
  delay(300);
  digitalWrite(led,LOW);
  delay(100);
  //HIGH O LOW PARA ESCRIBIR EN EL PIN
}