#include "common.h"

/*
 * H8/538F, emulating the H28F101: Hitachi 131072-word x 8-bit CMOS
 * flash w/ programming
 */

/*
 * Pin map
 *
 * 71  26 # RES
 * 55  44 # OE
 * 61  50 # CE
 * 101 25 # WE
 * 19  Vcc# Vpp = 5V
 * 74 12  # EXTAL
 * 75 NONE# XTAL
 * 36 27  # EO0 (Data output 0)
 * 37 28  # EO1
 * 38 29  # EO2
 * 39 30  # EO3
 * 40 31  # EO4
 * 41 32  # EO5
 * 42 33  # EO6
 * 43 34  # EO7
 * 45 35  # EA0 (Address input 0)
 * 46 36  # EA1
 * 47 37  # EA2
 * 48 38  # EA3
 * 49 39  # EA4
 * 50 40  # EA5
 * 51 41  # EA6
 * 52 42  # EA7
 * 54 43  # EA8
 * 72 52  # EA9
 * 56 45  # EA10
 * 57 46  # EA11
 * 58 47  # EA12
 * 59 48  # EA13
 * 60 49  # EA14
 * 66 51  # EA15
 * 100 53 # EA16
 * 1   5V # VCC
 * 44  5V # VCC
 * 62  5V # VCC
 * 63  5V # VCC
 * 70  5V # VCC
 * 76  5V # VCC
 * 79  5V # VCC
 * 84  5V # VCC
 * 85  5V # VCC
 * 108 5V # VCC
 * 10     # VSS
 * 26     # VSS
 * 35     # VSS
 * 53     # VSS
 * 73     # VSS
 * 81     # VSS (MD0)
 * 82     # VSS (MD1)
 * 83     # VSS (MD2)
 * 98     # VSS
 * 99     # VSS
 * 103    # VSS
 * 109    # VSS
 */
uint8_t SHN28_A_B[] = { // Address Pins: Breakout board
  45, 46, 47, 48, 49,
  50, 51, 52, 54, 72,
  56, 57, 58, 59, 60,
  66, 100
};

uint8_t SHN28_A[]   = { // Address Pins: Arduino
  35, 36, 37, 38, 39,
  40, 41, 42, 43, 52,
  45, 46, 47, 48, 49,
  51, 53
};

uint8_t SHN28_O_B[] = { // Output Pins: Breakout board
  36, 37, 38, 39, 40,
  41, 42, 43
};
uint8_t SHN28_O[] =  {   // Output Pins
  27, 28, 29, 30, 31,
  32, 33, 34
};

//                        OE  CE  WE
uint8_t SHN28_CE[] =     {44, 50, 25};      // Chip Select/Enable Pins
uint8_t SHN28_SELECT[] = { 0,  0,  1};      // Status for enabling the chip

void config_hn28(){
    A_pins =  sizeof  SHN28_A;
    CE_pins = sizeof  SHN28_CE;
    O_pins =  sizeof  SHN28_O;
    A=SHN28_A;
    O=SHN28_O;
    CE=SHN28_CE;
    SELECT = SHN28_SELECT;
    PROMsize = pow(2,A_pins);
    //  Vcc = 13;
    //  GND = A7;
    pinMode(26, OUTPUT);
    digitalWrite(26, LOW);
    delay(1);
    digitalWrite(26, HIGH);

}

// First, boot chip in PROM mode using the mode pins.
// To then put the H8/538 into PROM Read mode, per Table 16-14 "Operating Mode Selection in PROM Mode"
// https://downloads.laboratoryb.org/insight/documents/H8_538F_HWM_linear.PDF#page=511
/// Vpp = Vcc = 5V
//// Confirmed independently by https://downloads.laboratoryb.org/insight/documents/HN28F101.pdf#page=5, footnote 6
/// CE Vlow
//// Vlow means 0V, per Table 16-16 "DC Characteristics in PROM Mode"
//// https://downloads.laboratoryb.org/insight/documents/H8_538F_HWM_linear.PDF#page=515
/// WE Vhigh (5V OK)
//// This is also confirmed to NOT be 12V later on:
//// https://downloads.laboratoryb.org/insight/documents/HN28F101.pdf#page=5
//// Basically, Vih has to mean "high value" -- the HN28F101 document pins 2.2V <= Vih <= Vcc + 0.3
/// OE Vlow
//// Again, see https://downloads.laboratoryb.org/insight/documents/H8_538F_HWM_linear.PDF#page=511
