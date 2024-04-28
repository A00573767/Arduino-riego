
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <EEPROMex.h>
//#include <EEPROMVar.h>







// 2. Conexiones

// Joystick
#define xPin A6 //naranja
#define yPin A3
#define kPin 7
//Bombas de agua
#define pinS1 5
#define pinS2 11
#define pinS3 10
#define pinS4 3
//Sensores de humedad
#define moistureSensor A0
#define moistureSensor2 A1
#define moistureSensor3 A2
//SDA A4
//SCL A5







// 3. Variables y Comandos

//leerJoystick
byte joyRead;
byte joyPos;
byte lastJoyPos;
long lastDebounceTime = 0;
long debounceDelay = 70;


//Variables de humedad
int humedad;
int humedadlimite = 30;



//Control Joystick
bool PQCP;//valores del joystick en el display
byte editMode;
//sistema de menus
byte mNivel1;


byte nS;//duda
//Hora
DateTime now;
int horaAc;// Hora actual en minutos (0 a (1440-1))
byte lastMinute = 0;
byte lastSecond = 0;
byte timer;
//
bool IO=1;//
byte percent=100;
byte clearSave;
//
byte buffer[2]; //puerto de datos arreglo de 2 bytes
byte nH;//duda
byte progRec[4][9];// Tr, h1,m1...h4,m4 (for eprom) //duda guardar
byte controlPin[4]={pinS1,pinS2,pinS3,pinS4}; //arreglo de bombas
bool controlS[4]; //horario
byte TAM[4]; //duda
bool a; // bandera 



// 4. Objetos
RTC_DS1307 rtc; // reloj
LiquidCrystal_I2C lcd(0x27,16,2); //  pantalla LCD 16x2 (tenemos direccion 0x27)



// SETUP
void setup() {
  // S1. Pines
  Serial.begin(9600);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(kPin, INPUT_PULLUP);
  digitalWrite(pinS1,LOW);
  digitalWrite(pinS2,LOW);
  digitalWrite(pinS3,LOW);
  // digitalWrite(pinS4,LOW);
  pinMode(pinS1, OUTPUT);
  pinMode(pinS2, OUTPUT);
  pinMode(pinS3, OUTPUT);
  //pinMode(pinS4, OUTPUT);




  // S2. Objetos
  rtc.begin();
  lcd.init();
  lcd.backlight();
  
  

  // S3. Program

  eepromRead();
}




//------------------------------------------------------------
// LOOP todo lo del inicio
void loop() {
  now = rtc.now();
  horaAc = now.hour()*60 + now.minute();
  int sensorValue = analogRead(moistureSensor);
  humedad = (100 - map(sensorValue, 0, 1023, 0, 99));



  controlJoystick();

  menu();



  for(int i=0;i<4;i++ ){
    //si ON y app o programa o arranque manual y humedad de la tierra es inferior al valor marcado
    if ((IO)&&/*(controlS[i]||(TAM[i]>0)))&&*/(humedad>humedadlimite) ){
      
      digitalWrite(controlPin[i], HIGH);//off relay
    } else {
      digitalWrite(controlPin[i], LOW);//on relay
    }
  }//for


  if(now.second()!=lastSecond){
    controlH();
    timer++;
    lastSecond=now.second();
  }
  if (timer>30&&!(controlS[0]||controlS[1]||controlS[2]||controlS[3])&&!(TAM[0]||TAM[1]||TAM[2]||TAM[3])){
    lcd.noBacklight();
  }
  else{
    lcd.backlight(); 
  }
  if (timer==90){
    lcd.clear();
    mNivel1=0;
  }
  //vuelve a pantalla de inicio tras 90s de inactividad
  if (timer>250){timer=91;}//32k
  if(now.minute()!=lastMinute){
    for(int i=0;i<4;i++){
      if (TAM[i]>0){TAM[i]--;}
    }
    //for
    lastMinute=now.minute();
  }

}



//----------------------------------------------------------
//sale loop



//---------------------
//con

