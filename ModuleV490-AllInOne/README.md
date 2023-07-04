# DIYBMS All-in-one 16S monitoring board

This is the hardware for the (up to) 16S monitoring board.

It uses an [STM32F030K6T6](https://www.st.com/en/microcontrollers-microprocessors/stm32f030k6.html) chip, with 32K flash and 4K RAM.

**WARNING**: This hardware is considered experimental & prototype, use at your own risk - it may not work as intended or at all.

## Background Information
### Software
The software for this board is available pre-compiled.  See this [link for details](https://github.com/stuartpittaway/diyBMSv4ESP32/tree/all-in-one/STM32All-In-One#readme)

Details are in that README on programming the STM32 chip on this PCB.

### KiCAD 7
This design uses KiCAD version 7.0.5.  Use the same or newer version for editing and viewing the files.

### JLCPCB
The files create PCB and design files suitable for "almost" full construction at JLCPCB.  

Look inside the "export" folder for the relevant files.  You will need these files for the construction:

* module_16s_gerbers.zip
* Module_16S_bom_jlc.csv
* Module_16S_cpl_jlc.csv

Support this project by using JLCPCB referral link
https://jlcpcb.com/RSZ
when registering for the first time.

# READ THIS FIRST - IMPORTANT

Before construction of the PCB you must determine what battery cells you wish to monitor and a suitable cell voltage range.

The PCB design has options for two voltage ranges, determined by parts U2 and U6.  *ONLY ONE OF THESE DEVICES MUST BE FITTED*

## LIFEPO4 cells upto 4.00V maximum

Use part U2, a Microchip voltage reference "MCP1501-40E/SN".  This is a 4.096V ±0.1% reference chip.

This part is available on JLCPCB as part number C1575589.

The pick and place file (CPL file) contains the part placement for this chip by default.  No further changes are required, except to double-check the order screens from JLCPCB.

Do not install part U6.

## Cell voltages upto 4.50V maximum

This is all 18650 style cell voltage chemistry - normally with a maximum of 4.25V.  This configuration can also be used with LIFEPO4 cells with voltages below 4.50V.

Use part U6, a Microchip voltage reference "MCP1502T-45E/CHY".   This is a 4.500V ±0.1% reference chip.

This part is NOT available from JLCPCB.  This part must be obtained from [other sources](https://www.digikey.co.uk/en/products/detail/microchip-technology/MCP1502T-45E-CHY/16549208) and **MANUALLY** soldered onto the board.  Note this is a very small chip (SOT-23-6) and difficult to solder.

Edit the pick and place (CPL file) before upload to JLCPCB to remove the line marked "U2".

Do not install part U2.

## BATTERY VOLTAGES BELOW 24V

For battery voltages BELOW 24V (absolute maximum), which typically includes 4S or 5S cell configurations.  Ensure that the battery will not go above 24V during charging cycles.

Parts of the circuit are not required and must be ommitted for correct operation at below 24V.  These parts operate the step-down regulator normally required for voltages over 24V, such as a 16S setup.  

In this configuration, you do not require, and *MUST* not fit:

* U10 - XL7005A
* L1
* D21
* R76
* R77
* C44
* C43
* C45

*Edit the pick and place and BOM files to remove any reference to the above parts, before ordering from JLCPCB*

Once these parts are removed, the board must **NEVER** be used with a battery over 24V or damage will occur.

You may want to consider fitting a smaller connector (J7) such [as this one](https://www.lcsc.com/product-detail/Pluggable-System-Terminal-Block_Ningbo-Kangnex-Elec-WJ15EDGRC-3-81-6P_C7246.html) to prevent mistakes being made.  If you do this, you will also need to add a wire between pin 6 and pin 17 of J7 on the PCB, to ensure the MAX chip receives power.

Once the board is constructed, a wire link between J9 and pin 2  (indicated by square box on silkscreen) of JP1/PWR should be made.  This connects the full battery voltage to the board and bypasses the step down circuit.

In this configuration test point "TP1" will be zero volt/not connected.


## PASSIVE BALANCE BOARD

The passive balance PCB is part of a seperate project folder.

If you have no requirement for the passive balance board (its optional) then you can also omit the two headers marked J14 and J15 (both 22 pin).

## EXTERNAL TEMPERATURE SENSOR BOARD

The external temperature sensor PCB is part of a seperate project folder.

# CONNECTING THE ALL-IN-ONE MONITORING BOARD

Voltage monitoring connections are made using plugable terminal blocks, which are in male/female halves.

