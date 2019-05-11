EESchema Schematic File Version 4
EELAYER 29 0
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
L MyLibrary:WeMos_mini U1
U 1 1 5CD4757A
P 5100 4310
F 0 "U1" H 5100 4947 60  0000 C CNN
F 1 "WeMos_mini" H 5100 4841 60  0000 C CNN
F 2 "ESPControllerCircuit:wemos-d1-mini-with-pin-header" H 5650 3610 60  0001 C CNN
F 3 "" H 5650 3610 60  0000 C CNN
	1    5100 4310
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 5CD48DA7
P 4150 4460
F 0 "SW1" H 4150 4745 50  0000 C CNN
F 1 "SW_Push" H 4150 4654 50  0000 C CNN
F 2 "Buttons_Switches_THT:SW_PUSH_6mm" H 4150 4660 50  0001 C CNN
F 3 "~" H 4150 4660 50  0001 C CNN
	1    4150 4460
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 4460 4350 4460
$Comp
L power:+3.3V #PWR0101
U 1 1 5CD4A0EF
P 5960 3950
F 0 "#PWR0101" H 5960 3800 50  0001 C CNN
F 1 "+3.3V" H 5975 4123 50  0000 C CNN
F 2 "" H 5960 3950 50  0001 C CNN
F 3 "" H 5960 3950 50  0001 C CNN
	1    5960 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 3960 5800 3960
Wire Wire Line
	5960 3960 5960 3950
$Comp
L power:+5V #PWR0102
U 1 1 5CD4B4A8
P 4410 3930
F 0 "#PWR0102" H 4410 3780 50  0001 C CNN
F 1 "+5V" H 4425 4103 50  0000 C CNN
F 2 "" H 4410 3930 50  0001 C CNN
F 3 "" H 4410 3930 50  0001 C CNN
	1    4410 3930
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 3960 4410 3960
Wire Wire Line
	4410 3960 4410 3930
$Comp
L power:GND #PWR0103
U 1 1 5CD4B844
P 3660 4060
F 0 "#PWR0103" H 3660 3810 50  0001 C CNN
F 1 "GND" H 3665 3887 50  0000 C CNN
F 2 "" H 3660 4060 50  0001 C CNN
F 3 "" H 3660 4060 50  0001 C CNN
	1    3660 4060
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 4060 4020 4060
Wire Wire Line
	3950 4460 3950 4060
Connection ~ 3950 4060
Wire Wire Line
	3950 4060 3660 4060
$Comp
L Device:LED D1
U 1 1 5CD4CF59
P 6500 4260
F 0 "D1" H 6493 4005 50  0000 C CNN
F 1 "LED" H 6493 4096 50  0000 C CNN
F 2 "LEDs:LED_0805_HandSoldering" H 6500 4260 50  0001 C CNN
F 3 "~" H 6500 4260 50  0001 C CNN
	1    6500 4260
	-1   0    0    1   
$EndComp
$Comp
L Device:R R1
U 1 1 5CD4DABA
P 6040 4260
F 0 "R1" V 5833 4260 50  0000 C CNN
F 1 "512R" V 5924 4260 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5970 4260 50  0001 C CNN
F 3 "~" H 6040 4260 50  0001 C CNN
	1    6040 4260
	0    1    1    0   
$EndComp
Wire Wire Line
	5600 4260 5890 4260
Wire Wire Line
	6350 4260 6190 4260
$Comp
L power:GND #PWR0104
U 1 1 5CD4F11B
P 6830 4260
F 0 "#PWR0104" H 6830 4010 50  0001 C CNN
F 1 "GND" H 6835 4087 50  0000 C CNN
F 2 "" H 6830 4260 50  0001 C CNN
F 3 "" H 6830 4260 50  0001 C CNN
	1    6830 4260
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 4260 6830 4260
$Comp
L Connector:Conn_01x02_Female DEBUG1
U 1 1 5CD51ABF
P 4630 3140
F 0 "DEBUG1" H 4658 3070 50  0000 L CNN
F 1 "Conn_01x02_Female" H 4658 3070 50  0001 L CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 4658 2979 50  0001 L CNN
F 3 "~" H 4630 3140 50  0001 C CNN
	1    4630 3140
	1    0    0    -1  
$EndComp
Wire Wire Line
	4020 3240 4020 4060
Wire Wire Line
	4020 3240 4430 3240
Connection ~ 4020 4060
Wire Wire Line
	4020 4060 3950 4060
Wire Wire Line
	4600 4160 4290 4160
Wire Wire Line
	4290 4160 4290 3140
Wire Wire Line
	4290 3140 4430 3140
$Comp
L Isolator:PC817 U2
U 1 1 5CD55A2D
P 6420 2940
F 0 "U2" H 6420 3265 50  0000 C CNN
F 1 "HMHA2801" H 6420 3174 50  0000 C CNN
F 2 "Housings_SSOP:SOP-4_4.4x2.8mm_Pitch1.27mm" H 6220 2740 50  0001 L CIN
F 3 "https://uk.farnell.com/on-semiconductor/hmha2801/optocoupler-single-channel/dp/1652504" H 6420 2940 50  0001 L CNN
	1    6420 2940
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male RX1
U 1 1 5CD589E5
P 6990 3470
F 0 "RX1" H 7052 3514 50  0000 L CNN
F 1 "Conn_01x02_Male" H 6280 3410 50  0000 L CNN
F 2 "Connectors_JST:JST_PH_S2B-PH-K_02x2.00mm_Angled" H 6990 3470 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/2057211.pdf" H 6990 3470 50  0001 C CNN
	1    6990 3470
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x02_Male TX1
U 1 1 5CD595C3
P 7110 3000
F 0 "TX1" H 7082 2882 50  0000 R CNN
F 1 "Conn_01x02_Male" H 7082 2973 50  0000 R CNN
F 2 "Connectors_JST:JST_PH_S2B-PH-K_02x2.00mm_Angled" H 7110 3000 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/2057211.pdf" H 7110 3000 50  0001 C CNN
	1    7110 3000
	-1   0    0    1   
$EndComp
Wire Wire Line
	6790 3370 5800 3370
Wire Wire Line
	5800 3370 5800 3960
Connection ~ 5800 3960
Wire Wire Line
	5800 3960 5960 3960
$Comp
L Device:R R2
U 1 1 5CD5BA4A
P 6530 3690
F 0 "R2" V 6323 3690 50  0000 C CNN
F 1 "R" V 6414 3690 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 6460 3690 50  0001 C CNN
F 3 "~" H 6530 3690 50  0001 C CNN
	1    6530 3690
	0    1    1    0   
$EndComp
Wire Wire Line
	6790 3470 6680 3470
Wire Wire Line
	5690 3470 5690 4160
Wire Wire Line
	5690 4160 5600 4160
Wire Wire Line
	6680 3690 6680 3470
Connection ~ 6680 3470
Wire Wire Line
	6680 3470 5690 3470
Wire Wire Line
	5960 3960 6380 3960
Wire Wire Line
	6380 3960 6380 3690
Connection ~ 5960 3960
$EndSCHEMATC