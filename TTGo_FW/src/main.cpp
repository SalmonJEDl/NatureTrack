#include "config.h"

// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Bluetooth Serial object
BluetoothSerial SerialBT;

// Watch objects
TTGOClass *watch;
TFT_eSPI *tft;
BMA *sensor;

uint32_t sessionId = 30;

volatile uint8_t state;
volatile bool irqBMA = false;
volatile bool irqButton = false;

bool sessionStored = false;
bool sessionSent = false;

uint32_t dist; //distance (in cm)
uint32_t steps; // step counter

void initHikeWatch()
{
    // LittleFS
    if(!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        Serial.println("LITTLEFS Mount Failed");
        return;
    }

    // Stepcounter
    
    // Configure IMU
    Acfg cfg;
    cfg.odr = BMA4_OUTPUT_DATA_RATE_50HZ;
    cfg.range = BMA4_ACCEL_RANGE_2G;
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;
    sensor->accelConfig(cfg);
    sensor->enableAccel();

    // Enable BMA423 step count feature
    sensor->enableFeature(BMA423_STEP_CNTR, BMA4_ENABLE);

    // Reset steps
    sensor->resetStepCounter();

    // Turn on step interrupt
    sensor->enableStepCountInterrupt();

    // Side button
    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] {
        irqButton = true;
    }, FALLING);

    // BMA interrupts
    pinMode(BMA423_INT1, INPUT);
    attachInterrupt(BMA423_INT1, [] {
        irqBMA = true;
    }, RISING);

    //!Clear IRQ unprocessed first
    watch->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);
    watch->power->clearIRQ();

    return;
}

void sendDataBT(fs::FS &fs, const char * path)
{
    /* Sends data via SerialBT */
    fs::File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }
    Serial.println("- read from file:");
    while(file.available()){
        SerialBT.write(file.read());
    }
    file.close();
}

void sendSessionBT()
{
    // Read session and send it via SerialBT
    watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
    watch->tft->drawString("Sending session", 20, 80);
    watch->tft->drawString("to Hub", 80, 110);

    // Sending session id
    sendDataBT(LITTLEFS, "/id.txt");
    SerialBT.write(';');
    // Sending steps
    sendDataBT(LITTLEFS, "/steps.txt");
    SerialBT.write(';');
    // Sending distance
    sendDataBT(LITTLEFS, "/distance.txt");
    SerialBT.write(';');
    // Send connection termination char
    SerialBT.write('\n');
}


void saveIdToFile(uint16_t id)
{
    char buffer[10];
    itoa(id, buffer, 10);
    writeFile(LITTLEFS, "/id.txt", buffer);
}

void saveStepsToFile(uint32_t step_count)
{
    char buffer[10];
    itoa(step_count, buffer, 10);
    writeFile(LITTLEFS, "/steps.txt", buffer);
}

void saveDistanceToFile(uint32_t distance)
{
    char buffer[10];
    itoa(distance, buffer, 10);
    writeFile(LITTLEFS, "/distance.txt", buffer);
}

void deleteSession()
{
    deleteFile(LITTLEFS, "/id.txt");
    deleteFile(LITTLEFS, "/distance.txt");
    deleteFile(LITTLEFS, "/steps.txt");
    //deleteFile(LITTLEFS, "/coord.txt");
}

void setup()
{
    Serial.begin(115200);
    watch = TTGOClass::getWatch();
    watch->begin();
    watch->openBL();

    //Receive objects for easy writing
    tft = watch->tft;
    sensor = watch->bma;
    
    initHikeWatch();

    state = 1;

    SerialBT.begin("Hiking Watch");
}

