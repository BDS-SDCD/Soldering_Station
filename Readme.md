
# MAIN

 	This soldering station developed for the handle of the Hakko T12 soldering iron (needs to be modified according to the given scheme) Heat gun with the 8858 substation (~220 spiral 12V fan). Anything that can heat up from ~220 and has a thermocouple will do as a soldering separator. In my case, it is literally an iron. This project was developed for home made one layer PCB based on photoresist technology and using the element base that is already available.<br/>

[Schematic](https://github.com/BDS-SDCD/Soldering_Station/tree/main/Station%20Schematic/Chematic%20PDF)<br/>
[Code](https://github.com/BDS-SDCD/Soldering_Station/tree/main/Station%20Code/Core)<br/>

### Features

 	Each of the devices has a set of settings. As a rule, when I’m working with a soldering station, I use only a few temperature settings, which are implemented here in adjustable temperature presets. Each of the presets has its own temperature correction value, which allows you to accurately adjust the temperature of each preset. Also, the manual control mode has a separate temperature correction value.

In this project I did not provide the ability to configure important parameters such as the PID controller coefficient and the temperature conversion coefficient in the user interface. All this can be configured in the [Soldering_Station.h](https://github.com/BDS-SDCD/Soldering_Station/blob/main/Station%20Code/Core/Inc/Soldering_Station.h) file by changing the Define value. Also you can change Button mapping of presets in this file. Everything else is configured inside the user interface.

All interfaces are symbolic.

 	Switching on/off any soldering device can be done either by additional methods or by short pressing the encoder. Switching control between the heat gun and the soldering separator can be done by holding the encoder SW1. Switching temperature settings as well as setting the temperature of the manual mode is done only when the soldering device is turned on.

 	Holding the SW4 encoder will lead to the transition to the menu with devices and their parameters. The transition from the menu to the previous menu is carried out by holding the SW4 encoder. When setting the value, several step modifiers are available by pressing buttons SW8 and SW2. The standard value is x1. When pressing button SW8, the modifier changes to x2. When you press SW8 button again, the modifier will return. SW8 button with the x5 modifier works the same way. If you turn on 2 modifiers in a free order, you will get a x10 modifier.

### Soldering iron

 	The advantages include the fact that, unlike the original Chinese Hakko T12 controller, you will no longer be able to hear annoying clicks during heating, since here PWM is used to control the soldering iron. The PWM frequency is 24 kHz. Unlike the basic version of the soldering iron controller, it has the ability to press a button on the soldering iron to set the maximum temperature for heating massive soldering pads. The implementation of the button is the setting of the maximum temperature that the ADC can process. When implementing the supply of full power without regulation, it led me to several broken soldering tips.

### Soldering Heat Gun

 	As a control system for heating the heat gun, as well as the soldering separator, the Phase Angle Control(PAC) system is used. The control module for the air flow rate of the heat gun is completely linear and analog, which leads to the need to install a radiator on the MOSFET. Setting the minimum flow rate(RV6), as well as the maximum flow rate(RV4), is adjusted by variable resistors. A separate variable resistor is used to control the manual flow rate(RV3). The system automatically sets the maximum air flow rate during the cooling of the heat gun after switching off. Switching on/off can be done either from the control panel or by uninstalling/installing the heat gun on the stand. Ideally, the power part of the heat gun control should be connected so that the phase is broken, which in this case will ensure safe contact with the socket with the soldering station connected to the power.


### Soldering separator.

 	An old iron is used as a soldering separator in this project. This is the only test subject that survived many heating and cooling cycles, since the melting temperature of the soldering mouth is 187 degrees, and when soldering BGA chips, 270. Like the heat gun, the control system based on PAC. Switching on/off can be done either from the control panel.

### Update<br/>

	
	The PAC implementation has been changed from software to hardware. The input capture and PAC system is implemented based on the signal capture and PWM modes. 

### Further development.

 	The schematic provides for a ventilation control system. Since the Hakko T12 soldering iron can be modified similarly to the 908b Soldering iron smoke absorber. Since the project is being done primarily for myself, I have not yet decided which type of ventilation will be more convenient for me.

 	In the future, schematic of this module will be changed depending on my experience of use. At those moment, there is no software implementation of this unit.
In case of inconvenience during regular work, modification or replacement of the symbolic interface with a graphical one.
Also, if necessary, other functionality will be added.