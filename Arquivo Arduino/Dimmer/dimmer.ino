#include <SPI.h>         
#include <Ethernet.h>
#include <EthernetUdp.h>     

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 105);
unsigned int localPort = 8888;      
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; 
char  ReplyBuffer[] = "COMANDO N EXECUTADO ERRO DE SINTAXE";       

EthernetUDP Udp;

int CharToInt(char vet[]);
boolean Valida(char vet[],int pino);
void comando(char vet[],boolean ok,int pin);
long getDecimal(float val);
int load = 3;
volatile int power = 40;
int botao_0 = A0, botao_1 = A3;
int valorAnalog;
int estadoAnalog;
int chave = 8;
int porta = 9;
boolean validado = 0;
boolean chaveLigada = 0;

void setup() {
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  Serial.begin(9600);
  Serial.println("Iniciou!!");

  pinMode(load, OUTPUT);
  attachInterrupt(0, zero_cross_int, RISING);
  pinMode(botao_0, INPUT);
  pinMode(botao_1, INPUT);
  digitalWrite(botao_0, LOW);
  digitalWrite(botao_1, LOW);
}

void loop(){
  String cmd;
  int packetSize = Udp.parsePacket();
  if(packetSize) {
    Serial.println(" ");
    Serial.println(" ");
    Serial.print("TAMANHO DO PACOTE RECEBIDO: ");
    Serial.println(packetSize);
    Serial.print("IP DE ORIGEM DA MENSAGEM: ");
    IPAddress remote = Udp.remoteIP();
    for (int i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", PORTA: ");
    Serial.println(Udp.remotePort()); 
    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    Serial.print("MENSAGEM RECEBIDA: ");
    Serial.println(packetBuffer);
    if ((packetBuffer[0] == 'D') || (packetBuffer[0] == 'A')) { 
      cmd = (packetBuffer[0]);
      cmd = cmd + (packetBuffer[1]);
      Serial.print("PINO:");
      int pinn = getPino(packetBuffer);
      Serial.println(pinn);   
      boolean validado = validaComando(packetBuffer, pinn); 
      Serial.print("COMANDO VALIDADO? (BOLEANO): ");
      Serial.println(validado);
      comando(validado, pinn, cmd, valorAnalog);
    }else if ((packetBuffer[0] == 'T') && (packetBuffer[1] == 'C')){
      Serial.print("Resposta: TCOK ");
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write("TCOK");
    }     
    Udp.endPacket();
  }
  delay(10);
}

void zero_cross_int(){
  int powertime = (82 * (100 - power));

  if (powertime <= 820){
    digitalWrite(load, HIGH);
    digitalWrite(13, HIGH);
  } else if (powertime >= 8000){
    digitalWrite(load, LOW);
    digitalWrite(13, LOW);
  } else if ((powertime > 820) && (powertime < 8000)){
    delayMicroseconds(powertime);
    digitalWrite(load, HIGH);
    delayMicroseconds(8);
    digitalWrite(load, LOW);
  }
}

int getPino(char vet[]){
  int und;
  und = int(vet[3]);
  und-=48;
  int dez;
  dez = int(vet[2]);
  dez-=48;
  dez=dez*10;
  int aux;
  aux = dez+und;
  return aux;
}

boolean validaComando(char vet[],int pino){
  boolean retorno;
  retorno = false;
  if ((toUpperCase(vet[0]) == 'D') && (toUpperCase(vet[1]) == 'W')) {
    if ((toUpperCase(vet[4]) == 'H') || (toUpperCase(vet[4]) == 'L')){
      if (vet[4] == 'H') { 
        valorAnalog = 1;
      } else if (vet[4] == 'L') {
        valorAnalog = 0;
      }
      retorno = true;
    }else {
      if ((vet[4] == '+') || (vet[4] == '1')) { 
        valorAnalog = 1;
      } else if ((vet[4] == '-') || (vet[4] == 0)) {
        valorAnalog = 0;
      }
      retorno = true;
    }
  } else if (toUpperCase(vet[0]) == 'A') {
    if (((pino>=0)&&(pino<=5)) || ((pino>=0)&&(pino<=5))) {
      if (toUpperCase(vet[1]) == 'R'){
        retorno = true;
      } else if (toUpperCase(vet[1]) == 'W') {
        int centena,dezena,unidade;
        centena = (int(vet[4])-48)*100;
        dezena  = (int(vet[5])-48)*10;
        unidade = (int(vet[6])-48);
        valorAnalog = centena+dezena+unidade;        
        if ((valorAnalog>=0)&&(valorAnalog<=255)){
          retorno = true;
        }
      }
    }
  } else if ((toUpperCase(vet[0]) == 'D') && (toUpperCase(vet[1]) == 'R')) {
    if (((pino>=0)&&(pino<=13)) || ((pino>=22)&&(pino<=53))) {
        retorno = true;
    }
  } 
  return (retorno);
}    

void comando(boolean validado, int pin, String comando, int valor){
  if(validado){
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    comando.toUpperCase();
    if (comando == "DW") {
      pinMode(pin,OUTPUT);
      digitalWrite(pin, valor);
      Serial.println("COMANDOS EXECUTADOS");
      Serial.print("pinMode(");
      Serial.print(pin);
      Serial.println(", OUTPUT);");      
      
      if ((valorAnalog == '-') || (valorAnalog == 0)){
        if (power > 0){
          power = power - 10;
        }
        delay(200);
      }else if ((valorAnalog == '+') || (valorAnalog == 1)){
        if (power < 100){
          power = power + 10;
        }
        delay(200);
      }
      digitalWrite(pin, power);
      Serial.print("digitalWrite(");
      Serial.print(pin);
      Serial.print(", ");
      Serial.print(power);
      Serial.println(");");
      Serial.println("Resposta do Arduino: DWOK");
      Udp.write("DWOK");
    
    }
  }else{
    Serial.println("Resposta do Arduino: ERRO");
    Udp.write("ERRO");
  } 
}  

long getDecimal(float val)
{
  int intPart = int(val);
  long decPart = 1000*(val-intPart); 
  if(decPart>0)  
    return(decPart);    
  else if(decPart<0) 
    return((-1)*decPart);
  else 
    return(00);          
}
