#include <L298NX2.h>

#define EN_A 6
#define IN1_A 7
#define IN2_A 4
#define EN_B 5
#define IN1_B 3
#define IN2_B 2
#define senzor_stanga A1
#define senzor_dreapta A3
#define senzor_mijloc A2
#define senzor_obsInfo A0

int input_stanga;
int input_dreapta;
int input_mijloc;
int input_obstacol;
int previous_read=1;
int counter = 0;

char motorA;
char motorB;
char command;
char byte_output;
int prev_input = 0;
L298N::Direction motorAdir;
L298N::Direction motorBdir;
//instantierea librariei
//initializarea motoarelor
L298NX2 powerTrain(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);

//decide in functie de serverCmd ce comanda sa execute
L298N::Direction getCommand(char serverCmd){
  if(serverCmd == 1) {
    return L298N::STOP;
  }
  if(serverCmd == 2) {
    return L298N::FORWARD;
  }
  if(serverCmd == 3) {
    return L298N::BACKWARD;
  }
}

void setup() {
  Serial.begin(9600);
   // configurare pini pentru butoane, intrare
  pinMode(senzor_dreapta, INPUT); //senzor de linie dreapta 
  pinMode(senzor_stanga, INPUT); // senzor de linie stanga
  pinMode(senzor_mijloc, INPUT); //senzor de linie mijloc
  pinMode(senzor_obsInfo, INPUT); //senzor obstacol -logica negativa 
 
  powerTrain.setSpeedA(80);
  powerTrain.setSpeedB(80);
  powerTrain.stopA();
  powerTrain.stopB();
}
//
//void readInputAndSendToNode(){
//   input_stanga = digitalRead(senzor_stanga);
//   input_dreapta = digitalRead(senzor_dreapta);
//   input_mijloc = digitalRead(senzor_mijloc);
//   input_obstacol = digitalRead(senzor_obsInfo);
//   
//   byte_output = 0;
//   byte_output += input_stanga;
//   byte_output += 2 * input_mijloc;
//   byte_output += 4 * input_dreapta;
//   byte_output += 8 * input_obstacol;
//   if(byte_output != prevbyte_output){
//     Serial.write(byte_output);
//   }
//   prevbyte_output = byte_output;
//}
//
//void readCommandAndExecute(){
//     String mot = Serial.readString();//read pwm motor A, B and command
//     motorA = mot[0];
//     motorB = mot[1];
//     command = mot[2];
//     powerTrain.setSpeedA((unsigned short int)motorA);
//     powerTrain.setSpeedB((unsigned short int)motorB);
//     motorAdir = getCommand(3 & command); // selects the important bits for motor A and tried to decide direction
//     motorBdir = getCommand((12 & command) >> 2); // selects the important bits for motor B and tried to decide direction
//     powerTrain.runA(motorAdir);
//     powerTrain.runB(motorBdir);
//   
//}
//motorA-viteza pentru motorul A, motorB-viteza pentru motorulB, command-comanda
void Execute(char motorA, char motorB, char command){
     powerTrain.setSpeedA((unsigned short int)motorA);
     powerTrain.setSpeedB((unsigned short int)motorB);
     motorAdir = getCommand(3 & command); //  selectează bitii importanti pentru motrul A si decide directia
     motorBdir = getCommand((12 & command) >> 2); // selectează bitii importanti pentru motrul B si decide directia
     powerTrain.runA(motorAdir);
     powerTrain.runB(motorBdir);
}


void readInputAndSendToNode(){
   prev_input = input_stanga+input_dreapta+input_mijloc+input_obstacol;
   input_stanga = digitalRead(senzor_stanga);
   input_dreapta = digitalRead(senzor_dreapta);
   input_mijloc = digitalRead(senzor_mijloc);
   input_obstacol = digitalRead(senzor_obsInfo);
}