void controlH(){
  for(int g=0;g<4;g++){//sector
    for(int i=1;i<8;i+=2){//si t!=0 y hini !=0 y horario
    
      if((progRec[g][0]!=0)&&(((progRec[g][i]*60)+(progRec[g][i+1]))!=0)&&((horaAc>=((progRec[g][i]*60)+(progRec[g][i+1])))&&(
      horaAc<((progRec[g][i]*60)+(progRec[g][i+1])+int(progRec[g][0]*(percent/100)))))){
        a=1;
      }
    }//for horas
    
    if (a==1){controlS[g]=1;}
    if (a==0){controlS[g]=0;}
    a=0;
  }//for sector
}

 
void menu(){
  switch (mNivel1){
    case 0:
    menu0();//pantalla de inicio
    break;
    case 1:
    if(nS>0){menu11();
  }
  else{
    menu1();
  }
  break;
  case 2:
  menu2();
  break;
  case 3:
  if(nS>0){
    menu31();
  }
  else{
    menu3();
  }
  break;
  case 4:
  menu4(); //
  break;
  case 5:
  menu5(); //
  break;


  //inteto de hacer algo
  case 6:
  menu6();


  }//switch
}





// Pantalla de inicio
void menu0(){
  lcd.setCursor(0,0);
  printHour(now.hour());
  printDigits(now.minute());
  lcd.setCursor(7,0);
  if(IO){
    lcd.print("PREPARADO");
  }
  if(!IO){lcd.print("TODO OFF");
  }
  // if(timer & 0x01){ par
  if(timer%4==0){
    lcd.setCursor(0,1);
    lcd.print("HUMEDAD");
    lcd.setCursor(12,1);
    lcd.print(humedad);
    if (humedad < 10) {
     lcd.setCursor(13, 1); 
    }
    lcd.print("%");
    /* lcd.setCursor(4,1);
    lcd.print("S2 ");
    lcd.setCursor(8,1);
    lcd.print("S3 ");
    lcd.setCursor(12,1);
    lcd.print("S4 ") ; */
  }
  else{
    /* lcd.setCursor(6,1);
    if (TAM[0]>0){lcd.print("mON");
    }else if (controlS[0]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
    lcd.setCursor(4,1);
    if (TAM[1]>0){lcd.print("mON");
    }else if (controlS[1]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
    lcd.setCursor(8,1);
    if (TAM[2]>0){lcd.print("mON");
    }else if (controlS[2]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
    lcd.setCursor(12,1);
    if (TAM[0]>3){lcd.print("mON");
    }else if (controlS[3]>0){lcd.print("hON");
    }else {lcd.print("OFF");}*/
  }
}
//----------------------------------------------------
// Menu 1 riego manual 
void menu1(){
  lcd.setCursor(0,0);
  lcd.print("RIEGO MANUAL");
  lcd.setCursor(0,1);
  lcd.print("configurar -> ->");
}



//-------------------------------------------------1.1
void menu11(){
  lcd.setCursor(0,0);
  lcd.print("Riego manual ");
  // lcd.print(nS);
  lcd.setCursor(13,0);
  if(TAM[nS-1]>0)lcd.print("ON ");
  if(TAM[nS-1]==0)lcd.print("OFF");
  lcd.setCursor(0,1);
  lcd.print("Minutos ");
  printHour(TAM[nS-1]);
}





// SET PERCENT Menu 2
//----------------------------------------------------
void menu2(){
  lcd.setCursor(0,0);
  lcd.print("% HUMEDAD LIMITE");
  lcd.setCursor(6,1);
  lcd.print(humedadlimite);
  if (editMode>0){
    lcd.setCursor(15,1);
    lcd.print("#");
  } 
  else {
    lcd.setCursor(15,1);
    lcd.print(" ");
  }
}
//----------------------------------------------------
// SET PROGRAM Menu 3 
//----------------------------------------------------
void menu3(){
  lcd.setCursor(0,0);
  lcd.print("RIEGO AUTOMATICO");
  lcd.setCursor(0,1);
  lcd.print("configurar -> ->");
}



