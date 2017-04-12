/*********************************************************************
 *               Classic phone matrix keyboard decoding.
 *               Microsecond accurate pulse generator.
 *	             3 channels analog capture.
 *				 2 channel power half-bridge
 *********************************************************************/

#define BOARD 8X2A

#include <fruit.h>
#include <analog.h>
#include <switch.h>
//#include <dcmotor.h>

// Timer macros
#define TIMER 3
#include <timer.h>

//  val = /*0xFFFF - */pulseWidth;

#define SET_TIMER(us) do {\
	val = 0xFFFF - us;\
	TIMER_H = val>>8;\
	TIMER_L = val&255;\
	TIMER_IF = 0;\
	TIMER_ON = 1; } while(0)

#define TIMER_INIT() do{\
	TIMER_CON = 0; \
	TIMER_PS1 = 0;  /* 	no prescaler */\
	TIMER_16BIT = 1;/* 	16bits                          */\
	TIMER_IP = 1;	/* 	high/low priority interrupt */\
	TIMER_ON = 0;	/* 	stop timer                      */\
	TIMER_IF = 0;   /*  clear flag                      */\
	TIMER_IE = 1;	/* 	enable timer interrupt         */\
} while(0)


t_delay mainDelay, pulseDelay;
char pulsePhase = 0;
char pulseChan;
unsigned int pulseWidth = 1000;
unsigned int val;
unsigned int pWidth[4];
/*DCMOTOR_DECLARE(A);
DCMOTOR_DECLARE(B);
DCMOTOR_DECLARE(C);
DCMOTOR_DECLARE(D);*/

//-------------------------------------------
// pulse out: 

#define PHASE0(chan) do{ digitalClear(M##chan##1); digitalClear(M##chan##2); } while(0)
#define PHASE1(chan) do{ digitalSet(M##chan##1); digitalClear(M##chan##2); } while(0)
#define PHASE2(chan) do{ digitalClear(M##chan##1); digitalSet(M##chan##2); } while(0)
#define PHASE_(chan, phase) PHASE##phase(chan)

#define PHASE(numchan, phase) do { \
	if(numchan == 0) PHASE_(C,phase); \
	else if(numchan == 1) PHASE_(B,phase); \
	else if(numchan == 2) PHASE_(C,phase); \
	else if(numchan == 3) PHASE_(D,phase);} while(0)
	
void pulseFire()
{
	//delayStart(pulseDelay, pulseWidth);
	//pulseChan = chan;
	pulsePhase = 2;
	/*digitalSet(MC1);
	digitalClear(MC2);*/
	PHASE(pulseChan, 1);
	SET_TIMER(pulseWidth);
}

void pulseService()
{
  	static int chan = 0;
	/*if(pulsePhase == 1) {
		if(!delayFinished(pulseDelay)) return;
		digitalClear(MC1);
		digitalSet(MC2);
		delayStart(pulseDelay, pulseWidth);
		pulsePhase = 2;
	} else if(pulsePhase == 2) {
		if(!delayFinished(pulseDelay)) return;
		digitalClear(MC1);
		digitalClear(MC2);
		pulsePhase = 0;
	}*/
  	if(pulsePhase == 0) { // if last pulse finished
		if(pWidth[chan]) {
			pulseWidth = pWidth[chan];
			pulseChan = chan;
			pulseFire();
			pWidth[chan] = 0;
			chan++;
		}
	}	
}

// ----------- Interrupts --------------
 
void highInterrupts(void)
{
    //static unsigned int val;
    
    if(TIMER_IF == 0) return;
    
    TIMER_ON = 0;
    TIMER_IF = 0;

    if(pulsePhase == 1) {
		/*digitalClear(MC1);
		digitalClear(MC2);*/
		PHASE(pulseChan, 0);
		SET_TIMER(pulseWidth);
		pulsePhase = 2;
		return;
    } else if(pulsePhase == 2) {
		/*digitalSet(MC2);
		digitalClear(MC1);*/
		PHASE(pulseChan, 2);
		SET_TIMER(pulseWidth);
		pulsePhase = 3;
		return;
    } else if(pulsePhase == 3) {
		/*digitalClear(MC1);
		digitalClear(MC2);*/
		PHASE(pulseChan, 0);
		pulsePhase = 0;
		return;
    }
}

//---- phone keyboard :--------------
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



//----------- Setup ----------------
#define SETUP_BRIDGE(X) do {\
    digitalClear(M##X##1);\
    digitalClear(M##X##2);\
    digitalSet(M##X##EN);\
    pinModeDigitalOut(M##X##1);\
    pinModeDigitalOut(M##X##2);\
    pinModeDigitalOut(M##X##EN); } while(0)
	
void setup(void) {	
	fruitInit();
	delayStart(mainDelay, 5000); 	// init the mainDelay to 5 ms

// Analog setup ----------------
	analogInit();		// init analog module
	analogSelect(0, POT1);	
	analogSelect(1, POT2);	
	analogSelect(2, POT3);	

// Switch setup ----------------
	switchInit();		// init switch module
	switchSelect(0, SWITCH1);	

// pulse setup ----------------

	//SETUP_BRIDGE(A);
	//SETUP_BRIDGE(B);
	SETUP_BRIDGE(C);
	SETUP_BRIDGE(D);

	pulsePhase = 0;    
	TIMER_INIT();		

// keys setup ----------------
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

unsigned char turn = 0;

void loop() {
	fraiseService();	// listen to Fraise events
	analogService();	// analog management routine
	pulseService();
	fraiseService();	// listen to Fraise events
	keysService();
	fraiseService();	// listen to Fraise events
	switchService();
	
	if(delayFinished(mainDelay)) // when mainDelay triggers :
	{
		delayStart(mainDelay, 5000); 	// re-init mainDelay
		if(turn%2) sendKeys();
		else if(!switchSend()) analogSend();
		turn++;
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

#define PINSET(K) do{ pinModeDigitalOut(K); if(c2==0) digitalClear(K); else digitalSet(K); } while(0)

void fraiseReceive() // receive raw
{
	unsigned char c,c2;
	unsigned int i;
	c=fraiseGetChar();
	
	switch(c) {
	    PARAM_INT(10, i); c2 = fraiseGetChar() ; /*pWidth[c2] = i; */ pulseWidth = i; pulseChan = c2; pulseFire(); break;

	    PARAM_CHAR(30, c2); PINSET(MD1); break;
	    PARAM_CHAR(31, c2); PINSET(MD2); break;
	}
}

