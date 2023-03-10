# ESP controller and TFT screen

## BANG GOOD 
(affiliate links)
2.8 Inch ILI9341 240x320 SPI TFT LCD Display Touch
https://www.banggood.com/custlink/mmmRI7iYr3

## Amazon 
(affiliate links)
ESP32 DEVKITC (pack of 3)
https://amzn.to/3bFOaah

ESP32 DEVKITC (single board)
https://amzn.to/3qG3YOG

## JLCPCB
(affiliate links)
https://jlcpcb.com/RSZ


# Surface Mount Component List

These parts are necessary for the DIYBMS CONTROLLER
| Item | Qty | Reference(s) | Value | LibPart | Footprint | Datasheet | Arrow Part Number | Arrow Price/Stock | DONOTPLACE | Description | Field4 | Height | JLCPCBRotation | LCSCStockCode | Manufacturer_Name | Manufacturer_Part_Number | Mouser Part Number | Mouser Price/Stock | PartNumber |
| 1 | 6 | C1, C2, C6, C8, C9, C14 | 100nF | Device:C | Capacitor_SMD:C_0805_2012Metric | https://datasheet.lcsc.com/szlcsc/YAGEO-CC0805KRX7R9BB104_C49678.pdf |  |  |  |  |  |  |  | C49678 |  |  |  |  | CC0805KRX7R9BB104 |
| 2 | 2 | C3, C4 | 22uF 6.3V | Device:C | Capacitor_Tantalum_SMD:CP_EIA-3216-18_Kemet-A_Pad1.58x1.35mm_HandSolder | https://datasheet.lcsc.com/szlcsc/Hunan-Xiangyee-in-S-T-CA45-A010M226T_C126948.pdf |  |  |  |  |  |  |  | C110786 |  |  |  |  | TC212A226K006Y |
| 3 | 1 | C5 | 22uF | Device:C | Capacitor_SMD:C_1206_3216Metric |  |  |  |  |  |  |  |  | C5672 |  |  |  |  | CL31A226KPHNNNE |
| 4 | 4 | C7, C10, C11, C13 | 1uF | Device:C | Capacitor_SMD:C_0805_2012Metric |  |  |  |  |  |  |  |  | C28323 |  |  |  |  | CL21B105KBFNNNE |
| 5 | 1 | C12 | 0.1uF-100V | Device:C | Capacitor_SMD:C_0805_2012Metric |  |  |  |  |  |  |  |  | C696367 |  |  |  |  | C0805C104K1RECAUTO |
| 6 | 1 | D1 | RGB | Device:LED_RGB | LED_SMD:LED_RGB_5050-6 | https://datasheet.lcsc.com/szlcsc/Hongli-Zhihui-HONGLITRONIC-HL-AF-5060H271BU46FU79GC-S1-THL_C219409.pdf |  |  |  |  |  |  |  | C219409 |  |  |  |  |  |
| 7 | 2 | D2, D5 | LL4148 | Device:D | Diode_SMD:D_MiniMELF | https://datasheet.lcsc.com/szlcsc/Semtech-LL4148_C9808.pdf |  |  |  |  |  |  |  | C9808 |  |  |  |  | LL4148 |
| 8 | 2 | D3, D4 | YELLOW LED | Device:LED | LED_SMD:LED_0603_1608Metric_Pad1.05x0.95mm_HandSolder |  |  |  |  |  |  |  |  | C72038 |  |  |  |  |  |
| 10 | 1 | D8 | NUP2105LT1G | Power_Protection:NUP2105L | Package_TO_SOT_SMD:SOT-23 | http://www.onsemi.com/pub_link/Collateral/NUP2105L-D.PDF |  |  |  |  |  |  |  | C2976509 |  |  |  |  |  |
| 11 | 1 | F1 | Fuse, 1.85A | Device:Polyfuse | Fuse:Fuse_2920_7451Metric | https://datasheet.lcsc.com/szlcsc/TLC-Electronic-TLC-LSMD185_C262041.pdf |  |  |  |  |  |  |  | C262041 |  |  |  |  | TLC-LSMD185 |
| 19 | 1 | J15 | Molex 0472192001 | 0472192001:0472192001 | 0472192001 | https://www.molex.com/pdm_docs/sd/472192001_sd.pdf | 0472192001 | https://www.arrow.com/en/products/0472192001/molex |  | Memory Card Connectors 1.10MM MICRO SD 08P HINGE TYPE |  | 1.9 | 180 | C164170 | Molex | 0472192001 | N/A | https://www.mouser.com/Search/Refine.aspx?Keyword=N%2FA |  |
| 24 | 2 | Q1, Q2 | AO3400A | Device:Q_NMOS_GSD | Package_TO_SOT_SMD:SOT-23 | https://datasheet.lcsc.com/szlcsc/1812111733_Bourne-Semicon-Shenzhen-AO3400A_C344010.pdf |  |  |  |  |  |  |  | C344010 |  |  |  |  | AO3400A |
| 25 | 1 | Q3 | AO3401A | Transistor_FET:AO3401A | Package_TO_SOT_SMD:SOT-23 | https://datasheet.lcsc.com/szlcsc/Alpha-Omega-Semicon-AOS-AO3401A_C15127.pdf |  |  |  |  |  |  |  | C15127 |  |  |  |  |  |
| 26 | 1 | Q4 | AO3400A | Transistor_FET:AO3400A | Package_TO_SOT_SMD:SOT-23 | http://www.aosmd.com/pdfs/datasheet/AO3400A.pdf |  |  |  |  |  |  |  | C344010 |  |  |  |  | AO3400A |
| 27 | 5 | R1, R17, R18, R21, R25 | 220R | Device:R | Resistor_SMD:R_0805_2012Metric | ~ |  |  |  |  |  |  |  | C17557 |  |  |  |  | 0805W8F2200T5E |
| 28 | 3 | R2, R5, R16 | 2K2 | Device:R | Resistor_SMD:R_0805_2012Metric | ~ |  |  |  |  |  |  |  | C17520 |  |  |  |  | 0805W8F2201T5E |
| 29 | 1 | R3 | 180R | Device:R | Resistor_SMD:R_0805_2012Metric | ~ |  |  |  |  |  |  |  | C25270 |  |  |  |  | 0805W8F1800T5E |
| 30 | 5 | R4, R10, R12, R13, R14 | 560R | Device:R | Resistor_SMD:R_0805_2012Metric | ~ |  |  |  |  |  |  |  | C28636 |  |  |  |  | 0805W8F5600T5E |
| 31 | 3 | R6, R8, R9 | 120R | Device:R | Resistor_SMD:R_0805_2012Metric | ~ |  |  |  |  |  |  |  | C17437 |  |  |  |  | 0805W8F1200T5E |
| 32 | 4 | R7, R11, R15, R27 | 51K | Device:R | Resistor_SMD:R_0805_2012Metric | ~ |  |  |  |  |  |  |  | C17737 |  |  |  |  | 0805W8F5102T5E |
| 33 | 5 | R19, R22, R29, R31, R32 | 10K | Device:R | Resistor_SMD:R_0805_2012Metric | ~ |  |  |  |  |  |  |  | C17414 |  |  |  |  | 0805W8F1002T5E |
| 34 | 1 | R20 | 1M | Device:R | Resistor_SMD:R_0805_2012Metric | ~ |  |  |  |  |  |  |  | C17514 |  |  |  |  | 0805W8F1004T5E |
| 35 | 2 | R23, R30 | 10R | Device:R | Resistor_SMD:R_0805_2012Metric | ~ |  |  |  |  |  |  |  | C17415 |  |  |  |  | 0805W8F100JT5E |
| 36 | 3 | R24, R26, R28 | 22K | Device:R | Resistor_SMD:R_0805_2012Metric | ~ |  |  |  |  |  |  |  | C17560 |  |  |  |  | 0805W8F2202T5E |
| 40 | 1 | U1 | TXS0102DCU | Logic_LevelTranslator:TXS0102DCU | Package_SO:VSSOP-8_2.3x2mm_P0.5mm | http://www.ti.com/lit/gpn/txs0102 |  |  |  |  |  |  |  | C53434 |  |  |  |  |  |
| 41 | 1 | U2 | EL3H7(B)(TA)-G | Isolator:PC817 | Package_SO:SOP-4_4.4x2.6mm_P1.27mm | https://datasheet.lcsc.com/szlcsc/Everlight-Elec-EL3H7-B-TA-G_C32565.pdf |  |  |  |  |  |  |  | C32565 |  |  |  |  |  |
| 42 | 1 | U3 | TCA6416APWR | ControllerCircuit:TCA6416APWR | Package_SO:TSSOP-24_4.4x7.8mm_P0.65mm | https://www.ti.com/lit/gpn/TCA6416A |  |  |  |  |  |  |  | C130203 |  |  |  |  |  |
| 44 | 1 | U5 | SN65HVD75DR | ControllerCircuit-rescue:SN65HVD75DR-ControllerCircuit | Package_SO:SOIC-8_3.9x4.9mm_P1.27mm | https://www.ti.com/store/ti/en/p/product/?p=SN65HVD75DR |  |  |  |  |  |  |  | C57928 |  |  |  |  | SN65HVD75DR |
| 45 | 1 | U6 | AMS1117-3.3 | Regulator_Linear:AMS1117-3.3 | Package_TO_SOT_SMD:SOT-223-3_TabPin2 | http://www.advanced-monolithic.com/pdf/ds1117.pdf |  |  |  |  |  |  |  | C6186 |  |  |  |  | AMS1117-3.3 |
| 46 | 1 | U7 | SN65HVD1050D | Interface_CAN_LIN:SN65HVD1050D | Package_SO:SOIC-8_3.9x4.9mm_P1.27mm | http://www.ti.com/lit/ds/symlink/sn65hvd1050.pdf |  |  |  |  |  |  |  | C14019 |  |  |  |  |  |
| 47 | 1 | U8 | INA229 | ControllerCircuit-rescue:INA229-Q1-INA229-Q1 | Package_SO:VSSOP-10_3x3mm_P0.5mm | https://www.ti.com/lit/ds/symlink/ina229-q1.pdf |  |  |  |  |  |  |  | C2846803 |  |  |  |  | INA229AIDGSR |
| 48 | 2 | U9, U12 | AQY282SX | Relay_SolidState:ASSR-1218 | Package_SO:SO-4_4.4x3.6mm_P2.54mm | https://datasheet.lcsc.com/lcsc/1912111437_PANASONIC-AQY282SX_C250827.pdf |  |  |  |  |  |  |  | C250827 |  |  |  |  |  |

