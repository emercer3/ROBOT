/*
 * File:   breakwork.c
 * Author: emercer3
 *
 * Created on November 27, 2024, 8:46 AM
 */


#include "xc.h"

// Select oscillator
#pragma config FNOSC = LPRC
#pragma config SOSCSRC = DIG
// Global variables
int steps = 0;
int state = 1;
int N = 0;              // Desired steps
int threshold = 1551;

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
    _SMPI = 6;	  // AD1CON2<6:2> -- Every 4th conversion sent
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
    OC3RS = 0; // PWM period and duty cycle for pin 14
    OC3R = OC3RS * .5;

}

void config_pins(void) {
    // PWM FOR BALL DROP
    _TRISB1 = 0; // direction on pin 13
    _LATB1 = 1;
    OC3RS = 309; // PWM period and duty cycle for pin 14
    OC3R = 35; 
    // PWM right motor
    _TRISB9 = 0; // direction on pin 13
    _LATB9 = 1;      
    _TRISA6 = 0; // PWM on pin 14
    // PWM left motor
    _TRISA1 = 0; // directions on pin 3
    _LATA1 = 1;
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
    // IF SENSOR SETUP
    _ANSB12 = 1;
    _CSS12 = 1;
    
    // start button
    _TRISB2 = 1; // button on pin 6
    _ANSB2 = 0;

    // LED pin 11
    _TRISB7 = 0;
    _LATB7 = 0;
    
    // LASER pin 12
    _TRISB8 = 0;
    _LATB8 = 1;
    
    _OC1IE = 1;     // enable interrupt
    
}

void ballcollect(void) {
    steps = 0;
    N = 127; // old 128
    _LATB9 = 0; 
    _LATA1 = 1;
    while (steps <= N) {} // right
    steps = 0;
    _LATB9 = 1;
    _LATA1 = 1;
    while (steps < 230) {}
    steps = 0;
    OC1RS = 0; 
    OC1R = OC1RS * .5;
    OC2RS = 0; 
    OC2R = OC2RS * .5;
    int k = 0;
    while (k < 1900) {
        k++;
    }
    OC1RS = 80; 
    OC1R = OC1RS * .5;
    OC2RS = 80; 
    OC2R = OC2RS * .5;
    _LATB9 = 0;
    _LATA1 = 0;
    while (steps < 230) {}
    steps = 0;
    N = 127; // old 128
    _LATB9 = 1; 
    _LATA1 = 0;
    while (steps <= N) {} // turn left
    steps = 0;
    _LATB9 = 1;
    _LATA1 = 1;
    while(steps <= 200){}
}

void balldropoff(void) {
    if (ADC1BUF13 < 1551) { // read white
        steps = 0;
        N = 130;
        _LATB9 = 1;
        _LATA1 = 0;
        while (steps <= N) {} // turn right
        steps = 0;
        OC3R = 7;
        OC1RS = 0; // old 79 
        OC1R = OC1RS * .5;
        OC2RS = 0; 
        OC2R = OC2RS * .5;
        int k = 0;
        while (k < 1900) {
            k++;
        }
        OC1RS = 100; 
        OC1R = OC1RS * .5;
        OC2RS = 100; 
        OC2R = OC2RS * .5;
        steps = 0;
        N = 130;
        _LATB9 = 0;
        _LATA1 = 1;
        while (steps <= N) {} // turn left
        OC3R = 35;
        steps = 0;
        N = 200;
        _LATB9 = 1; // go straight
        _LATA1 = 1;
        while (steps <= N) {}
        return;
    } else {    // read black
        steps = 0;
        N = 130;
        _LATB9 = 0;
        _LATA1 = 1;
        while (steps <= N) {} // turn left
        steps = 0;
        OC3R = 7;
        OC1RS = 0; 
        OC1R = OC1RS * .5;
        OC2RS = 0; 
        OC2R = OC2RS * .5;
        int k = 0;
        while (k < 1700) {
            k++;
        }
        OC1RS = 100; 
        OC1R = OC1RS * .5;
        OC2RS = 100; 
        OC2R = OC2RS * .5;
        steps = 0;
        N = 130;
        _LATB9 = 1;
        _LATA1 = 0;
        while (steps <= N) {} // turn right
        OC3R = 35;
        N = 200;
        steps = 0;
        _LATB9 = 1; // go straight
        _LATA1 = 1;
        while (steps <= N) {}
        return;
    }
}