void avoidObstacle(){
  Serial.print("Obstacol:");
  Serial.println(!input_obstacol);
  if (!input_obstacol){
    powerTrain.setSpeedB(120); //Go bwd
    powerTrain.setSpeedA(120);
    powerTrain.runB(L298N::FORWARD);
    powerTrain.runA(L298N::FORWARD);
    delay(500);
    powerTrain.setSpeedB(0);//Go right 
    powerTrain.setSpeedA(120);
    powerTrain.runB(L298N::BACKWARD);
    powerTrain.runA(L298N::BACKWARD);
    delay(800);
    powerTrain.setSpeedB(120); // go fwd
    powerTrain.setSpeedA(120);
    powerTrain.runB(L298N::BACKWARD);
    powerTrain.runA(L298N::BACKWARD);
    delay(500);
    powerTrain.setSpeedB(130); // go left
    powerTrain.setSpeedA(0);
    powerTrain.runB(L298N::BACKWARD);
    powerTrain.runA(L298N::BACKWARD);
    delay(1300);
    powerTrain.setSpeedB(120); // go fwd
    powerTrain.setSpeedA(120);
    powerTrain.runB(L298N::BACKWARD);
    powerTrain.runA(L298N::BACKWARD);
    //citim doar daca senzorii trimit informatie diferita fata de cea precedenta
    //cat timp vede obstacol executa rutina si nu mai citeste linia
    while(input_stanga+input_dreapta+input_mijloc+input_obstacol != prev_input)
    {
      readInputAndSendToNode();
    }
  }
}

void followLine() {
  if (!input_stanga && !input_mijloc && input_dreapta) { // Linia e sub dreapta
    Serial.println("Right Right");
    powerTrain.setSpeedB(140);
    powerTrain.setSpeedA(0);
    powerTrain.runB(L298N::BACKWARD);
    powerTrain.runA(L298N::BACKWARD);
    delay(6);
  }
  
  if (!input_stanga && input_mijloc && input_dreapta) {
    Serial.println("Right");
    powerTrain.setSpeedB(120);
    powerTrain.setSpeedA(0);
    powerTrain.runB(L298N::BACKWARD);
    powerTrain.runA(L298N::BACKWARD);
    delay(6);
  }
  
  if (input_stanga && !input_mijloc && !input_dreapta) { // Linia e sub stanga
    Serial.println("Left Left");
    powerTrain.setSpeedB(0);
    powerTrain.setSpeedA(140);
    powerTrain.runB(L298N::BACKWARD);
    powerTrain.runA(L298N::BACKWARD);
    delay(6);
  }
  
  if (input_stanga && input_mijloc && !input_dreapta) {
    Serial.println("Left");
    powerTrain.setSpeedB(0);
    powerTrain.setSpeedA(120);
    powerTrain.runB(L298N::BACKWARD);
    powerTrain.runA(L298N::BACKWARD);
    delay(6);
  }
  
  if (!input_stanga && input_mijloc && !input_dreapta) { 
    Serial.println("Going forward");
    powerTrain.setSpeedB(95);
    powerTrain.setSpeedA(90);
    powerTrain.runB(L298N::BACKWARD);
    powerTrain.runA(L298N::BACKWARD);
    delay(6);
  }
  //conditia de oprire
  if (input_stanga && input_mijloc && input_dreapta) {
    Execute(255,255,'E');
    delay(6);
  }
//daca nu vede linia se tot roteste pana o gaseste
  if (!input_stanga && !input_mijloc && !input_dreapta) {
    powerTrain.setSpeedB(120);
    powerTrain.setSpeedA(120);
    powerTrain.runB(L298N::FORWARD);
    powerTrain.runA(L298N::BACKWARD);
    delay(6);
  }
//caz imposibil teoretic
  if (input_stanga && !input_mijloc && input_dreapta) {
    Execute(255,255,'E');
    delay(6);
  }
}

   
void loop() {
   readInputAndSendToNode();
   avoidObstacle();
   if (input_stanga+input_dreapta+input_mijloc+input_obstacol != prev_input){
    followLine();
   }
   
}
