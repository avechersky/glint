  Glint - Windows Computer Activity Monitor. 
       Version 1.28

Glint monitor shows system activity by using system performance counters
in local or remote computer. Recent values can be also shown as graphs.
It can display the most active processes in system in descending CPU
activity order. Counters names and last values are shown in tool tips.
Settings dialog allows to select available Windows system counters and 
tune their display modes. More than 200 indicators can be active at once.

            
    *******************
      Mouse commands  
    *******************
 Left Mouse Button - drag program window around the screen.
 Double Click      - toggles normal/mini-bar view mode.
 Mouse Right Click - shows the program commands menu.
                     (Shift-F10 from the keyboard)

    System tray icon mouse commands
    ------------------------------- 
 Left Click   - toggle mini-bar/normal view.
 Right Click  - shows program menu.


    **********************
      Keyboard commands:  
    **********************
  'M' or Double Click - toggle normal/mini-bar view mode.  
  'B'   - toggle normal/large panel view."            

  'G'   - Display glint lights indicators.            
  'O'   - Display column bar indicators.              
  'A'   - Display line graphs.                        

  'S'   - view/change settings.                       
  'N'   - change computer, i.e. show activity of another machine.   
          Administrative rights in the remote system are required.   
          NOTE: Remote computer name is entered without beginning '\'.

  Left   - decrease the time interval of indicators changing.
  Right  - increase the time interval of indicators changing.
  Up     - increase brightness of glinting indicators.
  Down   - decrease brightness of glinting indicators.

  'F'    - toggle 'Full View' mode on/off.  
  'T'    - toggle 'Topmost window' mode on/off.  
  'C'    - toggle showing most active processes in system in descending CPU activity order.  
  'V'    - toggle 'Show Values' in normal view mode.  
  'L'    - toggle showing labels above indicators on/off.  
  'I'    - toggle tool-tips display on hovering mouse over indicators.

  'R'    - reset counters (also time interval and brightness).
  'P'    - pausing on/off.  
   F1    - Help/About dialog.


 --------------------------------
    Indicators tool tips format
 --------------------------------
a) For glint lights indicators the tool tip format is:
    'Category: Counter (Instance) = current-value'

  The Instance name represents specific object in system that counter refers to, 
  i.e. one of devices, processes, system queues etc. 
  By default 'Instance-name' equals to '_Total', that means the sum of values
  for all objects in a given category.

b) For linear graphs the tool tip format is:
   'Category: Counter (Instance) = current-value {graph-min , graph-max}'

  The 'graph-min' and 'graph-max' values show the minimum and maximum
  values displayed by the corresponding graph. 
  If Graph maximum value is not fixed, the program automatically
  adjusts the scale of the graph to show it more precisely.

c) For column bars view mode the tool tip format is:
   'Category: Counter (Instance) = current-value {column-max}'
   
d) For active processes window the tool tip format is:
       Process  Average-CPU-usage  IO-data-bytes
  The I/O data byte count includes all file, network and device data transfers for a process.

            *********************************
                    Program Start
            *********************************

     glint.exe [-r] [schema-name]

Optional command line arguments:                   
  -r  : use Registry to store Settings instead of INI-file.
  schema-name : name of indicator schema to use upon start (without quotes).


            *********************************
                 Program Settings Dialogs
            *********************************

===============================
    General Options dialog
===============================
This dialog controls the common settings of the program.

    ----------------------
        Schemes
    ----------------------
This group of commands allows to store and retrieve the whole sets 
of program options, called schemes.   
   
  'Load' button 
Restore the previously stored settings with the selected scheme name in 
the schemes list. 

  'Save As' button 
Store the current settings by any user defined name. There are several 
predefined schemes, which are generated automatically during the first 
run of the program. 

  'Delete' button 
Delete the selected scheme. 
                       
  'Restore Default Schemes' button 
Reinitialize the predefined schemes, if they have been changed by the user. 
The operation does not affect other schemes saved by the user.

    ----------------------
      View Options
    ----------------------

    'Panel Display mode' combo list selects between:
 - Columns Bars display mode (key 'O').
 - Graphs display mode (key 'A'), showing last 35 average values per second.
 - Glinting Lights display mode (key 'G').

    'Mini bar view' checkbox (key 'M' or Mouse Double Click)
Toggle between normal and small view of the panel.

    'Large view' checkbox (key 'B')
Toggle the enlarged panel view. The large mode is useful for high resolution 
screens, i.e. 1600x1200 and higher.

    'Full set view' checkbox (key 'F')
