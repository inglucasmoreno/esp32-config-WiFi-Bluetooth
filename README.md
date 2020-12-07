# esp32-config-WiFi-Bluetooth
Configuracion de conexion WiFi via Bluetooth Terminal

### Requisitos
- Libreria WiFi.h
- Libreria BluetoothSerial.h
- Libreria EEPROM.h
- Celular con programa Bluetooth Terminal (Para enviar los comandos)

### Comandos via Bluetooth Terminal
- Setear SSID de red WiFi -> ssid-[ssid de red a conectar]
- Setear Password de red WiFi -> password-[password de red a conectar]
- Conectar a Red WiFi -> connect-wifi
- Desconectar de Red WiFi -> disconnect-wifi

Nota: No colocar los corchetes [ ]

### Memoria EEPROM
- Se aprovecha la memoria EEPROM de la ESP32 para almacenar SSID y Password
- Cuando el sistema inicia, levanta automaticamente SSID y Password desde la memoria

### Builtin LED
- Se aprovecha el Builtin LED de la ESP 32 para indicar estado de conexion
- BUILTIN LED Parpadeando -> Esta intentando de conectar
- BUILTIN LED Encendido -> Conexion a red establecida
