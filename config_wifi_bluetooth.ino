/*
  Configuración de conexion WiFi via Bluetooth
  Autor: Ing. Moreno Lucas Omar  
  Año: 2020
 */

#include "BluetoothSerial.h"
#include <WiFi.h>
#include <EEPROM.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

// Credenciales WiFi
String ssid = "";
String password = "";

// Flags
bool wifi_activado = true;

// Funcion: Grabar EEPROM
void grabar_eeprom(int addr, String text){
  int tamano = text.length();
  char inChar[50];
  text.toCharArray(inChar, tamano+1);
  for(int i = 0; i < tamano; i++){
    EEPROM.write(addr+i, inChar[i]);
  }
  for(int i = tamano; i < 50; i++){
    EEPROM.write(addr+i, 255);    
  }
  EEPROM.commit();
}

// Funcion: Leer EEPROM
String leer_eeprom(int addr){
  byte lectura;
  String strLectura;
  for(int i = addr; i < addr+50; i++){
    lectura = EEPROM.read(i);
    if(lectura != 255){
      strLectura += (char)lectura;
    }
  }
  return strLectura;
}

// Funcion: Conexion/Reconexion a red WiFi
void reconnect_wifi(){
  Serial.println("Conexion WiFi");
  Serial.print("Conectando a -> ");
  Serial.println(ssid);
  WiFi.begin(ssid.c_str(), password.c_str());
  while(WiFi.status() != WL_CONNECTED && wifi_activado){
    if(SerialBT.available()) decodificadorSerial();       // -> Se puede mandar un comando mientras reconecta     
    Serial.print(".");
    delay(200);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(200);
    digitalWrite(BUILTIN_LED, LOW);
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("");
    Serial.println("Conexion exitosa!");
  
    Serial.println("Mi IP es -> ");
    Serial.println(WiFi.localIP());    
  }
}

// Funcion: Se realiza la accion en funcion al "comando" y "valor" recibido
void realizarAccion(String comando, String valor){

  // --------------- COMANDO WiFi ----------------------
  
  // Seteo de SSID - WiFi
  if(comando.equals("ssid")){
    Serial.print("Nuevo SSID configurado - ");
    Serial.println(valor);
    ssid = valor;
    grabar_eeprom(0, valor); // 0 - 50 -> SSID
    SerialBT.write('y');
  }
  // Seteo de Password - WiFi
  else if(comando.equals("password")){
    Serial.print("Nuevo Password configurado - ");
    Serial.println(valor);
    password = valor;
    grabar_eeprom(50, valor); // 50 - 100 -> Password
    SerialBT.write('y');
  }
  // Establecer conexion WiFi
  else if(comando.equals("connect")){
    if(valor.equals("wifi")){
      reconnect_wifi();
      wifi_activado = true;      
    }
  }
  // Desconectar de red WiFi
  else if(comando.equals("disconnect")){
    if(valor.equals("wifi")){
      WiFi.disconnect();
      wifi_activado = false;
      digitalWrite(BUILTIN_LED, LOW);
      Serial.println("Conexion WiFi finalizada..."); 
    }
    delay(100);
  }
  // Comando desconocido
  else{
    Serial.println("Comando desconocido");
    SerialBT.write('n');
  }
  
}

// Funcion: Decodificacion de mensaje Bluetooth
void decodificadorSerial(){
  String mensaje = SerialBT.readStringUntil('\n');
  Serial.print("Mensaje: ");
  Serial.println(mensaje);  
  int posicionGuion = mensaje.indexOf('-');
  int posicionSaltoLinea = mensaje.length();
  String comando = mensaje.substring(0,posicionGuion);
  String valor = mensaje.substring(posicionGuion + 1, posicionSaltoLinea);
  Serial.print("Comando: ");
  Serial.print(comando);
  Serial.print(" | ");
  Serial.print("Valor: ");
  Serial.println(valor);
  realizarAccion(comando, valor);
}

// Funcion: Configuracion inicial de WiFi
void config_wifi(){
  ssid = leer_eeprom(0);
  password = leer_eeprom(50);  
}

// Funcion: Configuracion inicial de bluetooth
void config_bluetooth(){
  SerialBT.begin("ESP32_ET"); // Nombre de dispositivo Bluetooth
  Serial.println("Dispositivo inicializado...");
  Serial.println("Ahora puedes establecer una conexion bluetooth!");  
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  config_wifi();
  config_bluetooth();
}

void loop() {

  // Led - Conexion WiFi correcta
  if(WiFi.status() == WL_CONNECTED) digitalWrite(BUILTIN_LED, HIGH);
  else digitalWrite(BUILTIN_LED, LOW);
  
  // Reconexion WiFi
  if(WiFi.status() != WL_CONNECTED && wifi_activado) reconnect_wifi();
 
  // Bluetooth -> Puerto Serie
  if(SerialBT.available()) decodificadorSerial();
  
  delay(20);
}
