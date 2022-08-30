// 参考文献如下
// https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/

const byte pwmPin = 2;

// loopTimer:目的是每秒输出一份结果，单位是millis
unsigned long loopTimer = 0l;
// pwmTimer:目的是记录高电平持续时间，单位是micros
unsigned long pwmTimer = 0l;
// highHoldSumTime:高电平持续时间总和，每秒会清零一次，用于计算占空比
unsigned long highHoldSumTime = 0l;
// highHoldMaxTime,highHoldMinTime:单位时间内高电平持续时间最大值&最小值，用于测试是否稳定
unsigned long highHoldMaxTime = 0l;
unsigned long highHoldMinTime = 1000000l;
// highCount: 高电平计数器，用来计算频率
unsigned long highCount = 0l;

void setup() {
    Serial.begin(115200);
    pinMode(pwmPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pwmPin), changeCallback, CHANGE);
    // 好像并不支持多中断回调，只能在change中自己分
    // attachInterrupt(digitalPinToInterrupt(pwmPin), risingCallback, RISING);
    // attachInterrupt(digitalPinToInterrupt(pwmPin), fallingCallback, FALLING);

    loopTimer = millis();
}

void loop() {
    unsigned long now = millis();
    if ( now - loopTimer > 1000 ){
        // print here
        Serial.print("freq:");
        Serial.print(highCount);
        Serial.println("Hz");

        Serial.print("dutyCycel:");
        Serial.print((float)highHoldSumTime/10000.0f);
        Serial.println("%");

        Serial.print("max:");
        Serial.println(highHoldMaxTime);
        Serial.print("min:");
        Serial.println(highHoldMinTime);
        

        loopTimer = now;
        // init again
        highHoldSumTime = 0l;
        highHoldMaxTime = 0l;
        highHoldMinTime = 1000000l;
        highCount = 0l;
    }
}

void risingCallback() {
    // Serial.println("rising");
    pwmTimer = micros();
}

void fallingCallback() {
    // Serial.println("falling");
    unsigned long now = micros();
    unsigned long highHoldTime = now - pwmTimer;
    if ( highHoldTime > highHoldMaxTime ){
        highHoldMaxTime = highHoldTime;
    }
    if ( highHoldTime < highHoldMinTime ){
        highHoldMinTime = highHoldTime;
    }
    highHoldSumTime += highHoldMinTime;
    highCount ++;
}

void changeCallback() {
    // Serial.println("change");
    int d = digitalRead(pwmPin);
    if (d == HIGH){
        risingCallback();
    }
    if (d == LOW){
        fallingCallback();
    }
}