Toggle between full/abridged indicators sets. An abridged indicators set can
be defined, using Settings dialog (checkbox 'Full mode only' from the Indicator
Options)  and quickly switch to full set and back.  This mode allows to
temporarily switch off costly or rarely used indicators. 

    'Show Values' checkbox (key 'V').
If this mode is on, the program shows current absolute values 
(average value per last second) inside normal glinting indicators. 
In graphs and column bar modes the current counter value is 
displayed below each indicator and the maximum counter value on top of it.

    'Show Processes' checkbox (key 'C')
Toggle display of the most active processes in system in descending 
CPU activity order. 

    'Show Labels' checkbox (key 'L')
Toggle display of tool tips, containing a counter name and the last 
second average value.

    'Indicators update interval (msec)' field.
    Range: [150,2000], default is 333.
The main time interval for requesting counter values and updating indicators.
This parameter can be also easily adjusted with 'Left' and 'Right' arrows keys 
during normal work of the program. 'Reset' command sets it to default value.

    'Processes update interval (msec)' field.
    Range: [500, 5000], default is 1300.
The time interval for updating the most active processes in system panel.
It has no effect if 'Show Processes' mode is off.

    'Values update interval (msec)' field.
    Range: [500, 5000], default is 1300.
The time interval for calculating and showing the average counter value per second.
The average value per second is shown inside indicator, if 'Show values' mode is on.
This interval is also used to update linear graphs of recent values.
 
    'Indicators in row' field.
    Range: [1, 30].
The number of indicators in each row for the normal view mode.
In mini-bar view mode this number multiplied by 2 is used.

    'Always on Top' checkbox (key 'T').
If this mode is on, the program always stays above other windows on the screen. 

    ----------------------
      Tooltips Options
    ----------------------

    'Show Tool Tips' checkbox (key 'I').
Toggle display of tool tips, containing a counter name and its last 
average value per second.

    'Show system tasks activity' 
Add system processes activity (including Glint itself) to Active Processes tooltip. 

    'Activity tracking time'
Set the activity time interval for Active Processes tooltip (default is 60 seconds). 


===============================
   Counters Selection dialog
===============================
This dialog allows to choose system counters for indication.

    'Counter Group' Combo list
Select one of several common groups of counter objects. i.e. categories.

    'Computer: machine-name' button
This button shows the name of currently selected machine to watch and 
also allows to connect to another machine, if pressed.
Administrative rights in the remote system are required.
You can first check a computer availability by running regedit.exe tool
and selecting it in "File->Connect Network Registry" dialog from its menu. 

   'Objects/Counters' tree view.
The tree view shows all counters currently available in the system 
and allows to add them to the list of selected counters.  

   'Counter Object Instances' listbox 
The listbox shows the currently active instances in system for the selected counter. 
A new copy of an indicator can be created with any instance name from the list. 
Multiple selection is supported in this listbox.

   'Selected Counters'  listbox 
Show counters currently selected for indication. 
Counters that are not active in the system are displayed with gray color.

   'Add' button.
Add a counter from the 'Objects/Counter' list. 
If no instances are selected from 'Counter Object Instances' listbox then
only one counter with an empty instance name is added.
For each selected item in 'Counter Object Instances' list a separate indicator
is added. If a counter has been added without instance name, then the program 
uses '_Total' instance name or the first instance, if there is no instance 
with the name '_Total' .  
    
   'Remove' button.
remove counters selected in 'Selected Performance Counters' list. 
Multiple selection is supported.


===============================
   Indicator Options dialog
===============================
Set display options for specific counters.

   'Counters' list 
This list shows all counters used for indication with their instance names.
When a counter in the list is selected, its indicator attributes are displayed  
on the right side of the dialog and can be changed there.
A counter title has format:  'Object: Counter' or 'Object: Counter (Instance)'
The counters listed in the same order they are shown in the program panel.
Multiple selection is supported in this listbox.

   'Move Up/Move Down' buttons.
These buttons change the order, in which indicators are shown in the display panel.
Indicators on top are displayed first, i.e. from left to right

    -----------------------------
     Indicator Properties group
    -----------------------------
NOTE: If checkbox in properties is grayed, it means that its value is undefined 
      and the previous value will be used.

   'Color' button. 
The button allows to define an indicator color. In glinting lights display mode  
it means indicator color in its maximum brightness.
If multiple counters with different colors are selected in 'Indicators Counter List', 
then button will become gray, and you can set any new color for all selected counters.

   'Indicator max value' edit box. 
