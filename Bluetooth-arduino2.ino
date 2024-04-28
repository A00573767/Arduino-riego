/*
Arduino's Bluetooth with Android
*/
const int ledRed = 5; // Terminal donde conectamos nuestro sistema de Luz
const int ledGreen = 3; // Terminal donde conectamos nuestra Bomba de Agua
const int ledBlue = 4; // Terminal Libre/ Variable
const char separadorDatos = '#'; // Carácter que indica que es un Dato lo que enviamos
boolean statusLedRed = true;
boolean statusLedGreen = true;
boolean statusLedBlue = true;
void setup() {
  // Configuramos las terminales donde están conectados los leds como salidas
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  // Inicializamos los leds y deben de prenderse
  digitalWrite(ledRed, statusLedRed);
  digitalWrite(ledGreen, statusLedGreen);
  digitalWrite(ledBlue, statusLedBlue);
  // Inicializamos el puerto serial con una velocidad de 9600
  Serial.begin(9600);
}
// Este metodo se ejecuta infinitas veces
void loop() {
  // Agregamos el caracter # para indicar que es un dato
  Serial.print("#");
  
  // Leemos la entrada analógica A0 y enviamos el resultado por el puerto serial
  Serial.println(analogRead(A0));
  
  // Esperamos un tiempo de 20 milisegundos
  delay(200);
}
/*La función SerialEvent se ejecuta cuando un nuevo dato
 llega al hardware por la terminal Rx*/
void serialEvent() {
 // Si esta disponible el puerto serial leemos los datos
 while (Serial.available()) {
 
  // Obtiene el siguiente byte que se recibió, este es un carácter
  char comando = (char)Serial.read();
  
  // Dependiendo del carácter recibido ejecutamos una acción
  switch (comando) {
    case 'r':
    cambiarLedRojo();
    break;
    case 'a':
    cambiarLedAzul();
    break;
    case 'v':
    cambiarLedVerde();
    break;
    default:
    // Si no es ningún carácter de comando regresamos el siguiente mensaje
    Serial.print(comando);
    Serial.println(": Comando no reconocido");
    break;
    }
  }
}
void cambiarLedRojo() {
  // Cambiamos el estado del LED
  statusLedRed = !statusLedRed;
  digitalWrite(ledRed, statusLedRed);
  Serial.println("Led Rojo cambio");
}
void cambiarLedAzul() {
  // Cambiamos el estado del LED
  statusLedBlue = !statusLedBlue;
  digitalWrite(ledBlue, statusLedBlue);
  Serial.println("Led Azul cambio");
}
void cambiarLedVerde() {
  // Cambiamos el estado del LED
  statusLedGreen = !statusLedGreen;
  digitalWrite(ledGreen, statusLedGreen);
  Serial.println("Led Verde cambio");
}
