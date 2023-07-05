# DIYBMS All-in-one 16S monitoring board

This is the hardware for the (up to) 16S monitoring board.

It uses an [STM32F030K6T6](https://www.st.com/en/microcontrollers-microprocessors/stm32f030k6.html) chip, with 32K flash and 4K RAM.

**WARNING**: This hardware is considered experimental & prototype, use at your own risk - it may not work as intended or at all.

## Background Information
### Software
The software for this board is available pre-compiled.  See this [link for details](https://github.com/stuartpittaway/diyBMSv4ESP32/tree/all-in-one/STM32All-In-One#readme)

Details are in the README on programming the STM32 chip for this PCB.

### KiCAD 7
This design uses KiCAD version 7.0.5.  Use the same or newer version for editing and viewing the files.  You don't need to install KiCAD if you just wish to build one.

### JLCPCB
This design will create a PCB and design files suitable for "almost" full construction at JLCPCB.  Some manual soldering will still be required for the connectors and cabling.

Look inside the "export" folder for the relevant production files.  You will need these files for the construction:

* module_16s_gerbers.zip
* Module_16S_bom_jlc.csv
* Module_16S_cpl_jlc.csv

Support this project by using JLCPCB referral link
https://jlcpcb.com/RSZ
when registering for the first time.

# READ THIS FIRST - IMPORTANT

Before construction of the PCB you must determine what battery cells you wish to monitor and a suitable cell voltage range.

The PCB design has options for two voltage ranges, determined by parts U2 and U6.  *ONLY ONE OF THESE DEVICES MUST BE FITTED*

The design uses a MAX14921 chip, this has a maximum of 16 cells and **65V** total battery voltage - do not exceed.

## LIFEPO4 cells upto 4.00V maximum

Use part U2, a Microchip voltage reference "MCP1501-40E/SN".  This is a 4.096V ±0.1% reference chip.

This part is available on JLCPCB as part number C1575589.

The pick and place file (CPL file) contains the part placement for this chip by default.  No further changes are required, except to double-check the order screens from JLCPCB.

Do not install part U6.

*This option has been tested by the author as working*

## Cell voltages upto 4.50V maximum

This is all 18650 style cell voltage chemistry - normally with a maximum of 4.25V.  This configuration can also be used with LIFEPO4 cells with voltages below 4.50V.

Use part U6, a Microchip voltage reference "MCP1502T-45E/CHY".   This is a 4.500V ±0.1% reference chip.

This part is NOT available from JLCPCB.  This part must be obtained from [other sources](https://www.digikey.co.uk/en/products/detail/microchip-technology/MCP1502T-45E-CHY/16549208) and **MANUALLY** soldered onto the board.  Note this is a very small chip (SOT-23-6) and difficult to solder.

Edit the pick and place (CPL file) before upload to JLCPCB to remove the line marked "U2".

Do not install part U2.

*This option has NOT been tested by the author, please feedback if you decide to experiment*

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

*This option has NOT been tested by the author, please feedback if you decide to experiment*

## PASSIVE BALANCE BOARD

The passive balance PCB is part of a seperate project folder named "ModuleV490-PassiveBal".

If you have no requirement for the passive balance board (its optional) then you can also omit the two headers marked J14 and J15 (both 22 pin).

## EXTERNAL TEMPERATURE SENSOR BOARD

The external temperature sensor PCB is part of a seperate project folder named "TemperaturePCB".

If you already have previous v4.50 DIYBMS monitoring boards, the same temperature sensor can be used with this design.

# CONNECTING THE ALL-IN-ONE MONITORING BOARD

Cell voltage connections are made using plugable terminal blocks.  The large 17 pin connector is connected to the most negative cell terminal, and then every positive connection to the other cells in the battery.

Ensure that you have the correct sequence of cell voltages before connecting to the BMS.  Test the pins on the plug with a multimeter to ensure the voltage continualy increases as you move up the pins (when compared to negative pin).

If you are using less than 16 cells, use wire connections between the screw terminals to link the highest cell to the remaining terminals.  For example, a 14S setup, would need a wire link between terminals 14 to 15 and 15 to 16.

The two pin plug is used to power the BMS.  This needs to be connected to the negative and positive of the battery.  Note the correct orientation of the two pins, the positive is near the edge of the board.  The silkscreen on the bottom of the PCB has the details.

Important:
* Do not reverse polarity.
* Connect the 17 pin connector FIRST before connecting the power connector.
* Ensure the PRG/RUN jumper is in the RUN position for normal operation.
* Ensure the PWR jumper is connected (except 24V setups see above).
* If the LED flashes a sequence on start up, check the software README to determine the cause

