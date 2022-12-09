# Glint - Windows system activity monitor 

Glint is a Windows computer monitoring tool, that uses Windows system performance counters. 
Most recent counter values are displayed as graphs, bars or glint lights indicators.
Up to 200 indicators can be shown simultaneously.
The program also displays most active processes in their descending activity order.
When mouse hovers over processes pane, the most active processes for the last 60 seconds are shown in tooltip.
Counter names with their current and maximum values are shown in tool tips. 
Settings dialog allows to select available Windows system counters and tune their display modes. 

---
##    Screenshots

![screen1](/doc/mainfraime_panel.JPG "Glint predecessor")  

Glint predecessor

![screen2](/doc/glint_mini_lights.TIF "Mini-bar monitoring lights view")  

Mini-bar monitoring lights view

![screen3](/doc/Glint_screenshot_0.gif "Column bars view")  

Column bars view.

![screen4](/doc/Glint_screenshot_1.gif "Graphs View on remote computer")  

Graphs View on remote computer.

---
##    Building the program
- Open solution file Project\glint.sln in Microsoft Visual Studio Community 2022 (64-bit).
- Run Build command (F7).

---
##     Troubleshooting notes 

-  System Performance counters in Registry must be available to the program for reading.     
 In case of doubt run Microsoft Performance monitor and if it gives error messages, run command: LODCTR /R  in command line window in  Administrator mode to restore system counters.

- If some counters are not displaying (greyed or missing in Settings dialog), check their availability by command: LODCTR /Q    
If they are disabled, enable them by command: LODCTR /E<service name>

- If remote computer performance counters are not available, check that Remote Registry Service is running on the remote computer. 
