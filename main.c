#include "msp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * main.c
 */

// Note Statics
// note frequencies as variables  (https://www.doctormix.com/blog/note-to-frequency-chart)
    double C3 = 130.813;
    double Cs3 = 138.591;   //C#3
    double Db3 = 138.591;   //Db3
    double D3 = 146.832;
    double Ds3 = 155.563;   //D#3
    double Eb3 = 155.563;   //Eb3
    double E3 = 164.814;
    double F3 = 174.614;
    double Fs3 = 184.997;   //F#3
    double Gb3 = 184.997;   //Gb3
    double G3 = 195.998;
    double Gs3 = 207.652;   //G#3
    double Ab3 = 207.652;   //Ab3
    double A3 = 220.000;
    double As3 = 233.082;   //A#3
    double Bb3 = 233.082;   //Bb3
    double B3 = 246.942;

    double C4 = 261.626;
    double Cs4 = 277.183;   //C#4
    double Db4 = 277.183;   //Db4
    double D4 = 293.665;
    double Ds4 = 311.127;   //D#4
    double Eb4 = 311.127;   //Eb4
    double E4 = 329.628;
    double F4 = 349.228;
    double Fs4 = 369.994;   //F#4
    double Gb4 = 369.994;   //Gb4
    double G4 = 391.995;
    double Gs4 = 415.305;   //G#4
    double Ab4 = 415.305;   //Ab4
    double A4 = 440.000;
    double As4 = 466.164;   //A#4
    double Bb4 = 466.164;   //Bb4
    double B4 = 493.883;

    double C5 = 523.251;
    double Cs5 = 554.365;
    double Db5 = 554.365;
    double D5 = 587.330;
    double Ds5 = 622.254;
    double Eb5 = 622.254;
    double E5 = 659.255;
    double F5 = 698.456;
    double Fs5 = 739.989;
    double Gb5 = 739.989;
    double G5 = 783.991;
    double Gs5 = 830.609;
    double Ab5 = 830.609;
    double A5 = 880.000;
    double As5 = 932.328;
    double Bb5 = 932.328;
    double B5 = 987.767;

    double C6 = 1046.50;
    double Cs6 = 1108.73;
    double Db6 = 1108.73;
    double D6 = 1174.66;
    double Ds6 = 1244.51;
    double Eb6 = 1244.51;
    double E6 = 1318.51;
    double F6 = 1396.91;
    double Fs6 = 1479.98;
    double Gb6 = 1379.98;
    double G6 = 1567.98;
    double Gs6 = 1661.22;
    double Ab6 = 1661.22;
    double A6 = 1760.00;
    double As6 = 1864.66;
    double Bb6 = 1864.66;
    double B6 = 1975.53;

// Specify Time-based Variables
    double sixteenth = 0.0625;
    double eighth = 0.125;
    double dottedEighth = 0.1875;
    double quarter = 0.25;
    double dottedQuarter = 0.375;
    double half = 0.5;
    double dottedHalf = 0.75;
    double whole = 1.00;

    unsigned int tempo = 90;

    double period = 0.0;
    double halfperiod = 0.0;
    double noteCycles = 0;
    int loopbreak = 0;

// Button Inputs
    int firstRow_input = 0;
    int secondRow_input = 0;
    int thirdRow_input = 0;
    int fourthRow_input = 0;
    int modeSelection_input = 0;
    unsigned char mode = 'u';

// Functions List
void init(void);
void Cycle(double);
void Note(double, double);
void Rest(double);
void modeSelection(void);

// Music Function List
// 1.
void chirp(void);

// 2.
void LostAlarm(void);

// 3.
void dropOffAlarm(void);

// Main Function
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    init();

    while(1){
        P1->OUT ^= BIT0;    // Toggle LED
        if (mode == '1'){
            chirp();
            //mode = 'u';
        }
        if (mode == '2'){
            P3->OUT |= BIT6;    // Turn On LED
            LostAlarm();
            //mode = 'u';
        }
        if (mode == '3'){
            dropOffAlarm();
            //mode = 'u';
        }

        if (mode != '2'){
            P3->OUT &= ~BIT6;   // Turn Off LED
        }
        __delay_cycles(5000);
    }


}

