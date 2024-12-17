#define FCY 16500UL
#include "xc.h"
#include <libpic30.h>

// Select oscillator
//#pragma config FNOSC = LPRC
#pragma config FNOSC = FRCDIV
#pragma config SOSCSRC = DIG
#pragma config OSCIOFNC = OFF
// Global variables
int steps = 0;
int state = 0;
int N = 0;              // Desired steps
int threshold = 1900; // 1800 old

// OC1 Interrupt Service Routine
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void) {      // step counter 
    // When the OC1 Interrupt is activated, the code will jump up here
    // each time your PIC generates a PWM step
    // Add code to increment the step count each time this function
    // is called
    _OC1IF = 0;
    steps++;
}

void config_ad(void) { 
    _ADON = 0;    // AD1CON1<15> -- Turn off A/D during config
    
    // AD1CON1 register
    _ADSIDL = 0;  // AD1CON1<13> -- A/D continues in idle mode
    _MODE12 = 1;  // AD1CON1<10> -- 12-bit A/D operation
    _FORM = 0;    // AD1CON1<9:8> -- Unsigned integer output
    _SSRC = 7;    // AD1CON1<7:4> -- Auto conversion (internal counter)
    _ASAM = 1;    // AD1CON1<2> -- Auto sampling

    // AD1CON2 register
    _PVCFG = 0;   // AD1CON2<15:14> -- Use VDD as positive
                  // ref voltage
    _NVCFG = 0;   // AD1CON2<13> -- Use VSS as negative
                  // ref voltage
    _BUFREGEN = 1;// AD1CON2<11> -- Result appears in buffer
                  // location corresponding to channel
    _CSCNA = 1;   // AD1CON2<10> -- Scans inputs specified
                  // in AD1CSSx registers
    _SMPI = 8;	  // AD1CON2<6:2> -- Every 4th conversion sent
                  // to buffer (if sampling 4 channels)
    _ALTS = 0;    // AD1CON2<0> -- Sample MUXA only

    // AD1CON3 register
    _ADRC = 0;    // AD1CON3<15> -- Use system clock
    _SAMC = 0;    // AD1CON3<12:8> -- Auto sample every A/D
                  // period TAD
    _ADCS = 0x3F; // AD1CON3<7:0> -- A/D period TAD = 64*TCY

    // AD1CSS registers
    // SET THE BITS CORRESPONDING TO CHANNELS THAT YOU WANT
    // TO SAMPLE

    _ADON = 1;    // AD1CON1<15> -- Turn on A/D
}

void config_pwm(void) {
    // configure output compare interrupt
    _OC1IP = 4;
    _OC1IE = 0;
    _OC1IF = 0; 
    
    // Configure PWM
    OC1CON1 = 0;                // Clear all bits of OC1CON1
    OC1CON2 = 0;                // Clear all bits of OC1CON2
    OC1CON1bits.OCTSEL = 0b111; // System clock as timing source
    OC1CON2bits.SYNCSEL = 0x1F; // Self-synchronization
    OC1CON2bits.OCTRIG = 0;     // Synchronization mode
    OC1CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC1RS = 0; // PWM period and duty cycle for pin 14
    OC1R = OC1RS * .5;
    
    // Configure PWM
    OC2CON1 = 0;                // Clear all bits of OC1CON1
    OC2CON2 = 0;                // Clear all bits of OC1CON2
    OC2CON1bits.OCTSEL = 0b111; // System clock as timing source
    OC2CON2bits.SYNCSEL = 0x1F; // Self-synchronization
    OC2CON2bits.OCTRIG = 0;     // Synchronization mode
    OC2CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC2RS = 0; // PWM period and duty cycle for pin 4
    OC2R = OC2RS * .5;
    
    // Configure PWM
   OC3CON1 = 0;                // Clear all bits of OC1CON1
   OC3CON2 = 0;                // Clear all bits of OC1CON2
   OC3CON1bits.OCTSEL = 0b111; // System clock as timing source
   OC3CON2bits.SYNCSEL = 0x1F; // Self-synchronization
   OC3CON2bits.OCTRIG = 0;     // Synchronization mode
   OC3CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
   OC3RS = 9999; // PWM period and duty cycle for pin 14
   OC3R = 1250;

}