This is the maximum value shown on graph. It's also used to calculate the current 
counter brightness or column bar height. 
This value is ignored, if 'Display local variations' option is on.

   'Label' edit box.
This string (up to 6 characters) will be shown above the indicator.

   'Visible' checkbox. 
If this checkbox is off, then indicator is not shown by the program. This 
option can be used to preserve indicator settings for the future possible use. 

   'Full mode only' checkbox. 
If this checkbox is on, the indicator is displayed only in full show 
mode. Full/Short modes is toggled by 'F' key.

   'Visible in mini-bar' checkbox. 
If this checkbox is off, then indicator is displayed only in 
normal display mode. Check this button to show it also in mini-bar mode.

   'Maximum automatically adjusted' checkbox. 
If this checkbox is on and current value is bigger than 'Indicator max value' 
then max value is adjusted to a current value, but it can not become less than
the value from 'Indicator max value' edit box.

   'Display local variations' checkbox. 
If this checkbox is on, the min and max values in graph are automatically
adjusted to local history and are not remembered. This option is useful to show local variations 
in counter value, such as amount of memory or network activity counters. 
If this checkbox is on, then 'Indicator max value' is ignored.


            *********************************
                    Hints and Tips:
            *********************************

1. To transfer program settings to other computer, including customer indicator schemes, 
   you can copy glint.ini file from program directory to the corresponding directory 
   on the other computer. This requires storing program settings in GLINT.INI file - 
   the default behavior since v. 1.25.  (See also: -r command line key) 

2. The renaming of an indicator scheme can be done by loading it ('Load' button), 
   saving it by different name ('Save As' button) and then deleting 
   the original scheme ('Delete' button).

3. Counters related to several system objects, that do not have '_Total' instance, may 
   require the proper selection of Counter Object Instance in Counter Selection dialog. 
   For example, for the object "Network Interface" each instance represents a specific 
   network adaptor in your machine. By default the program selects the first adaptor, 
   that may be not what you actually need.
   	
4. CAVEAT: Depending on Windows OS version, some performance counters can use significant 
   amount of CPU time, e.g. "Networking" counters on  Windows 7 first releases eat a lot of CPU. 
   To minimize Glint CPU usage use them only when it's really necessary.
   You can quickly switch some indicators on and off by 'F' key or 
   (see 'F' keyboard command for quick switching indicators on ). 


            *********************************
                History since version 1.22
            *********************************
v. 1.28  
    - Added mouse tooltip to Active Processes indicator. 
      The tooltip contains average CPU usage and amount of I/O data transfer for last 
	  60 seconds (the interval is configurable).
    - Added option 'Show system task activity' into General_Options settings dialog, 
      adding system processes (including Glint itself) to Active Processes tooltip. 
    - Added option 'Activity tracking time' into General_Options settings dialog, 
      to set the activity time interval for Active Processes tooltip (default is 60 secs). 
    - Removed 'Brightness coefficient' option from General_Options settings dialog.
	  The brightness of glinting lights is still modifiable by Up and Down keyboard arrows. 
    - Fixed too frequent tooltip updates in Vista and Win7.
v. 1.27  
    - Error handling on selecting different system counter configurations is improved.
    - "Maximum automatically adjusted" indicator option now works with a maximum value. 
      This value is used as a lower threshold for maximum indicator graph maximum value. 
    - Fixed "Visible in Mini-bar" indicator option.  
v. 1.26  
    - [build 1020] Fixed panel window positioning issue on multi-monitor system.  
    - [build 1019] Fixed extra lines in Aero screen theme on Windows 7.
v. 1.25  [build 1015] 
    - By default settings are now stored inside INI-file at working directory
          (can be reverted back to Windows Registry by command line key: -R).
v. 1.24  [build 1012] 
    - Added handling of mouse hovering over indicator labels and values text boxes.
    - Standard themes are improved, old NT4/Win2K stuff is removed.
    - Initial schemes now are: Standard, Large, Small, Few_Graphs.
v. 1.23  [build 1010] 
    - Fixed cursor disappearance in Windows 7 and program crashing in Vista.
  
-----------------------------------------------------------------------------------
            DISCLAIMER  
The program is distributed AS IS, without any charge, warranty or liability.  
-----------------------------------------------------------------------------------
See the latest version on the program site: http://sites.google.com/site/glintutility/

Copyright (C) 2001-2010  Alexander Vechersky  
email: avechersky@gmail.com