//-------------------------------------------------3.1
void menu31(){
  lcd.setCursor(0,0);
  if (editMode>0){
    lcd.print("#");
  } 
  else {
    lcd.print("S");
  }
  lcd.setCursor(1,0);
  lcd.print(nS);
  lcd.setCursor(0,1);
  lcd.print("T ");//tr
  printHour(progRec[nS-1][0]);//tr
  lcd.setCursor(5,0);
  printHour(progRec[nS-1][1]);//h1
  lcd.setCursor(7,0);
  printDigits(progRec[nS-1][2]);
  lcd.setCursor(11,0);
  printHour(progRec[nS-1][3]);//h2
  lcd.setCursor(13,0);
  printDigits(progRec[nS-1][4]);
  lcd.setCursor(5,1);
  printHour(progRec[nS-1][5]);//h3
  lcd.setCursor(7,1);
  printDigits(progRec[nS-1][6]);
  lcd.setCursor(11,1);
  printHour(progRec[nS-1][7]);//h4
  lcd.setCursor(13,1);
  printDigits(progRec[nS-1][8]);
}
//----------------------------------------------------
// Clear / Save EEPROM Menu 4
//----------------------------------------------------
void menu4(){
  lcd.setCursor(4,0);
  lcd.print("Opción");
  if (editMode){
    lcd.setCursor(5,1);
    if (clearSave==0){
      lcd.print("Cancelar");
    }
    if (clearSave==1){
      lcd.print("Guardar");
    }
    if (clearSave==2){
      lcd.print("Borrar");
    }
    lcd.setCursor(15,1);
    lcd.print("#");
  } else {
    lcd.setCursor(1,1);
    lcd.print("Borrar/Guardar");
    lcd.setCursor(15,1);
    lcd.print(" ");
  }
}




//----------------------------------------------------
void progRecClear(){
  for(int a=0;a<4;a++){
    for(int b=0;b<8;b++){
      progRec[a][b]=0;
    }
  }
}



//----------------------------------------------------
void eepromWrite(){
  int address=0;
  for(int i=0;i<4;i++){
    for(int j=0;j<8;j++){
    EEPROM.writeByte(address, progRec[i][j]);
    address++;
    }
  }
}



//----------------------------------------------------
void eepromRead(){
  int address=0;
  for(int i=0;i<4;i++){
    for(int j=0;j<8;j++){
      progRec[i][j]=EEPROM.readByte(address);
      address++;
    }
  }
}



//----------------------------------------------------
// Set HORA Menu 5
//----------------------------------------------------
void menu5(){
  lcd.setCursor(0,0);
  lcd.print("Cambia hora");
  if (editMode>0){
    lcd.setCursor(10,0);
    lcd.print("#");
  }
  lcd.setCursor(11,0);
  printHour(buffer[0]);
  printDigits(buffer[1 ]);
  lcd.setCursor(0,1);
  lcd.print("Actual ");
  lcd.setCursor(11,1);
  printHour(now.hour());
  printDigits(now.minute());
}

void menu6() {
  
} 




