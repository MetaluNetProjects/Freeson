/*********************************************************************
 *               analog example for Versa1.0
 *	Analog capture on connectors K1, K2, K3 and K5. 
 *********************************************************************/

#define BOARD Versa1

#include <fruit.h>


t_delay mainDelay, pulseDelay;

unsigned char keyphase = 0, keycols[4], oldkeycols[4];

void keysService() 
{
	pinModeDigitalIn(KEYC1);
	pinModeDigitalIn(KEYC2);
	pinModeDigitalIn(KEYC3);
	pinModeDigitalIn(KEYC4);
	digitalClear(KEYC1);
	digitalClear(KEYC2);
	digitalClear(KEYC3);
	digitalClear(KEYC4);
	
	if(keyphase == 0) pinModeDigitalOut(KEYC1);
	else if(keyphase == 1) pinModeDigitalOut(KEYC2);
	else if(keyphase == 2) pinModeDigitalOut(KEYC3);
	else if(keyphase == 3) pinModeDigitalOut(KEYC4);
	
	digitalClear(KEYC1);
	digitalClear(KEYC2);
	digitalClear(KEYC3);
	digitalClear(KEYC4);
	Nop();
	Nop();
	
	keycols[keyphase] = (digitalRead(KEYL1)==0) + 2*(digitalRead(KEYL2)==0) + 4*(digitalRead(KEYL3)==0) + 8*(digitalRead(KEYL4)==0);
	keyphase++;
	if(keyphase == 4) keyphase = 0;
}

void sendKeys()
{
	if((keycols[0] != oldkeycols[0])
	|| (keycols[1] != oldkeycols[1])
	|| (keycols[2] != oldkeycols[2])
	|| (keycols[3] != oldkeycols[3]))
	{
		putchar('B');
		putchar('A');
		putchar('0' + keycols[0]);
		putchar('0' + keycols[1]);
		putchar('0' + keycols[2]);
		putchar('0' + keycols[3]);
		putchar('\n');
		
		oldkeycols[0] = keycols[0];
		oldkeycols[1] = keycols[1];
		oldkeycols[2] = keycols[2];
		oldkeycols[3] = keycols[3];
	}
}

/*void motOn()
{
	pinModeDigitalIn(MOT1);
	pinModeDigitalIn(MOT2);
	pinModeDigitalIn(MOT3);
	pinModeDigitalIn(MOT4);
	digitalSet(MOT1);
	digitalSet(MOT2);
	digitalSet(MOT3);
	digitalSet(MOT4);
	pinModeDigitalOut(MOT1);
	pinModeDigitalOut(MOT2);
	pinModeDigitalOut(MOT3);
	pinModeDigitalOut(MOT4);
}	

void motOff()
{
	pinModeDigitalIn(MOT1);
	pinModeDigitalIn(MOT2);
	pinModeDigitalIn(MOT3);
	pinModeDigitalIn(MOT4);
	digitalClear(MOT1);
	digitalClear(MOT2);
	digitalClear(MOT3);
	digitalClear(MOT4);
	pinModeDigitalOut(MOT1);
	pinModeDigitalOut(MOT2);
	pinModeDigitalOut(MOT3);
	pinModeDigitalOut(MOT4);
}*/

void mot1On()
{
	pinModeDigitalIn(MOT1);
	pinModeDigitalIn(MOT2);
	digitalSet(MOT1);
	digitalSet(MOT2);
	pinModeDigitalOut(MOT1);
	pinModeDigitalOut(MOT2);
}	

void mot1Off()
{
	pinModeDigitalIn(MOT1);
	pinModeDigitalIn(MOT2);
	digitalClear(MOT1);
	digitalClear(MOT2);
	pinModeDigitalOut(MOT1);
	pinModeDigitalOut(MOT2);
}	

void mot2On()
{
	pinModeDigitalIn(MOT3);
	pinModeDigitalIn(MOT4);
	digitalSet(MOT3);
	digitalSet(MOT4);
	pinModeDigitalOut(MOT3);
	pinModeDigitalOut(MOT4);
}	

void mot2Off()
{
	pinModeDigitalIn(MOT3);
	pinModeDigitalIn(MOT4);
	digitalClear(MOT3);
	digitalClear(MOT4);
	pinModeDigitalOut(MOT3);
	pinModeDigitalOut(MOT4);
}	
//----------- Setup ----------------
	
void setup(void) {	
	fruitInit();
//	pinModeDigitalOut(LED); 	// set the LED pin mode to digital out
//	digitalClear(LED);		// clear the LED
	delayStart(mainDelay, 5000); 	// init the mainDelay to 5 ms
	
	digitalClear(MOT1);
	digitalClear(MOT2);
	pinModeDigitalOut(MOT1);
	pinModeDigitalOut(MOT2);
	
	mot1Off();
	mot2Off();

	pinModeDigitalIn(KEYL1);
	pinModeDigitalIn(KEYL2);
	pinModeDigitalIn(KEYL3);
	pinModeDigitalIn(KEYL4);
	
	keycols[0] = oldkeycols[0] = 0;
	keycols[1] = oldkeycols[1] = 0;
	keycols[2] = oldkeycols[2] = 0;
	keycols[3] = oldkeycols[3] = 0;
}

// ---------- Main loop ------------

void loop() {
	fraiseService();	// listen to Fraise events
	keysService();
  
	if(delayFinished(mainDelay)) // when mainDelay triggers :
	{
		delayStart(mainDelay, 5000); 	// re-init mainDelay
		sendKeys();
	}
}


// ---------- Receiving ------------

void fraiseReceiveChar() // receive text
{
	unsigned char c;
	
	c=fraiseGetChar();
	if(c=='L'){		//switch LED on/off 
		c=fraiseGetChar();
		//digitalWrite(LED, c!='0');		
	}
	else if(c=='E') { 	// echo text (send it back to host)
		printf("C");
		c = fraiseGetLen(); 			// get length of current packet
		while(c--) printf("%c",fraiseGetChar());// send each received byte
		putchar('\n');				// end of line
	}	
}

void fraiseReceive() // receive raw
{
	unsigned char c,c2;
	//unsigned int i;
	c=fraiseGetChar();
	
	if(c == 10) {
	    c2 = fraiseGetChar();
	    if(c2) mot1On(); 
	    else mot1Off();
	}
	if(c == 11) {
	    c2 = fraiseGetChar();
	    if(c2) mot2On(); 
	    else mot2Off();
	}
}