void loop()
{
    switch (state)
    {
    case 1:
    {
        /* Initial stage */
        //Basic interface
        watch->tft->fillScreen(TFT_BLACK);
        watch->tft->setTextFont(4);
        watch->tft->setTextColor(TFT_WHITE, TFT_BLACK);
        watch->tft->drawString("NatureTrack",  50, 25, 4);
        watch->tft->drawString("Press button", 50, 80);
        watch->tft->drawString("to start session", 40, 110);

        //Used for finding the BT address
        //watch->tft->setCursor(45, 140);
        //watch->tft->print(SerialBT.getBtAddressString());
            

        bool exitSync = false;

        //Bluetooth discovery
        while (1)
        {
            /* Bluetooth sync */
            if (SerialBT.available())
            {
                char incomingChar = SerialBT.read();
                if (incomingChar == 'c' and sessionStored and not sessionSent)
                {
                    sendSessionBT();
                    sessionSent = true;
                }

                if (sessionSent && sessionStored) {
                    // Update timeout before blocking while
                    unsigned long  updateTimeout = 0;
                    unsigned long last = millis();
                    while(1)
                    {
                        updateTimeout = millis();

                        if (SerialBT.available())
                            incomingChar = SerialBT.read();
                        if (incomingChar == 'r')
                        {
                            Serial.println("Got an R");
                            // Delete session
                            deleteSession();
                            sessionStored = false;
                            sessionSent = false;
                            incomingChar = 'q';
                            exitSync = true;
                            break;
                        }
                        else if ((millis() - updateTimeout > 2000))
                        {
                            Serial.println("Waiting for timeout to expire");
                            updateTimeout = millis();
                            sessionSent = false;
                            exitSync = true;
                            break;
                        }
                    }
                }
            }
            if (exitSync)
            {
                delay(1000);
                watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
                watch->tft->drawString("NatureTrack",  50, 25, 4);
                watch->tft->drawString("Press button", 50, 80);
                watch->tft->drawString("to start session", 40, 110);
                exitSync = false;
            }

            /*      IRQ     */
            if (irqButton) {
                irqButton = false;
                watch->power->readIRQ();
                if (state == 1)
                {
                    state = 2;
                }
                watch->power->clearIRQ();
            }
            if (state == 2) {
                if (sessionStored)
                {
                    watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
                    watch->tft->drawString("Overwriting",  55, 100, 4);
                    watch->tft->drawString("session", 70, 130);
                    delay(1000);
                }
                break;
            }
        }
        break;
    }
    case 2:
    {
        /* Hiking session initialisation */
        watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
        watch->tft->setCursor(55, 100);
        watch->tft->drawString("Initialising", 55, 100);
        sensor->begin();
        delay(1000);
        state = 3;
        break;
    }
    case 3:
    {
        /* Hiking session ongoing */
        dist = 0;
        steps = 0;
        watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
        watch->tft->drawString("Starting hike", 45, 100);
        delay(1000);
        watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
        watch->tft->setCursor(45, 70);
        watch->tft->print("Steps: 0");
        watch->tft->setCursor(45, 100);
        watch->tft->print("Dist: 0 m");

        while(!irqButton){
            if (irqBMA){
                irqBMA = false;
                steps = sensor->getCounter();
                dist = steps *75;                   //75cm per step
                watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
                watch->tft->setCursor(45, 70);
                watch->tft->print("Steps: ");
                watch->tft->print(steps);
                
                if (dist<100000){//Distance in meters if < 1000m
                    watch->tft->setCursor(45, 100);
                    watch->tft->print("Dist: ");
                    watch->tft->print(dist/100);
                    watch->tft->print(" m");
                }else{//Distance in km if >= 1km
                    watch->tft->setCursor(45, 100);
                    watch->tft->print("Dist: ");
                    watch->tft->print(dist/100000);
                    watch->tft->print(" km");
                }
                
            }

            
        }
        irqButton = false;
        watch->power->clearIRQ();
        
        unsigned long last = millis();
        unsigned long updateTimeout = 0;

        //reset step-counter
        sensor->resetStepCounter();
        state = 4;
        break;
    }
    case 4:
    {
        watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
        watch->tft->drawString("Session finished", 30, 40);
        watch->tft->setCursor(45, 70);
        watch->tft->print("Steps: ");
        watch->tft->print(steps);
        watch->tft->setCursor(30, 100);
        watch->tft->print("Distance: ");
        if (dist<100000){
            watch->tft->print(dist/100);
            watch->tft->print(" m");
        }else{
            watch->tft->print(dist/100000);
            watch->tft->print(" km");
        }
        watch->tft->drawString("Saving session",30, 130);
        watch->tft->drawString("data...", 75, 160);

        //Save hiking session data
        saveIdToFile(sessionId);
        saveStepsToFile(steps);
        saveDistanceToFile(dist);
        sessionStored = true;
        delay(3000);
        watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
        watch->tft->setCursor(15, 70);
        watch->tft->print("Session data saved");
        delay(1000);
        state = 1;  
        break;
    }
    default:
        // Restart watch
        ESP.restart();
        break;
    }
}