// Control Joystic
void controlJoystick(){
  leeJoystick();
  if(PQCP) {
    PQCP=0;
    timer=0;
    //----------------------------------------------------
    // JOYSTICK BUTTON== joyPos=5
    //----------------------------------------------------
    if (joyPos==5&&mNivel1==0){IO=!IO;
      //manual start
      //percent
    }
    else if (joyPos==5&&mNivel1==2&&editMode==0){
      editMode=2;
    }
    else if (joyPos==5&&mNivel1==2&&editMode>0){
      editMode=0;
      //set prog
    }
    else if (joyPos==5&&mNivel1==3&&nS>0&&editMode==0){
      editMode=3;
      nH=0;
    }
    else if (joyPos==5&&mNivel1==3&&editMode>0){//on exit
      editMode=0;
      //eprom
    }
    else if (joyPos==5&&mNivel1==4&&editMode==0){
      editMode=4;
      lcd.clear();
    }
    else if (joyPos==5&&mNivel1==4&&editMode>0){//on exit
      if (clearSave==1){//save
        eepromWrite();
      }
      if (clearSave==2){//clear
        progRecClear();
        eepromWrite();
        //syncProg();
      }
      editMode=0;
      //sethora
    }
    else if (joyPos==5&&mNivel1==5&&editMode==0){
      editMode=5;
      nH=0;
      buffer[0]=now.hour();
      buffer[1]=now.minute();
    }
    else if (joyPos==5&&mNivel1==5&&editMode>0){//on exit
      nH=0;
      DateTime dt(2015, 1, 1, buffer[0], buffer[1], 0);
      rtc.adjust(dt);
      editMode=0;
    }



    //OTROS CONTROLES
    switch (editMode){
      case 0: //no editMode
      if (mNivel1==1&&joyPos==3&&TAM[nS-1]>0&&nS>0){
      TAM[nS-1]--;}
      if (mNivel1<5&&joyPos==3&&(mNivel1!=1||nS==0)){ //UP
      lcd.clear();
      mNivel1++;
      nS=0;}
      if (mNivel1==1&&joyPos==4&&TAM[nS-1]<99&&nS>0){ //DOWN
      TAM[nS-1]++;}
      if (mNivel1>0&&joyPos==4&&(mNivel1!=1||nS==0)){
      lcd.clear();
      mNivel1--;
      nS=0;}
      if ((mNivel1==1||mNivel1==3)&&joyPos==1&&nS<1){ //RIGHT
      lcd.clear();
      nS++;}
      if ((mNivel1==1||mNivel1==3)&&joyPos==2&&nS>0){ //LEFT
      lcd.clear();
      nS--;}
      case 1: // editMode Manual start
      break;
      case 2: // editMode Ser Percent
      if (joyPos==4&&humedadlimite<99){//UP
      lcd.clear();
      humedadlimite+=10;}
      if (joyPos==3&&humedadlimite>1){//DOWN
      lcd.clear();
      humedadlimite-=10;}
      break;
      case 3: // editMode Set Program
      //if (joyPos==3&&buffer[nH]>0){buffer[nH]--;} //abajo*/
      if (joyPos==4&&nH==0&&progRec[nS-1][0]<99){progRec[nS-1][0]++; } //arriba
      if (joyPos==4&&(nH==1||nH==3||nH==5||nH==7)&&progRec[nS-1][nH]<23){
      progRec[nS-1][nH]++; }//arriba
      if (joyPos==4&&(nH==2||nH==4||nH==6||nH==8)&&progRec[nS-1][nH]<59){
      progRec[nS-1][nH]++; } //arriba
      if (joyPos==3&&progRec[nS-1][nH]>0){progRec[nS-1][nH]--;} //abajo
      if (joyPos==1&&nH<8){nH++; } //derecha
      if (joyPos==2&&nH>0){nH--; } //izq
      break;
      case 4: // editMode Save/clear Proram
      if (joyPos==3&&clearSave<2){//DOWN
      lcd.clear();
      clearSave++;}
      if (joyPos==4&&clearSave>0){//UP
      lcd.clear();
      clearSave--;}
      break;
      case 5: // editMode Set Time
      if (joyPos==4&&buffer[0]<23&&nH==0){
      buffer[0]++; }//arriba
      if (joyPos==4&&buffer[1]<59&&nH==1){
      buffer[1]++; } //arriba
      if (joyPos==3&&buffer[nH]>0){buffer[nH]--;} //abajo
      if (joyPos==1&&nH<1){nH++; } //derecha
      if (joyPos==2&&nH>0){nH--; } //izq
      break;
    }//!edit
  }//PQCP
}



//----------------------------------------------------
int leeJoystick(){
//----------------------------------------------------
int x = analogRead(xPin);
int y = analogRead(yPin);
int k = digitalRead(kPin);
  if(x>900){
    joyRead=1; //x+
  }
  else if(x<100){
    joyRead=2; //x-
  }
  else if(y>900){
    joyRead=3; //y+
  }
  else if(y<100){
    joyRead=4; //y-
  }
  else if(!k){
    joyRead=5;
  }
  else{
    joyRead=0;
    }
  if (joyRead != lastJoyPos){
    lastDebounceTime = millis();
  }
  if(((millis() - lastDebounceTime) > debounceDelay)&&(joyRead!=joyPos)){
  joyPos=joyRead;
  if(!PQCP){
    PQCP=1;
  }
  }
  if(((millis() - lastDebounceTime) > (4*debounceDelay))&&(joyPos==3||joyPos==4)){
  joyPos=joyRead; //repeat time
  if(!PQCP){PQCP=1;}
  }
  lastJoyPos=joyRead;
  }



//imprime horas
void printHour(byte digits){
  if(digits < 10){
  lcd.print(' ');
  lcd.print(digits,DEC);}
  else {lcd.print(digits,DEC);}
}
//imprime minutos :00
void printDigits(byte digits){
  lcd.print(":");
  if(digits < 10){
    lcd.print('0');
    lcd.print(digits,DEC);
  }
  else {lcd.print(digits,DEC);}
}