void init(){
    // UART Set Up
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; //eUSCI reset state
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | EUSCI_B_CTLW0_SSEL__SMCLK;  //eUSCI clock course, SMCLK
    EUSCI_A0->BRW = 19; //9600 bps setting
    EUSCI_A0->MCTLW = (9<<EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;  //9600 bps setting

    P1->SEL1 &= ~(BIT2|BIT3);   //UART function for P1.2 & P1.3
    P1->SEL0 |= BIT2|BIT3;  //UART function for P1.2 & P1.3

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;    //eUSCI operation state

    // Interrupt Setup
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE; //Set UCA0RXIE
    NVIC->ISER[0] = 1<<((EUSCIA0_IRQn) & 31);

    __delay_cycles(1000);   //delay
    __enable_irq(); //enable global interrupt

    // Port Setup
    // Port 1
    P1->DIR |= BIT0;    // Red LED Direction
    P1->OUT &= ~BIT0;

    // Port 3
    P3->DIR |= 0xE1;    // Set P3.0, P3.5, P3.6, and P3.7 as output pins
    P3->OUT &= ~0xE1;   // Clear output

    // Clock setup
    TIMER_A0->CCR[0] = 5000;
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1 | TIMER_A_CTL_CLR;  // TA0CTL setup (SMCLK, Up Mode, Clear)
}

void Cycle(double halfperiod){
    // Turn on Buzzer
    P3->OUT |= BIT0;    // Turn on P3.0

    // Wait
    while (loopbreak == 0){
        if((TIMER_A0->CTL & TIMER_A_CTL_IFG) != 0){
            //P3->OUT ^= BIT0;    // Toggle P3.0
            TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;  // Clear TAIFG flag
            loopbreak = 1;
        }
    }
    loopbreak = 0;

    // Turn Off Buzzer
    P3->OUT ^= BIT0;    // Toggle P3.0

    // Wait
    while (loopbreak == 0){
        if((TIMER_A0->CTL & TIMER_A_CTL_IFG) != 0){
            //P3->OUT ^= BIT0;    // Toggle P3.0
            TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;  // Clear TAIFG flag
            loopbreak = 1;
        }
    }
    loopbreak = 1;
}


void Note(double freq, double length){
    length *= 8/(tempo/60);

    // Calculate number of cycles
    period = 1/freq;
    halfperiod = period/2;

    // Convert half-period to clock cycles
    halfperiod = halfperiod * 3000000;  // half-period * SMCLK frequency
    noteCycles = round(length * freq);     // The amount of times the note frequency will be cycled through (note length * note frequency)

    // Set value that counter counts to
    TIMER_A0->CCR[0] = round(halfperiod);   // set value timer will count up to the rounded version of the half-period

    // Loops set number of cycles to play the note
    int i = 0;
    for (i = 0; i < noteCycles; i++){
        Cycle(halfperiod);
    }

}


void Rest(double length){
    int restLength = round(3000 * length * 4/(tempo/60));
    int t = 0;
    for (t=0; t < restLength; t++){
        __delay_cycles(1000);
    }
}



void modeSelection(){
    // Clear P2->OUT
    P2->OUT &= 0x00;

    // Mode Selection from UART
    modeSelection_input = EUSCI_A0->RXBUF;   // mode selection

    // Decide Output (Row 1)
    if (modeSelection_input == 'c'){   // Routine Chirp
        mode = '1';
        __delay_cycles(500000); // Delay
    }
    else if (modeSelection_input == 'A'){  // Lost Alarm
        mode = '2';
        __delay_cycles(500000); // Delay
    }
    else if (modeSelection_input == 'O'){  // Obstacle Detection
        mode = '3';
        __delay_cycles(500000); // Delay
    }
}


void chirp(){
    tempo = 150;

    Note(A5, sixteenth);
    __delay_cycles(5000);
    Rest(whole);
    Rest(whole);
    Rest(whole);
}


void LostAlarm(){
    tempo = 120;

    P3->OUT ^= BIT6;
    Note(C5, quarter);
    __delay_cycles(5000);
    P3->OUT ^= BIT6;
    Note(C3, quarter);
    __delay_cycles(5000);
}


void dropOffAlarm(){
    tempo = 120;

    Note(C5, whole);
    __delay_cycles(5000);
    Rest(half);
}

void EUSCIA0_IRQHandler(void){
    if((EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) != 0){       //check whether UCA0RXIFG is set
        EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;  //read the data and write it back

        modeSelection();

        while((EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG) == 0);    //waits util UCA0TXIFG is set

        EUSCI_A0->IFG &= ~EUSCI_A_IFG_TXIFG;    //clear UCA0TXIFG flag
        EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    //clear UCA0RXIFG flag
    }
}
