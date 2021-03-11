EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "DIYBMS ISP Programmer Adapter"
Date "2021-03-11"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:AVR-ISP-6 J1
U 1 1 6030FCCE
P 2810 2935
F 0 "J1" H 2481 3031 50  0000 R CNN
F 1 "AVR-ISP-6" H 2481 2940 50  0000 R CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x03_P2.54mm_Vertical" V 2560 2985 50  0001 C CNN
F 3 " ~" H 1535 2385 50  0001 C CNN
F 4 "C124421" H 2810 2935 50  0001 C CNN "LCSCStockCode"
	1    2810 2935
	1    0    0    -1  
$EndComp
$Comp
L Connector:AVR-ISP-6 J2
U 1 1 60310A3D
P 4505 2935
F 0 "J2" H 4176 3031 50  0000 R CNN
F 1 "AVR-ISP-6" H 4176 2940 50  0000 R CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x03_P2.54mm_Vertical" V 4255 2985 50  0001 C CNN
F 3 " ~" H 3230 2385 50  0001 C CNN
F 4 "C124421" H 4505 2935 50  0001 C CNN "LCSCStockCode"
	1    4505 2935
	1    0    0    -1  
$EndComp
Wire Wire Line
	3210 2735 3210 2190
Wire Wire Line
	3210 2190 4905 2190
Wire Wire Line
	4905 2190 4905 2635
Wire Wire Line
	4905 2835 5020 2835
Wire Wire Line
	5020 2835 5020 2090
Wire Wire Line
	5020 2090 3325 2090
Wire Wire Line
	3325 2090 3325 2835
Wire Wire Line
	3325 2835 3210 2835
Wire Wire Line
	3210 2935 3455 2935
Wire Wire Line
	3455 2935 3455 2305
Wire Wire Line
	3455 2305 5125 2305
Wire Wire Line
	5125 2305 5125 2935
Wire Wire Line
	5125 2935 4905 2935
Wire Wire Line
	4905 3035 5010 3035
Wire Wire Line
	5010 3035 5010 3525
Wire Wire Line
	5010 3525 3210 3525
Wire Wire Line
	3210 3525 3210 3035
Wire Wire Line
	2710 3335 4210 3335
$Comp
L Connector:Conn_01x03_Female J3
U 1 1 603140EE
P 5710 2900
F 0 "J3" H 5738 2926 50  0000 L CNN
F 1 "Conn_01x03_Female" H 5738 2835 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x03_P2.54mm_Vertical" H 5710 2900 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/2010160505_CJT-Changjiang-Connectors-A2541HWV-3P_C225500.pdf" H 5710 2900 50  0001 C CNN
F 4 "C225500" H 5710 2900 50  0001 C CNN "LCSCStockCode"
	1    5710 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4375 2435 4375 2370
Wire Wire Line
	4375 2370 5305 2370
Wire Wire Line
	5510 2370 5510 2800
Wire Wire Line
	2710 2435 4375 2435
Connection ~ 4375 2435
Wire Wire Line
	4375 2435 4405 2435
Wire Wire Line
	5510 3000 5510 3425
Wire Wire Line
	5510 3425 5175 3425
Wire Wire Line
	4210 3425 4210 3335
Connection ~ 4210 3335
Wire Wire Line
	4210 3335 4405 3335
Wire Wire Line
	4905 2635 5360 2635
Wire Wire Line
	5360 2635 5360 2900
Wire Wire Line
	5360 2900 5510 2900
Connection ~ 4905 2635
Wire Wire Line
	4905 2635 4905 2735
Text GLabel 5020 2835 2    50   Input ~ 0
MOSI
Text GLabel 5125 2935 2    50   Input ~ 0
SCK
Text GLabel 5010 3035 2    50   Input ~ 0
RESET
Text GLabel 4905 2190 1    50   Input ~ 0
MISO
Text GLabel 4235 2435 1    50   Input ~ 0
VCC
$Comp
L power:GND #PWR?
U 1 1 604A6850
P 5175 3425
F 0 "#PWR?" H 5175 3175 50  0001 C CNN
F 1 "GND" H 5180 3252 50  0000 C CNN
F 2 "" H 5175 3425 50  0001 C CNN
F 3 "" H 5175 3425 50  0001 C CNN
	1    5175 3425
	1    0    0    -1  
$EndComp
Connection ~ 5175 3425
Wire Wire Line
	5175 3425 4210 3425
$Comp
L power:+3V3 #PWR?
U 1 1 604A6F0F
P 5305 2370
F 0 "#PWR?" H 5305 2220 50  0001 C CNN
F 1 "+3V3" H 5320 2543 50  0000 C CNN
F 2 "" H 5305 2370 50  0001 C CNN
F 3 "" H 5305 2370 50  0001 C CNN
	1    5305 2370
	1    0    0    -1  
$EndComp
Connection ~ 5305 2370
Wire Wire Line
	5305 2370 5510 2370
Text Notes 3455 1755 0    50   ~ 0
Probably easier to use a ribbon cable !!
$EndSCHEMATC