void canyon(void) {
    while(1) {
        if (ADC1BUF15 < 1200) { // front
           if (ADC1BUF4 < 1300) { // left
               steps = 0;
               N = 127; // old 128
               _LATB9 = 0; // this is right
               _LATA1 = 1;
               while (steps <= N) {} // turn left
               steps = 0;
               _LATB9 = 1;
               _LATA1 = 1;
               while (steps < 75) {}
           }
           else {
               steps = 0;
               N = 125 ; // old 128 
               _LATB9 = 1; // this is left 
               _LATA1 = 0;
               while (steps <= N) {} // turn right
               steps = 0;
               _LATB9 = 1;
               _LATA1 = 1;
               while (steps < 75) {}
           }
        }
           if ((ADC1BUF10 < threshold) || (ADC1BUF9 < threshold) || (ADC1BUF11 < threshold)) {
               steps = 0;
               while (steps < 30) {} // turn right
               if (ADC1BUF15 < 1200) { // something on left
                    steps = 0;
                    N = 125; // old 128
                    _LATB9 = 0; // this is right
                    _LATA1 = 1;
                    while (steps <= N) {} // turn right
                    steps = 0;
                    _LATB9 = 1;
                    _LATA1 = 1;
                    while (steps < 100) {}
                   return;
               } else {
                    steps = 0;
                    N = 127; // old 128 
                    _LATB9 = 1; // this is left
                    _LATA1 = 0;
                    while (steps <= N) {} // turn left
                    steps = 0;
                    _LATB9 = 1;
                    _LATA1 = 1;
                    while (steps < 100) {}
                    return;
               }
       } 
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
}

void linefollow(void) {    
    while (1) {
        if (ADC1BUF12 > 400) { // read IR sensor
            _LATB7 = 0;
            steps = 0;
            while (steps < 65) {}
            ballcollect();
        } else if (ADC1BUF4 < 1200 && ADC1BUF10 < 1551) { // read wall on left and on line
            _LATB7 = 0;
            balldropoff();
        } 
        _LATB7 = 1;
        if (ADC1BUF9 < threshold && ADC1BUF10 < threshold) { // right and middle
            OC1RS = 110; // right
            OC1R = OC1RS * .5;
            OC2RS = 90; // left
            OC2R = OC2RS * .5;
        } if (ADC1BUF10 < threshold && ADC1BUF11 < threshold) { // left and middle
            OC1RS = 90; // right
            OC1R = OC1RS * .5;
            OC2RS = 110; // left
            OC2R = OC2RS * .5;
        } 
        if (ADC1BUF11 < threshold) { // just right
            OC1RS = 100; // right
            OC1R = OC1RS * .5;
            OC2RS = 0; // left
            OC2R = OC2RS * .5;
        } if (ADC1BUF9 < threshold) { // just left
            OC1RS = 0; // right
            OC1R = OC1RS * .5;
            OC2RS = 100; // left
            OC2R = OC2RS * .5;
        } 
//        if (ADC1BUF10 < threshold) { // just middle
//            OC1RS = 100; // right
//            OC1R = OC1RS * .5;
//            OC2RS = 100; // left
//            OC2R = OC2RS * .5;
//        } 
        else { // just middle
            OC1RS = 100; // right
            OC1R = OC1RS * .5;
            OC2RS = 100; // left
            OC2R = OC2RS * .5;
        } 

//        else {
//            canyon();
//        }
    }
}


int main()
{
    // Configure A/D
    config_ad();
    config_pwm();
    config_pins();
        
    OC1RS = 80; // old 79 
    OC1R = OC1RS * .5;
    OC2RS = 80; 
    OC2R = OC2RS * .5;

    // Loop
    while(1) {
        int k = 0;
        while (k < 2500) { // waste time
            k++;
        }
        //undoc
        linefollow();
        //doc 
//        _LATB8 = 0;
        
    }
    return 0;
}
