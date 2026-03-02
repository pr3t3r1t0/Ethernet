#include <SPI.h>
#include <Ethernet.h>

// ── Configuración de red ──────────────────────────────────
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
EthernetServer webServer(80);
EthernetClient conexion;

// ── Pines del LED RGB ─────────────────────────────────────
const int PIN_R = 3;
const int PIN_G = 5;
const int PIN_B = 8;

// ── Función: controlar LED ────────────────────────────────
void setLed(int r, int g, int b) {
  digitalWrite(PIN_R, r);
  digitalWrite(PIN_G, g);
  digitalWrite(PIN_B, b);
}

// ── Función: enviar página HTML ───────────────────────────
void enviarPagina() {
  conexion.println("HTTP/1.1 200 OK");
  conexion.println("Content-Type: text/html");
  conexion.println("Connection: close");
  conexion.println();
  conexion.println("<!DOCTYPE html>");
  conexion.println("<html lang='es'><head><title>Control LED</title></head>");
  conexion.println("<body style='background:#B5B5B5; text-align:center'>");
  conexion.println("<form style='background:#828282; padding:20px'>");
  conexion.println("<button type='submit' formaction='btVermelho' style='width:300px;height:100px;font-size:30px;color:red;margin:10px'><b>ROJO</b></button><br>");
  conexion.println("<button type='submit' formaction='btVerde'    style='width:300px;height:100px;font-size:30px;color:green;margin:10px'><b>VERDE</b></button><br>");
  conexion.println("<button type='submit' formaction='btAzul'     style='width:300px;height:100px;font-size:30px;color:blue;margin:10px'><b>AZUL</b></button><br>");
  conexion.println("<button type='submit' formaction='btDesligar' style='width:300px;height:100px;font-size:30px;margin:10px'><b>APAGAR</b></button><br>");
  conexion.println("<button type='submit' formaction='btSair'     style='width:150px;height:50px;font-size:20px;margin:10px'><b>SALIR</b></button>");
  conexion.println("</form></body></html>");
}

// ── Setup ─────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  setLed(0, 0, 0); // LED apagado al inicio

  Ethernet.begin(mac, ip);
  webServer.begin();

  Serial.print("Servidor en: ");
  Serial.println(Ethernet.localIP());
}

// ── Loop ──────────────────────────────────────────────────
void loop() {
  conexion = webServer.available();

  if (conexion) {
    Serial.println("Nueva conexión");
    String recibido = "";
    boolean lineaEnBlanco = true;
    int cod = 0;

    while (conexion.connected()) {
      if (conexion.available()) {
        char c = conexion.read();
        recibido += c;

        // Detectar qué botón se presionó
        if      (recibido.endsWith("/btVermelho")) cod = 1;
        else if (recibido.endsWith("/btVerde"))    cod = 2;
        else if (recibido.endsWith("/btAzul"))     cod = 3;
        else if (recibido.endsWith("/btDesligar")) cod = 4;
        else if (recibido.endsWith("/btSair"))     cod = 5;

        // Fin de cabecera HTTP → responder
        if (c == '\n' && lineaEnBlanco) {
          // Aplicar color según botón
          if      (cod == 1) setLed(1, 0, 0); // Rojo
          else if (cod == 2) setLed(0, 1, 0); // Verde
          else if (cod == 3) setLed(0, 0, 1); // Azul
          else if (cod == 4) setLed(0, 0, 0); // Apagar

          if (cod == 5) {
            conexion.stop(); // Cerrar conexión sin enviar página
          } else {
            enviarPagina();  // Enviar HTML con los botones
          }
          break;
        }

        lineaEnBlanco = (c == '\n') ? true : (c != '\r' ? false : lineaEnBlanco);
      }
    }

    delay(1);
    conexion.stop();
    Serial.println("Conexión cerrada");
  }
}