# Through Hole Component List
| Item | Qty | Reference(s) | Value | LibPart | Footprint | Datasheet | Arrow Part Number | Arrow Price/Stock | DONOTPLACE | Description | Field4 | Height | JLCPCBRotation | LCSCStockCode | Manufacturer_Name | Manufacturer_Part_Number | Mouser Part Number | Mouser Price/Stock | PartNumber |
| 12 | 4 | J1, J4, J7, J12 | ScrewTerminal | Connector:Conn_01x04_Female | ControllerCircuit:Terminal-Block_3.81_4P_LCSC_C395880 | ~ |  |  |  |  |  |  |  | C395880 |  |  |  |  |  |
| 13 | 2 | J2, J3 | ScrewTerminal3Pin | Connector:Conn_01x03_Female | ControllerCircuit:Terminal-Block_5.0mm_3P-LCSC_C395850 | ~ |  |  |  |  |  |  |  | C395850 |  |  |  |  |  |
| 14 | 1 | J5 | ScrewTerminal | Connector:Conn_01x02_Female | ControllerCircuit:Terminal-Block_3.81_2P-LCSC_C395878 | ~ |  |  |  |  |  |  |  | C395878 |  |  |  |  |  |
| 15 | 1 | J8 | 2x3 2.54mm IDC Connectors | Connector:AVR-ISP-6 | Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical | https://lcsc.com/product-detail/IDC-Connectors_Ckmtw-Shenzhen-Cankemeng-C132438_C132438.html |  |  |  |  |  |  |  | C132438 |  |  |  |  |  |
| 16 | 1 | J9 | Shunt | Connector:Conn_01x04_Female | ControllerCircuit:Terminal-Block_3.81_4P_LCSC_C395880 | ~ |  |  |  |  |  |  |  | C395880 |  |  |  |  |  |
| 17 | 1 | J10 | Conn_01x02_Female | Connector:Conn_01x02_Female | Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Horizontal | ~ |  |  |  |  |  |  |  | C376120 |  |  |  |  |  |
| 22 | 2 | K1, K2 | SRD-05VDC-SL-C | ControllerCircuit-rescue:SRD-05VDC-SL-C-ControllerCircuit | ControllerCircuit:RELAY_SRD-05VDC-SL-C | https://datasheet.lcsc.com/szlcsc/1811021933_Ningbo-Songle-Relay-SRD-05VDC-SL-C_C35449.pdf |  |  |  |  | SONGLE RELAY |  |  | C35449 |  |  |  |  | SRD-05VDC-SL-C |
| 23 | 1 | LCD1 | LCD | ControllerCircuit-rescue:LCD-ControllerCircuit | ControllerCircuit:TFTSPI_2_8_240x320_TOUCH |  |  |  |  |  |  |  |  | C52711/C358718 |  |  |  |  |  |
| 37 | 1 | RX1 | Receive | Connector:Conn_01x02_Male | Connector_JST:JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal |  |  |  |  |  |  |  |  | C157932 |  |  |  |  | S2B-PH-K-S-GW |
| 38 | 2 | SW1, SW2 | SW_Push | Switch:SW_Push | Button_Switch_THT:SW_PUSH_6mm_H5mm | ~ |  |  |  |  |  |  |  | C455101 |  |  |  |  | XUNPU TS-1002-10026 |
| 39 | 1 | TX1 | Transmit | Connector:Conn_01x02_Male | Connector_JST:JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal |  |  |  |  |  |  |  |  | C157932 |  |  |  |  | S2B-PH-K-S-GW |
| 43 | 1 | U4 | ESP32-DEVKITC-32D | ControllerCircuit-rescue:ESP32-DEVKITC-32D-ControllerCircuit | ControllerCircuit:MODULE_ESP32-DEVKITC-32D | Espressif Systems |  |  |  |  | 4 |  |  | C319202/C319202 |  |  |  |  |  |

# "Do not place" items

These parts are on the circuit diagram, but are not required.  Do not place/purchase these parts.

| Item | Qty | Reference(s) | Value          | LibPart         | Footprint                            | Datasheet                                                                     | Arrow Part Number | Arrow Price/Stock | DONOTPLACE | Description | Field4 | Height | JLCPCBRotation | LCSCStockCode | Manufacturer_Name | Manufacturer_Part_Number | Mouser Part Number | Mouser Price/Stock | PartNumber |
| ---- | --- | ------------ | -------------- | --------------- | ------------------------------------ | ----------------------------------------------------------------------------- | ----------------- | ----------------- | ---------- | ----------- | ------ | ------ | -------------- | ------------- | ----------------- | ------------------------ | ------------------ | ------------------ | ---------- |
|      |     | D6           | MHPC3528CRGBCT | Device:LED_RGBC | ControllerCircuit:LED_MHPC3528CRGBCT | https://datasheet.lcsc.com/lcsc/2202141600_MEIHUA-MHPC3528CRGBCT_C2962096.pdf |                   |                   |            |             |        |        |                |               |                   |                          |                    |                    |            |
