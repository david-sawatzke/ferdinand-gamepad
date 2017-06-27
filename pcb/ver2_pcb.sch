EESchema Schematic File Version 2
LIBS:switches
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:ver2_pcb-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L +12V #PWR?
U 1 1 5912A9BF
P 5600 3400
F 0 "#PWR?" H 5600 3250 50  0001 C CNN
F 1 "+12V" H 5600 3540 50  0000 C CNN
F 2 "" H 5600 3400 50  0001 C CNN
F 3 "" H 5600 3400 50  0001 C CNN
	1    5600 3400
	1    0    0    -1  
$EndComp
$Comp
L L L?
U 1 1 5912AA14
P 5800 3450
F 0 "L?" V 5750 3450 50  0000 C CNN
F 1 "L" V 5875 3450 50  0000 C CNN
F 2 "" H 5800 3450 50  0001 C CNN
F 3 "" H 5800 3450 50  0001 C CNN
	1    5800 3450
	0    -1   -1   0   
$EndComp
$Comp
L SW_Push SW?
U 1 1 5912AD02
P 6000 3850
F 0 "SW?" H 6050 3950 50  0000 L CNN
F 1 "SW_Push" H 6000 3790 50  0000 C CNN
F 2 "" H 6000 4050 50  0001 C CNN
F 3 "" H 6000 4050 50  0001 C CNN
	1    6000 3850
	0    1    1    0   
$EndComp
Wire Wire Line
	5600 4100 5600 4150
$Comp
L GND #PWR?
U 1 1 5912AD5B
P 5600 4150
F 0 "#PWR?" H 5600 3900 50  0001 C CNN
F 1 "GND" H 5600 4000 50  0000 C CNN
F 2 "" H 5600 4150 50  0001 C CNN
F 3 "" H 5600 4150 50  0001 C CNN
	1    5600 4150
	1    0    0    -1  
$EndComp
Wire Notes Line
	6150 3200 6150 4350
Wire Notes Line
	6300 3200 6300 4350
Wire Notes Line
	6300 4350 7250 4350
Wire Notes Line
	6300 3200 7250 3200
$Comp
L GND #PWR?
U 1 1 5912AE10
P 7150 4150
F 0 "#PWR?" H 7150 3900 50  0001 C CNN
F 1 "GND" H 7150 4000 50  0000 C CNN
F 2 "" H 7150 4150 50  0001 C CNN
F 3 "" H 7150 4150 50  0001 C CNN
	1    7150 4150
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5912AE99
P 6500 3450
F 0 "R?" V 6580 3450 50  0000 C CNN
F 1 "R" V 6500 3450 50  0000 C CNN
F 2 "" V 6430 3450 50  0001 C CNN
F 3 "" H 6500 3450 50  0001 C CNN
	1    6500 3450
	0    -1   -1   0   
$EndComp
Text GLabel 6800 3650 2    60   Input ~ 0
Eingang
$Comp
L +5V #PWR?
U 1 1 5912AF22
P 7150 3400
F 0 "#PWR?" H 7150 3250 50  0001 C CNN
F 1 "+5V" H 7150 3540 50  0000 C CNN
F 2 "" H 7150 3400 50  0001 C CNN
F 3 "" H 7150 3400 50  0001 C CNN
	1    7150 3400
	1    0    0    -1  
$EndComp
Text Notes 5750 4200 0    60   ~ 0
Traktor
Text Notes 6350 4200 0    60   ~ 0
Mikrocontroller
Wire Wire Line
	5600 4100 7150 4100
Wire Notes Line
	7250 3200 7250 4350
Wire Notes Line
	6150 4350 5450 4350
Wire Notes Line
	5450 4350 5450 3200
Wire Notes Line
	5450 3200 6150 3200
Wire Wire Line
	7100 3450 7150 3450
Wire Wire Line
	7150 3450 7150 3400
Wire Wire Line
	6650 3450 6800 3450
Wire Wire Line
	6750 3650 6800 3650
$Comp
L D D?
U 1 1 5927ED53
P 6750 3850
F 0 "D?" H 6750 3950 50  0000 C CNN
F 1 "D" H 6750 3750 50  0000 C CNN
F 2 "" H 6750 3850 50  0001 C CNN
F 3 "" H 6750 3850 50  0001 C CNN
	1    6750 3850
	0    1    1    0   
$EndComp
$Comp
L D D?
U 1 1 5927EDC3
P 6950 3450
F 0 "D?" H 6950 3550 50  0000 C CNN
F 1 "D" H 6950 3350 50  0000 C CNN
F 2 "" H 6950 3450 50  0001 C CNN
F 3 "" H 6950 3450 50  0001 C CNN
	1    6950 3450
	-1   0    0    1   
$EndComp
Connection ~ 6000 4100
Wire Wire Line
	6000 4050 6000 4100
Wire Wire Line
	5600 3400 5600 3450
Wire Wire Line
	5600 3450 5650 3450
Wire Wire Line
	6000 3450 6000 3650
Connection ~ 6000 3450
Connection ~ 6350 3450
Connection ~ 6750 3450
Wire Wire Line
	5950 3450 6350 3450
Wire Wire Line
	6750 3450 6750 3700
Connection ~ 6750 3650
Wire Wire Line
	7150 4100 7150 4150
Wire Wire Line
	6750 4100 6750 4000
Connection ~ 6750 4100
$EndSCHEMATC
