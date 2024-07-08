#include "srnemodbus.hpp"

HardwareSerial serial2(2);
String energyData = "";

#define NUM_REGISTERS 35
#define MODBUS_SLAVE_ADDR 255
#define MODBUS_REQUEST_START_ADDR 256
#define REQUEST_DELAY 3000     // Delay in ms between requests to the charge controller over modbus.
#define SETUP_FINISH_DELAY 100 // Delay in ms to wait after setup has finished to allow everything to settle down.
#define JSON_BUFFER_SIZE 2048  // Maximum size for the JSON.

enum STATE
{
    WAIT = 0,
    QUERY = 1,
    TRANSMIT = 2,
    ERR = 3
};
STATE state;

const char *chargeModes[7] = {
    "OFF",      // 0
    "NORMAL",   // 1
    "MPPT",     // 2
    "EQUALIZE", // 3
    "BOOST",    // 4
    "FLOAT",    // 5
    "CUR_LIM"   // 6 (Current limiting)
};

const char *faultCodes[15] = {
    "Charge MOS short circuit",      // (16384 | 01000000 00000000)
    "Anti-reverse MOS short",        // (8192  | 00100000 00000000)
    "PV panel reversely connected",  // (4096  | 00010000 00000000)
    "PV working point over voltage", // (2048  | 00001000 00000000)
    "PV counter current",            // (1024  | 00000100 00000000)
    "PV input side over-voltage",    // (512   | 00000010 00000000)
    "PV input side short circuit",   // (256   | 00000001 00000000)
    "PV input overpower",            // (128   | 00000000 10000000)
    "Ambient temp too high",         // (64    | 00000000 01000000)
    "Controller temp too high",      // (32    | 00000000 00100000)
    "Load over-power/current",       // (16    | 00000000 00010000)
    "Load short circuit",            // (8     | 00000000 00001000)
    "Battery undervoltage warning",  // (4     | 00000000 00000100)
    "Battery overvoltage",           // (2m     | 00000000 00000010)
    "Battery over-discharge"         // (1     | 00000000 00000001)
};
ModbusMaster node;
// Store all the raw data collected from the charge controller.
uint16_t chargeControllerRegisterData[NUM_REGISTERS];

// Was there an error when reading from the charge controller?
uint8_t modbusErr;

// Last time isTime() was run and returned 1.
unsigned long lastTime;

void srnemodbus::setup()
{
    Serial.println("SRNE Setup...");
    serial2.begin(9600, SERIAL_8N1, 16, 17);
    node.begin(MODBUS_SLAVE_ADDR, serial2);
    state = WAIT;

    delay(SETUP_FINISH_DELAY);
    Serial.println("SRNE Setup end");
    lastTime = millis();
}

int getRealTemp(int temp)
{
    return temp / 128 ? -(temp % 128) : temp;
}

void readNode()
{
    static uint32_t i;
    i++;
    // set word 0 of TX buffer to least-significant word of counter (bits 15..0)
    node.setTransmitBuffer(0, lowWord(i));
    // set word 1 of TX buffer to most-significant word of counter (bits 31..16)
    node.setTransmitBuffer(1, highWord(i));

    uint8_t result = node.readHoldingRegisters(MODBUS_REQUEST_START_ADDR, NUM_REGISTERS);
    if (result == node.ku8MBSuccess)
    {
        modbusErr = 0;
        // Serial.println(F("Successfully read from CC"));

        for (int j = 0; j < NUM_REGISTERS; j++)
        {
            chargeControllerRegisterData[j] = node.getResponseBuffer(j);
        }
        printl("TTL connected ");
        state = TRANSMIT;
    }
    else
    {
        modbusErr = 1;
        printl("Failed to connect with device" + String(result));
        Serial.print(F("Failed to read from CC"));
        Serial.print(F(" ("));
        Serial.print(result, HEX);
        Serial.println(F(")"));
    }
}

