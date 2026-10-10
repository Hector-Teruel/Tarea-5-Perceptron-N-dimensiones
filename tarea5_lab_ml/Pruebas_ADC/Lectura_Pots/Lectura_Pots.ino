const int pinesAnalogicos[] = {34, 35, 32, 33, 39};
//const int pinesAnalogicos[] = {34};
const int numEntradas = 5;//5

void setup() {
    Serial.begin(115200);
    analogReadResolution(12);
}

void loop() {
    for (int i = 0; i < numEntradas; i++) {
        int lectura = analogRead(pinesAnalogicos[i]);

        Serial.print("Entrada ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(lectura);
    }

    Serial.println("----------------");
    delay(500);
}
