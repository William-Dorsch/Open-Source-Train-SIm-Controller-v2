# Open-Source-Train-SIm-Controller-v2

After 9 months I finally made a custom train sim controller I'm happy with, so here's a framework on how to build your own

Attached in this repo are both the files (stl and fbx) of my current design, as well as the scripts neccesary to make them work
(and perhaps documentation on how to make your own scripts for your own projects in the future)

### PRELIMINARY BUILD GUIDE ###
https://docs.google.com/document/d/1KSgfsF_5sMxbXlPAzv2AgdFZr4h-Y06j3LuT_041sC8/edit?tab=t.0#heading=h.wbio56opicyw<br>

# Hardware
You can either use my current design (based off of a North American desktop setup), or venture into CAD and make your own
FOr my design, you will need the following materials:<br/>
- Arduino Leonardo (or anything that has the usb serial whatever interface) <br/>
- 50 #4 1/2" screws to be safe (you can buy two boxes of 25 from Ace for 5 bucks<br/>
- 1 6mm ball bearing <br/>
- a couple pen springs will need to be sacrificed, 13mm length<br/>
- 4 5x2mm magnets are necessary for the reverser<br/>
- 4 potentiometers (digikey part #  PDB12-H4301-103BF-ND, I suggest buying a couple spares in case something goes wrong while oldering (ask me how I know))<br/>
- 1 cherry MX brown switch (you can find a pack of them for cheap on Amazon, this is for the bail off, if you don't use bail off then you don't need this)<br/>
- black filament for the housing, need like 600g of that, then you can use whatever other colors you want.  Shouldn't be more than a spool total for the other parts, I use light gray for the handles, gray for the underframe, and gold to fake the brass brake valve parts<br/>
- A printer with a 256cm cubed build volume.  It will get really tricky if you don't, the throttle housing barely fit on my printer (Bambu P1S) <br/>
- some small gauge wire to connect to the potentiometers, the rest is up to you to make<br/>
- a soldering iron<br/>
- some electrical tape (used for friction on the independent brake)<br/>
- something to be used as a counterweight on the levers (I used pinewood derby car weights and pennies, and just hot glued these in place)<br/>

You only need to print 1 of all the parts, EXCEPT FOR THE FOLLOWING<br/>
- 5x axleSupportTall.stl<br/>
- 3x potentiometerHolderTall.stl<br/>
- 2x reverserAxleSupport<br/>

You'll also need to print a reverser, such as this one: https://makerworld.com/en/models/919480-locomotive-reverser#profileId-881394<br/>
(huge credit to this design it is exactly what I was looking for)<br/>
And you'll need 2 printed m5 bolts to allow the throttle and independent brake handles to pivot, such as: https://www.printables.com/model/48364-3d-printable-m5-hex-screw-and-nut

There are some images attached to show how to assemble it, i think you'll be able to figure it out.

# Software
There are a variety of scripts included with this project<br/>
- an arduino sketch to pass data over the serial<br/>
- an arduino keybind-based sketch (for RSU)<br/>
- a python script to interface with the run8 UDP<br/>
- a python script to send the serial data over a local TCP server, paired with a c# .exe to interface with train sim classic<br/>
So you will need the latest version of python installed on your computer, as well as the arduino IDE.

To use it with run8, load the serialInterface arduino file, and run the run8 python script.  Ensure the com port is set correctly in the python script (use VScode or whatever to edi tit)<br/>
To use with TSC, load the serialInterface arduino file, run the TSCinterface python script, and run the code from Program.cs via visual studio.  I'll get an exe in there eventually, again ensure the com port in python is set correctly<br/>
To use it with RSU, just load the RSU arduino script and that's it.

NOTE: you will need the serial and socket libraries for python.  Get them by running the following in your command prompt: <br/>
>pip install serial <br/>
>pip install socket <br/>


NOTE: The TSC script is currently programmed to be used with the Surliner CC Enhancement Pack.  The ControlIDs change from locomotive to locomotive.  Use the function GetControllerList(); to return the list of controls for your specific locomotive.  The index begins at 0, count up from there for the ID of each control.  Plug those into the variables at the top of the script.
If you run it from Visual Studio, you will need the NuGet System.IO.ports package installed (right click the project and go to manage NuGet packages in the solution explorer)

## Calibration
- The values in the arduino script are hard-coded but can be changed<br/>
- For the throttle, note the output from the throttle potentiometer at each notch, and put that into the notch centers array.  If the value varies, add a piece of electrical tape on the potentiometer to avoid wiggle between it and the axle<br/>
- for the dynamic brake, note the value right after the notch, and the value while fuily applied.  Use those in the mapping function.<br/>
- For the automatic brakes, you need to note the values at release, minimum application, full service, suppression, handle off, and emergency (the 6 notches).  Plug in the values from there.<br/>
- FOr the independent brakes, just note the fully released as well as the fully applied values, and apply those to the mapping function<br/>
- For the reverser, note the center and fwd / rvs values.  Choose something halfway between the center and extremes to plug into the function.<br/>