void readModData()
{
    JsonDocument doc;
    int loadOffset = chargeControllerRegisterData[32] > 6 ? 32768 : 0;

    JsonArray records = doc["records"].to<JsonArray>();
    records[0]["resource"] = "tipoCarga";
    records[0]["data"] = chargeModes[chargeControllerRegisterData[32] - loadOffset];
    records[1]["resource"] = "Cargador_Ampers";
    records[1]["data"] = chargeControllerRegisterData[2] * 0.01;
    records[2]["resource"] = "cargador_MaxAmps";
    records[2]["data"] = chargeControllerRegisterData[13] * 0.01;
    records[3]["resource"] = "cargador_watts";
    records[3]["data"] = chargeControllerRegisterData[9];
    records[4]["resource"] = "cargador_maxwatts";
    records[4]["data"] = chargeControllerRegisterData[15];
    records[5]["resource"] = "cargador_AmpersHoraDia";
    records[5]["data"] = chargeControllerRegisterData[17];
    records[6]["resource"] = "cargador_TotalAmpersHora";
    records[6]["data"] = ((chargeControllerRegisterData[24] * 65536 + chargeControllerRegisterData[25]) * 0.001);
    records[7]["resource"] = "cargador_dailyPower";
    records[7]["data"] = chargeControllerRegisterData[19] * 0.001;
    records[8]["resource"] = "cargador_totalPower";
    records[8]["data"] = ((chargeControllerRegisterData[28] * 65536 + chargeControllerRegisterData[29]) * 0.001);
    records[9]["resource"] = "bateria_Carga";
    records[9]["data"] = chargeControllerRegisterData[0];
    records[10]["resource"] = "bateria_volts";
    records[10]["data"] = chargeControllerRegisterData[1] * 0.1;
    records[11]["resource"] = "bateria_minVolts";
    records[11]["data"] = chargeControllerRegisterData[11] * 0.1;
    records[12]["resource"] = "bateria_maxVolts";
    records[12]["data"] = chargeControllerRegisterData[12] * 0.1;
    records[13]["resource"] = "panel_volts";
    records[13]["data"] = chargeControllerRegisterData[7] * 0.1;
    records[14]["resource"] = "panel_ampers";
    records[14]["data"] = chargeControllerRegisterData[8] * 0.01;
    records[15]["resource"] = "carga_estado";
    records[15]["data"] = chargeControllerRegisterData[10] ? true : false;
    records[16]["resource"] = "carga_volts";
    records[16]["data"] = chargeControllerRegisterData[4] * 0.1;
    records[17]["resource"] = "carga_amps";
    records[17]["data"] = chargeControllerRegisterData[5] * 0.01;
    records[18]["resource"] = "carga_watts";
    records[18]["data"] = chargeControllerRegisterData[6];
    records[19]["resource"] = "carga_maxamps";
    records[19]["data"] = chargeControllerRegisterData[14] * 0.01;
    records[20]["resource"] = "carga_maxwatts";
    records[20]["data"] = chargeControllerRegisterData[16];
    records[21]["resource"] = "dailyAmpHours";
    records[21]["data"] = chargeControllerRegisterData[18];
    records[22]["resource"] = "totalAmpHours";
    records[22]["data"] = ((chargeControllerRegisterData[26] * 65536 + chargeControllerRegisterData[27]) * 0.001);
    records[23]["resource"] = "dailyPower";
    records[23]["data"] = chargeControllerRegisterData[20] * 0.001;
    records[24]["resource"] = "totalPower";
    records[24]["data"] = ((chargeControllerRegisterData[30] * 65536 + chargeControllerRegisterData[31]) * 0.001);

    // records["tipoCarga"] = chargeModes[chargeControllerRegisterData[32] - loadOffset];
    // records["sobrecarga"] = chargeControllerRegisterData[22];
    // records["CargaCompleta"] = chargeControllerRegisterData[23];
    // records["Cargador_Ampers"] = chargeControllerRegisterData[2] * 0.01;
    // records["cargador_MaxAmps"] = chargeControllerRegisterData[13] * 0.01;
    // records["cargador_watts"] = chargeControllerRegisterData[9];
    // records["cargador_maxwatts"] = chargeControllerRegisterData[15];
    // records["cargador_AmpersHoraDia"] = chargeControllerRegisterData[17];
    // records["cargador_TotalAmpersHora"] = ((chargeControllerRegisterData[24] * 65536 + chargeControllerRegisterData[25]) * 0.001);
    // records["cargador_dailyPower"] = chargeControllerRegisterData[19] * 0.001;
    // records["cargador_totalPower"] = ((chargeControllerRegisterData[28] * 65536 + chargeControllerRegisterData[29]) * 0.001);
    // records["bateria_Carga"] = chargeControllerRegisterData[0];
    // records["bateria_volts"] = chargeControllerRegisterData[1] * 0.1;
    // records["bateria_minVolts"] = chargeControllerRegisterData[11] * 0.1;
    // records["bateria_maxVolts"] = chargeControllerRegisterData[12] * 0.1;
    // records["panel_volts"] = chargeControllerRegisterData[7] * 0.1;
    // records["panel_ampers"] = chargeControllerRegisterData[8] * 0.01;
    // records["carga_estado"] = chargeControllerRegisterData[10] ? true : false;
    // records["carga_volts"] = chargeControllerRegisterData[4] * 0.1;
    // records["carga_amps"] = chargeControllerRegisterData[5] * 0.01;
    // records["watts"] = chargeControllerRegisterData[6];
    // records["carga_maxamps"] = chargeControllerRegisterData[14] * 0.01;
    // records["carga_maxwatts"] = chargeControllerRegisterData[16];
    // records["dailyAmpHours"] = chargeControllerRegisterData[18];
    // records["totalAmpHours"] = ((chargeControllerRegisterData[26] * 65536 + chargeControllerRegisterData[27]) * 0.001);
    // records["dailyPower"] = chargeControllerRegisterData[20] * 0.001;
    // records["totalPower"] = ((chargeControllerRegisterData[30] * 65536 + chargeControllerRegisterData[31]) * 0.001);

    serializeJsonPretty(doc, energyData);
    // printl(energyData);
    // JsonObject controller = doc.createNestedObject("Controlador");
    // controller["Modo de carga"] = chargeModes[chargeControllerRegisterData[32] - loadOffset];
    // controller["Temperatura"] = getRealTemp(chargeControllerRegisterData[3] >> 8);
    // controller["Dias"] = chargeControllerRegisterData[21];
    // controller["SobreDescarga"] = chargeControllerRegisterData[22];
    // controller["CargaCompleta"] = chargeControllerRegisterData[23];

    // JsonObject charging = doc.createNestedObject("Cargador");
    // charging["Ampers"] = chargeControllerRegisterData[2] * 0.01;
    // charging["MaxAmps"] = chargeControllerRegisterData[13] * 0.01;
    // charging["Watts"] = chargeControllerRegisterData[9];
    // charging["MaxWatts"] = chargeControllerRegisterData[15];
    // charging["AmpersHoraDia"] = chargeControllerRegisterData[17];
    // charging["TotalAmpersHora"] = ((chargeControllerRegisterData[24] * 65536 + chargeControllerRegisterData[25]) * 0.001);
    // charging["dailyPower"] = chargeControllerRegisterData[19] * 0.001;
    // charging["totalPower"] = ((chargeControllerRegisterData[28] * 65536 + chargeControllerRegisterData[29]) * 0.001);

    // JsonObject battery = doc.createNestedObject("Bateria");
    // battery["Carga"] = chargeControllerRegisterData[0];
    // battery["Volts"] = chargeControllerRegisterData[1] * 0.1;
    // battery["minVolts"] = chargeControllerRegisterData[11] * 0.1;
    // battery["maxVolts"] = chargeControllerRegisterData[12] * 0.1;
    // battery["temperature"] = getRealTemp(chargeControllerRegisterData[3] & 0xFF);

    // JsonObject panels = doc.createNestedObject("Panel");
    // panels["volts"] = chargeControllerRegisterData[7] * 0.1;
    // panels["amps"] = chargeControllerRegisterData[8] * 0.01;

    // JsonObject load = doc.createNestedObject("Carga");
    // load["Estado"] = chargeControllerRegisterData[10] ? true : false;
    // load["volts"] = chargeControllerRegisterData[4] * 0.1;
    // load["amps"] = chargeControllerRegisterData[5] * 0.01;
    // load["watts"] = chargeControllerRegisterData[6];
    // load["maxAmps"] = chargeControllerRegisterData[14] * 0.01;
    // load["maxWatts"] = chargeControllerRegisterData[16];
    // load["dailyAmpHours"] = chargeControllerRegisterData[18];
    // load["totalAmpHours"] = ((chargeControllerRegisterData[26] * 65536 + chargeControllerRegisterData[27]) * 0.001);
    // load["dailyPower"] = chargeControllerRegisterData[20] * 0.001;
    // load["totalPower"] = ((chargeControllerRegisterData[30] * 65536 + chargeControllerRegisterData[31]) * 0.001);

    //     JsonArray faults = doc.createNestedArray("Fallas");
    // int faultId = chargeControllerRegisterData[34];
    // uint8_t count = 0;
    // while (faultId != 0)
    // {
    //     if (faultId >= pow(2, 15 - count))
    //     {
    //         faults.add(faultCodes[count - 1]);
    //         faultId -= pow(2, 15 - count);
    //     }
    //     count += 1;
    // }
    // serializeJsonPretty(doc, Serial);
    // serializeJsonPretty(doc, *ioManager::getSerialBT());
    // Serial.println();

    state = QUERY;
}

void srnemodbus::loop()
{
    if (state == QUERY)
    {
        unsigned long currentTime = millis();
        unsigned long elapsedTime = currentTime - lastTime;
        if (elapsedTime >= 30000)
        {                           // If 1 minute has passed
            lastTime = currentTime; // Update the last time to the current time
            printl("send Data to dashboard");
            ioManager::sendToHttpClient(energyData);
            state = WAIT;
        }
    }

    switch (state)
    {
    case WAIT:
        readNode();
        break;

    case TRANSMIT:

        readModData();
        break;

    case QUERY:
        break;
    }
}