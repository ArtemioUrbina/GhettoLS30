# GhettoLS30

  The idea is to use the extra buttons on 
  a kick harness and an Arduino Mega to 
  rotate as an LS-30 via two buttons.
  
  The third button is used constant rotation
  and pulse rotation (default).

  The LCD shield is not mandatoiry, only used
  fo enable and disable the protection for 
  Guevara/Guerilla War. This can also be changed
  with the variable as commented at the start of
  the code.

# Features
  
  - Supports Guerilal War protection Mode
  - Has variable frame delay for constant rotation
  - Low cost and no extra Control Panel for a playable experience

# Cabling

All grounds must be connected

Player 1:

    Arduino     Molex 13 Pin
        31      13
        33      12
        35      11
        37      10
        39       9
        41       8
        43       7
        45       6
        47       5
        49       4
        51       3
        53       2
        GND      1


Player 2:

    Arduino     Molex 13 Pin
        30      13
        32      12
        34      11
        36      10
        38       9
        40       8
        42       7
        44       6
        46       5
        48       4
        50       3
        52       2
        GND      1

Kick Harness:

    Arduino     Buttons
        25      P1 Rotate Left
        27      P1 Rotate Right
        29      P1 Change Mode
        24      P1 Rotate Left
        26      P2 Rotate Right
        28      P2 Change Mode
       GND      Kick Harness Ground
