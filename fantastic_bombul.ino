#include <LiquidCrystal.h>

//LED Setup
LiquidCrystal LCD(2, 3, 4, 5, 6, 7);
int ss, mm, hh;
int alMM, alHH;
int isAP;
int isAlarm = 0;
int ring = 0;

//Piezo
int buzz = 13;

//buttons
int timeB = 11;
int alarmB = 10;
int tmpB = 9;
int snoozeB = 8;

//Temperature Setup
int tmpPin = A0;
int tmpInput;
double temperature;

int isCelcius = 1;

byte alarmOff[] = {
  B00100,
  B01010,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B00100
};

byte alarmOn[] = {
  B00100,
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00100
};

unsigned long previousMillis = 0;
unsigned long currentMillis = millis();

//Time Setup
int isAlarmSetup = 0;

int isTimeSetup = 0;
int isMinSetup = 1;
int isHourSetup = 0;
int blink = 0;
int firstRelease = 0;

void setup() {
	LCD.begin(16, 2);
  	Serial.begin(9600);
  
  	LCD.createChar(1, alarmOff);
  	LCD.createChar(2, alarmOn);
 
  	ss = 45;
  	mm = 54; 
  	hh = 14;
  	isAP = 0; 
  
  	alHH = 14;
  	alMM = 55;
  
  	//Timer Setup
  	noInterrupts();
  	
  	TCCR1A = 0;
  	TCCR1B = 0;
  	TCNT1 = 0;
  
  	OCR1A = 62500;
  
  	TCCR1B |= (1 << WGM12);
  	TCCR1B |= (1 << CS12);
  	TIMSK1 |= (1 << OCIE1A);
  
  	interrupts();
}

ISR(TIMER1_COMPA_vect) {
	ss = ss + 1;
}

void loop() {
  	currentMillis = millis();
  	
    LCD.home();
  
	if (!isTimeSetup) {
       showTime();
    }
  
  	if (!isAlarmSetup) {
      showAlarm();
    }
    calculateTemperature();
  
 	buttonControl();
  	alarmControl();
 
}

void showAlarm() {
  	if (alMM == 60) {
  		alMM = 0;
      	alHH++;
    }
  
  	if (alHH == 24) {
     	alHH = 0; 
    }
  
	LCD.setCursor(10, 1);
  	printTime(alHH);
  	line();
  	printTime(alMM);
	if (isAlarm) {
  		LCD.write(2);
    }
	else {
      	LCD.write(1);
    }
}

void clearAlarm() {
  	if (isMinSetup) {
     	LCD.setCursor(13, 1); 
    }
  	else if (isHourSetup) {
     	LCD.setCursor(10, 1); 
    }
  
  	blank(); blank();
}

void alarmControl() {
  	if (hh == alHH && mm == alMM && isAlarm) {
     	ring = 1; 
    }
  
  	if (ring) {
     	tone(buzz, 350); 
    }
	else {
  		noTone(buzz);
    }
}

int lastTimeB = 0;	
int lastTmpB = 0;
int lastAlarmB = 0;
int lastSnoozeB = 0;

int timeFlag = 0;
int alarmFlag = 0;
int start = 0;