void config_pins(void) {
    // PWM FOR BALL DROP
    _TRISB1 = 0; // direction on pin 13
    _LATB1 = 1;
//    OC3RS = 309; // PWM period and duty cycle for pin 14
//    OC3R = 35; 
    // PWM right motor
    _TRISB9 = 0; // direction on pin 13
    _LATB9 = 1;      
    _TRISA6 = 0; // PWM on pin 14
    // PWM left motor
    _TRISA1 = 0; // directions on pin 3
    _LATA1 = 0; // was 0
    _TRISB0 = 0; // PWM on pin 4
    // SERVO BALL DROPER


    // SETUP FOR AD RANGE SENSOR
    _ANSB4 = 1; // front
    _CSS15 = 1;
    _ANSB2 = 1; // side
    _CSS4 = 1;
    // BALL COLOR 
    _ANSA2 = 1;     // ball color pin 7
    _CSS13 = 1;
    // AD LINE SENSOR FOLLOWER
    _ANSB15 = 1;
    _CSS9 = 1;      // left
    _ANSB14 = 1;
    _CSS10 = 1;     // middle
    _ANSB13 = 1;
    _CSS11 = 1;     // right
    _ANSA3 = 1;
    _CSS14 = 1;     // far left
    // IR SENSOR SETUP
    _ANSB12 = 1;    // collector 
    _CSS12 = 1;
    _ANSA0 = 1;     // satellite
    _CSS0 = 1;

    // LED pin 11 and 10
    _TRISB7 = 0;
    _LATB7 = 0;
    _TRISA4 = 0;
    _LATA4 = 0;
    
    // LASER pin 12
    _TRISB8 = 0;
    _LATB8 = 0;
    
    _OC1IE = 1;     // enable interrupt
    
}

void ballcollect(void) {
    steps = 0;
    N = 250; // old 128
    _LATB9 = 0; 
    _LATA1 = 0;
    while (steps <= N) {} // right
    steps = 0;
    _LATB9 = 1;
    _LATA1 = 0;
    while (steps <= 460) {} //straight
    steps = 0;
    OC1RS = 0; 
    OC1R = OC1RS * .5;
    OC2RS = 0; 
    OC2R = OC2RS * .5;
    __delay_ms(18000); // 12000
    _LATB9 = 0;
    _LATA1 = 1;
    OC1RS = 1250; 
    OC1R = 625;
    OC2RS = 1250; 
    OC2R = 625;
    while (steps <= 400) {} // back up
    steps = 0;
    N = 255;
    _LATB9 = 1; 
    _LATA1 = 1;
    while (!(ADC1BUF11 < threshold)) {} // turn left  OLD CODE: steps <= N
    steps = 0;
    _LATB9 = 1;
    _LATA1 = 0;
    while(steps <= 175){} // was 200
}

void balldropoff(void) {
    OC1RS = 0; // 1350
    OC1R = 0;
    OC2RS = 0;  // 1350
    OC2R = 0;
    int k = 0;
    while (k < 8000) {
        k++;
    }
    if (ADC1BUF13 < 1551) { // read white
        _LATB9 = 1;
        _LATA1 = 1;
        OC1RS = 1500; // 1350
        OC1R = 750;
        OC2RS = 1500;  // 1350
        OC2R = 750;
        steps = 0;
        while (steps <= 250) {} // turn left
        OC3R = 250;
        OC1RS = 0; 
        OC1R = OC1RS * .5;
        OC2RS = 0; 
        OC2R = OC2RS * .5;
        __delay_ms(15000);
        _LATB9 = 0;
        _LATA1 = 0;
        OC1RS = 1500; 
        OC1R = 750;
        OC2RS = 1500; 
        OC2R = 750;
        steps = 0;
        while (steps <= 250) {} // turn right steps <= N was middle
        OC3R = 1250;
        _LATB9 = 1; // go straight
        _LATA1 = 0;
        steps = 0;
        while (steps <= 300) {}
        return;
    } else {    // read black
        _LATB9 = 0;
        _LATA1 = 0;
        OC1RS = 1500; // 1350
        OC1R = 750;
        OC2RS = 1500;  // 1350
        OC2R = 750;
        steps = 0;
        while (steps <= 250) {} // turn right
        OC3R = 250;
        OC1RS = 0; 
        OC1R = OC1RS * .5;
        OC2RS = 0; 
        OC2R = OC2RS * .5;
        __delay_ms(15000);
        _LATB9 = 1;
        _LATA1 = 1;
        OC1RS = 1500; // 1350
        OC1R = 750;
        OC2RS = 1500;  // 1350
        OC2R = 750;
        steps = 0;
        while (steps <= 250) {} // turn left  steps <= N
        OC3R = 1250;
        _LATB9 = 1; // go straight
        _LATA1 = 0;
        steps = 0;
        while (steps <= 190) {}
        return;
    }
}