void buttonControl() {
  	
  	//
  	//
  	//time button
  	if (digitalRead(timeB) == 1 && !isAlarmSetup) {
      	lastTimeB = 1;
      
      	//hold
      	if (timeFlag == 0) {
      		timeFlag = 1;
          	start = ss;
        }
      	if (((ss + 60) - start) % 60 == 3) {
          	isTimeSetup = 1;
        }
    }
	else if (digitalRead(timeB) == 0) {
    	timeFlag = 0;
    }
  
	if (isTimeSetup) {
	    if (currentMillis - previousMillis >= 300) {	
			previousMillis = currentMillis;
          
	        if (blink) {
              showTime();
              blink = 0;
            }
	        else {
              clearTime();
              blink = 1;
            }
        }
    }
  
  	//press
  	if (digitalRead(timeB) == 0 && lastTimeB == 1) {
      	if (isTimeSetup) {
          	if (!firstRelease) {
              	firstRelease = 1;
            }
          	else {
              	if (isMinSetup) {
                  isMinSetup = 0;
                  isHourSetup = 1;
                }
                else if (isHourSetup) {
                  isHourSetup = 0;
                  isTimeSetup = 0;
                  firstRelease = 0;
                  isMinSetup = 1;
                }
            }
        }
	    else {
          if (isAP == 0) {
            isAP = 1;
          }
          else {
            isAP = 0;
          }	
        }
      	lastTimeB = 0;
    }
  	
  	//
  	//
  	//alarm button
 	if (digitalRead(alarmB) == 1 && !isTimeSetup) {
		lastAlarmB = 1;
      	
      	//hold
      	if (alarmFlag == 0) {
      		alarmFlag = 1;
          	start = ss;
        }
      	if (((ss + 60) - start) % 60 == 3) {
          	isAlarmSetup = 1;	
        }
    }
  	else if (digitalRead(timeB) == 0) {
    	alarmFlag = 0;
    }
  
  	if (isAlarmSetup) {
        if (currentMillis - previousMillis >= 300) {	
          previousMillis = currentMillis;

          if (blink) {
            showAlarm();
            blink = 0;
          }
          else {
            clearAlarm();
            blink = 1;
          }
        }
      }
  
		//press
	if (digitalRead(alarmB) == 0 && lastAlarmB == 1) {
      	if (isAlarmSetup) {	
          	if (!firstRelease) {
              	firstRelease = 1;
            }
          	else {
              	if (isMinSetup) {
                  isMinSetup = 0;
                  isHourSetup = 1;
                }
                else if (isHourSetup) {
                  isHourSetup = 0;
                  isAlarmSetup = 0;
                  firstRelease = 0;
                  isMinSetup = 1;
                }
            }
        }
      	else if (isAlarm) {
			isAlarm = 0;
          	ring = 0;
        }
      	else {
        	isAlarm = 1;
        }
  	
      	lastAlarmB = 0;
    } 
      
  	//	
  	//
  	//temperature button
	if (digitalRead(tmpB) == 1) {
		lastTmpB = 1;
    }

	if (digitalRead(tmpB) == 0 && lastTmpB == 1) {
      	if (isTimeSetup) {
          	if (isMinSetup) {
            	mm++;
            }
          	else if (isHourSetup) {
             	hh++; 
            }
        }
      	else if (isAlarmSetup) {
          	if (isMinSetup) {
            	alMM++;
            }
          	else if (isHourSetup) {
             	alHH++; 
            }
        }
      	else {
          	LCD.setCursor(0, 1);
          
          	for (int i = 0; i < 7; i++) {
             	blank(); 
            }
          
      		if (isCelcius == 0) {
              isCelcius = 1;
            }
          	else {
            	isCelcius = 0;
          	}	
        }
     	
      	lastTmpB = 0;
    }
  
  	//
  	//
  	//snooze button
  	if (digitalRead(snoozeB) == 1) {
		lastSnoozeB = 1;
    }

	if (digitalRead(snoozeB) == 0 && lastSnoozeB == 1) {
      	
      	if (ring) {
          	if (alMM + 5 >= 60) {
             	alMM = (alMM + 5) % 60;
              	alHH++;
            }
          	else {
              	alMM += 5;
            }
          	ring = 0;
        }
  	
      	lastSnoozeB = 0;
    } 
}

void showTime() {
  	if (ss == 60) {
    	mm++;
      	ss = 0;
    }

	if (mm == 60) {
        hh++;
        mm = 0;
    }
  
  	if (hh == 24) {
      hh = 0;
    }
  
  	LCD.setCursor(0, 0);
  
  	if (isAP && hh > 12) {
		printTime(hh%12);
    }
  	else {
     	printTime(hh);
    }
  
  
  	line();
    printTime(mm);
    line(); 
  	printTime(ss);
  
  	if (isAP) {
      	if (hh > 12) {
        	LCD.print("PM");
        }
      	else {
      		LCD.print("AM");
      	}
    }
  	else {
     	blank(); blank();
  	} 
  	
  	
}

void printTime(int x) {
  	if (x <= 9) {
    	LCD.print("0");
      	LCD.print(x);
    }
  	else {
    	LCD.print(x);
    }
}

void line() {
	LCD.print(":");
}

void blank() {
	LCD.print(" ");
}

void clearTime() {
  	if (isMinSetup) {
    	LCD.setCursor(3, 0);
    }
  	else if (isHourSetup) {
     	LCD.setCursor(0, 0); 
    }
  
  	blank(); blank();
}

int fahrenheit;

void calculateTemperature() {
	tmpInput = analogRead(A0);
  	temperature = (double)tmpInput / 1024;
  	temperature *= 5;
  	temperature -= 0.5;
  	temperature *= 100; //Celcius
  	fahrenheit = temperature * 1.8 + 32.0;
  
  	LCD.setCursor(0, 1);	
  	
  	if (isCelcius) {
      	LCD.print(temperature);
    	LCD.print("C");
      	
      	if (temperature < 100 && temperature > 0 || temperature < 0 && temperature > -10) {
     		LCD.setCursor(6, 1);
      		blank();
    	}
      
     	if (temperature < 10 && temperature > 0) {
			LCD.setCursor(5, 1);
          	blank();
        }
        
    }
  	else {
      	LCD.print(fahrenheit);
    	LCD.print("F");
      
      	if (fahrenheit < 100 && fahrenheit > 0 || fahrenheit < 0 && fahrenheit > -10) {
     		LCD.setCursor(3, 1);
      		blank();
    	}
      	if (fahrenheit < 10 && fahrenheit > 0) {
			LCD.setCursor(2, 1);
          	blank();
        }
     	
    }
  
  	
}