void canyon(void) {
    while(1) {
        if (ADC1BUF15 < 1000) { // front
           if (ADC1BUF4 < 1300) { // left
               steps = 0;
               N = 250; // old 128
               _LATB9 = 0; 
               _LATA1 = 0;
               while (steps <= N) {} // turn right
               steps = 0;
               _LATB9 = 1;
               _LATA1 = 0;
               while (steps < 150) {}
           }
           else {
               steps = 0;
               N = 250; // old 128 
               _LATB9 = 1; // this is left 
               _LATA1 = 1;
               while (steps <= N) {} // turn right
               steps = 0;
               _LATB9 = 1;
               _LATA1 = 0;
               while (steps < 150) {}
           }
        }
           if ((ADC1BUF10 < threshold) || (ADC1BUF9 < threshold) || (ADC1BUF11 < threshold)) {
               steps = 0;
               while (steps < 160) {} // go straight
               if (ADC1BUF4 < 1300) { // something on left
                    steps = 0;
                    N = 250;
                    _LATB9 = 0;
                    _LATA1 = 0;
                    while (!(ADC1BUF10 < threshold)) {} // turn right  steps <= N
                    steps = 0;
                    _LATB9 = 1;
                    _LATA1 = 0;
//                    while (steps < 50) {}
                   return;
               } else {
                    steps = 0;
                    N = 250; 
                    _LATB9 = 1;
                    _LATA1 = 1;
                    while (!(ADC1BUF10 < threshold)) {} // turn left steps <= N
                    steps = 0;
                    _LATB9 = 1;
                    _LATA1 = 0;
//                    while (steps < 50) {}
                    return;
               }
       } 
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
}

void leavedoc(void) {
    while (1) {
        if (ADC1BUF14 < 3500) {
            __delay_ms(9000);
            steps = 0;
            N = 250;
            _LATB9 = 1;
            _LATA1 = 1;
            while (steps <= N) {} // turn left
            steps = 0;
            _LATB9 = 1;
            _LATA1 = 0;
            while (steps < 150) {} 
            return;
        }
    }
}

void laser(void) {
    int max = 0;
    int maxangle = 700; // 500

    while (OC3R <= 1250) {
        if (ADC1BUF0 > max) {
            max = ADC1BUF0;
            maxangle = OC3R;
        }
        OC3R = OC3R + 2;
        __delay_ms(200);
    }
    if (maxangle <= 700) {
        OC3R = 900;
        _LATA4 = 0;
    } else {
        OC3R = maxangle;
    }
    _LATB8 = 1;
    state = 0;
}

void enterdoc(void) {
    steps = 0;
    while (steps < 100) {}
    _LATB9 = 1;
    _LATA1 = 1;
    steps = 0;
    while (steps <= 240) {} // turn left
    _LATB9 = 1;
    _LATA1 = 0;
    steps = 0;
    while (steps < 200) {} //go forward
    while(1) {
        if (ADC1BUF15 < 1200){ // front
            OC1RS = 0; // right
            OC1R = 0;
            OC2RS = 0; // left
            OC2R = 0;
            int k = 0;
            while (k < 10000) {
                k++;
            }
            _LATB9 = 1;
            _LATA1 = 1;
            OC1RS = 1500; 
            OC1R = 750;
            OC2RS = 1500; 
            OC2R = 750;
            while (!(ADC1BUF10 < threshold)) {} // turn left
            _LATB9 = 0;
            _LATA1 = 1;
            steps = 0;
            while (steps < 400) {} //go backward
            OC1RS = 0; // right
            OC1R = 0;
            OC2RS = 0; // left
            OC2R = 0;
            OC3R = 500;
            laser();
            break;
        }
        else if (ADC1BUF11 < threshold && ADC1BUF10 < threshold) { // right and middle
            OC1RS = 1600; // right
            OC1R = 800;
            OC2RS = 1100; // left
            OC2R = 550;
        } else if (ADC1BUF10 < threshold && ADC1BUF9 < threshold) { // left and middle
            OC1RS = 1100;   // right
            OC1R = 550;
            OC2RS = 1600;   // left
            OC2R = 800;
        } else if (ADC1BUF11 < threshold) { // just left
            OC1RS = 850;   // right
            OC1R = 425;
            OC2RS = 2400;   // left
            OC2R = 1200;
        } else if (ADC1BUF9 < threshold) {  // just right
            OC1RS = 2400; // right
            OC1R = 1200;
            OC2RS = 850; // left
            OC2R = 425;
        } else if (ADC1BUF10 < threshold) { // just middle
            OC1RS = 1350; 
            OC1R = 675;
            OC2RS = 1350; 
            OC2R = 675;
        }
    }
}

void linefollow(void) {   
    while (1) {
       if (ADC1BUF12 > 1000) { // read IR sensor
            _LATB7 = 0;
            _LATA4 = 1;
            steps = 0;
            while (steps < 50) {}
            ballcollect();
            _LATA4 = 0;
            state = state + 1;
       } 
       else if ((ADC1BUF4 < 700) && (ADC1BUF10 < threshold) && (state >= 2)) { // read wall on left and on line
            _LATB7 = 1;
            balldropoff();
            state = state + 1;
            _LATB7 = 0;
       } 
       if (ADC1BUF14 < 3800) { // far left  && state == 4
           if (ADC1BUF11 < threshold || ADC1BUF10 < threshold || ADC1BUF9 < threshold) {
                _LATB7 = 1;
                _LATA4 = 1;
                enterdoc();
                break;
           }
       } 

        if (ADC1BUF11 < threshold && ADC1BUF10 < threshold) { // right and middle
            OC1RS = 1600; // right
            OC1R = 800;
            OC2RS = 1100; // left
            OC2R = 550;
        } else if (ADC1BUF10 < threshold && ADC1BUF9 < threshold) { // left and middle
            OC1RS = 1100;   // right
            OC1R = 550;
            OC2RS = 1600;   // left
            OC2R = 800;
        } else if (ADC1BUF11 < threshold) { // just left
            OC1RS = 800;   // right
            OC1R = 400;
            OC2RS = 2450;   // left
            OC2R = 1225;
        } else if (ADC1BUF9 < threshold) {  // just right
            OC1RS = 2450; // right
            OC1R = 1225;
            OC2RS = 800; // left // 850
            OC2R = 400;
        } else if (ADC1BUF10 < threshold) { // just middle
            OC1RS = 900; // 1350
            OC1R = 450;
            OC2RS = 900;  // 1350
            OC2R = 450;
        }  

        else if (ADC1BUF15 < 1200) { // something in front
            OC1RS = 1000; 
            OC1R = 500;
            OC2RS = 1000; 
            OC2R = 500;
            canyon();
            state = state + 1;
        } 
    }
}

int main() {
    // Configure crap
    _RCDIV = 0b011;
    config_ad();
    config_pwm();
    config_pins();
        
    state = 0;
    steps = 0;
    OC3R = 1200;
    while(1) {
        __delay_ms(10000)

        if (ADC1BUF12 > 1000) {
            _LATB7 = 1;
            state = 1;
            __delay_ms(8000);
            _LATB7 = 0;
        }

        // TEST SERVOR
//        if (state == 1) {
//            int max = 0;
//            int maxangle = 250;
//    
//            while (OC3R <= 1250) {
//                if (ADC1BUF0 > max) {
//                    max = ADC1BUF0;
//                    maxangle = OC3R;
//                }
//                OC3R = OC3R + 2;
//                __delay_ms(150);
//            }
//            OC3R = maxangle;
//            _LATB8 = 1;
//            state = 0;
//        }


        // NORMAL STATE CRAP
        if (state == 1) {
            OC1RS = 1350; 
            OC1R = 675;
            OC2RS = 1350; 
            OC2R = 675;
            leavedoc();
            linefollow();
            state = 0;
        } else {
            OC1RS = 0; 
            OC1R = 0;
            OC2RS = 0; 
            OC2R = 0;
        }

    } return 0;
}


