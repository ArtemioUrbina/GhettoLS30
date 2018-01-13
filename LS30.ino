/*****************************************************************

     Ghetto LS-30 interface
         by Artemio Urbina 2018
         GNU GPL 2.0

  The idea is to use the extra buttons on 
  a kick harness and an Arduino Mega to 
  rotate as an LS-30 via two buttons.

  The third button is used constant rotation
  and pulse rotation (default).

  The LCD shield is not mandatoiry, only used
  fo enable and disable the protection for 
  Guevara/Guerilla War. This can also be changed
  with the variable as commented below
  
 *****************************************************************/

/* 
 *  Change this to 1 for Guerilla War 
 *  protection as default.
 */
int guerrilla_war = 0;
/* 
 *  Change this for constant rotation
 *  every N frames. Default is 6.
 */
int delay_frames = 6;

#include <EEPROM.h>
#include "lcdsimp.h"

#define LENGTH      12
#define ROT_POS     LENGTH
#define ROT_LAST    LENGTH + 1
#define PROT_CHECK  LENGTH + 2

#define P1_00     31
#define P1_01     33
#define P1_02     35
#define P1_03     37
#define P1_04     39
#define P1_05     41
#define P1_06     43
#define P1_07     45
#define P1_08     47
#define P1_09     49
#define P1_10     51
#define P1_11     53

int rot_bus1[LENGTH+3] = { P1_00, P1_01, P1_02, P1_03, P1_04, P1_05,
                        P1_06, P1_07, P1_08, P1_09, P1_10, P1_11, 0, 0, 0 };

#define P1_L      25
#define P1_R      27
#define P1_C      29

#define P2_00     30
#define P2_01     32
#define P2_02     34
#define P2_03     36
#define P2_04     38
#define P2_05     40
#define P2_06     42
#define P2_07     44
#define P2_08     46
#define P2_09     48
#define P2_10     50
#define P2_11     52

int rot_bus2[LENGTH+3] = { P2_00, P2_01, P2_02, P2_03, P2_04, P2_05,
                        P2_06, P2_07, P2_08, P2_09, P2_10, P2_11, 0, 0, 0 };

#define P2_L      24
#define P2_R      26
#define P2_C      28

long magicSalt __attribute__ ((section(".noinit")));  // Magic number in EEPROM for static variables

#define DELAY_LEN delay_frames*16

#define ROT_LEFT  -1
#define ROT_RIGHT 1


void setArrayOutput(int *rotArray)
{
  int count = 0;
  
  for (count = 0; count < LENGTH; count++)
    pinMode(rotArray[count], OUTPUT);
}

void setArrayValue(int *rotArray, int val)
{
  int count = 0;
  
  for (count = 0; count < LENGTH; count++)
    digitalWrite(rotArray[count], val);
}

void PrepareOutput()
{
  setArrayOutput(rot_bus1);
  setArrayValue(rot_bus1, HIGH);
    
  digitalWrite(rot_bus1[0], LOW);

  pinMode(P1_L, INPUT_PULLUP);
  pinMode(P1_C, INPUT_PULLUP);
  pinMode(P1_R, INPUT_PULLUP);

  setArrayOutput(rot_bus2);
  setArrayValue(rot_bus2, HIGH);
    
  digitalWrite(rot_bus2[0], LOW);

  pinMode(P2_L, INPUT_PULLUP);
  pinMode(P2_C, INPUT_PULLUP);
  pinMode(P2_R, INPUT_PULLUP);
}

/*****************************************************************************

Guerrilla War protection (From MAME driver)

We add a 0xf value for 1 input read once every 8 rotations.
0xf isn't a valid direction, but avoids the "joystick error" protection
which happens when direction changes directly from 0x5<->0x6 8 times.
The rotary joystick is a mechanical 12-way positional switch, so what happens
is that occasionally while rotating the stick none of the switches will be
closed. The protection test verifies that this happens, to prevent replacement
of the rotary stick with a simple TTL counter.
Note that returning 0xf just once is enough to disable the test. On the other
hand, always returning 0xf inbetween valid values confuses the game.

*****************************************************************************/

void movePosition(int *rotArray, int value)
{
    //setArrayValue(rotArray, HIGH);
    digitalWrite(rotArray[rotArray[ROT_POS]], HIGH);

    rotArray[ROT_LAST] = rotArray[ROT_POS];
    rotArray[ROT_POS] += value;
    if(rotArray[ROT_POS] >= LENGTH)
      rotArray[ROT_POS] = 0;  
    if(rotArray[ROT_POS] < 0)
      rotArray[ROT_POS] = LENGTH - 1; 
       
    if(guerrilla_war)
    {
      if(rotArray[ROT_POS] == 6 && rotArray[ROT_LAST] == 5 ||
         rotArray[ROT_POS] == 5 && rotArray[ROT_LAST] == 6)
      {
        rotArray[PROT_CHECK]++;
      }
        
      if(rotArray[PROT_CHECK] < 7)
        digitalWrite(rotArray[rotArray[ROT_POS]], LOW);
      else
        rotArray[PROT_CHECK] = 0;
    }
    else
      digitalWrite(rotArray[rotArray[ROT_POS]], LOW);
}


//  ----------------------------------------------------------------------------------------------------------------------------
//  User Interface
//  ----------------------------------------------------------------------------------------------------------------------------

void DisplayIntro()
{
  Display("Ghetto LS30", "Artemio 2018");
  delay(1000);
}


//  ----------------------------------------------------------------------------------------------------------------------------
//  Main Loop
//  ----------------------------------------------------------------------------------------------------------------------------

void clearEEPROM()
{
  magicSalt = 0;
  EEPROM.put(0, magicSalt);
  Display("EEPROM cleared", "");
  delay(1000);
}


void LoadStatus()
{
  EEPROM.get(0, magicSalt);
  if(magicSalt != 946522)
  {
    guerrilla_war = 0;
    delay_frames = 6;
    SaveStatus();
  }
  else
  {
    EEPROM.get(sizeof(long) + 1*sizeof(int), guerrilla_war);
    EEPROM.get(sizeof(long) + 2*sizeof(int), delay_frames);
  }
}

void SaveStatus()
{
  magicSalt = 946522;
  EEPROM.put(0, magicSalt);
  EEPROM.put(sizeof(long) + 1*sizeof(int), guerrilla_war);
  EEPROM.put(sizeof(long) + 2*sizeof(int), delay_frames);
}

void DisplayStatus()
{
  char buff[20];
  
  sprintf(buff, "Delay frames: %d", delay_frames);
  DisplayTop(buff);
  if(guerrilla_war)
    DisplayBottom("Guevara/GW ON ");
  else
    DisplayBottom("Guevara/GW OFF");
}

void DisplayMenu()
{
    int k;
    
    k = WaitKey();
    if(k == btnUP || k == btnDOWN)
    {
      guerrilla_war = !guerrilla_war;
      SaveStatus();
      DisplayStatus();
    }
    if(k == btnLEFT)
    {
      delay_frames --;
      if(delay_frames < 0)
        delay_frames = 0;
      SaveStatus();
      DisplayStatus();
    }
    if(k == btnRIGHT)
    {
      delay_frames ++;
      if(delay_frames > 20)
        delay_frames = 20;
      SaveStatus();
      DisplayStatus();
    }
    if(k == btnSELECT)
    {
       delay_frames = 6;
       guerrilla_war = 0;
       SaveStatus();
       DisplayStatus();
    }
}

void setup()
{
  initLCD();
    
  lcd_key = read_LCD_buttons();  // have a clear reset if all goes wrong
  if(lcd_key != btnNONE)
    clearEEPROM();
    
  PrepareOutput();
  DisplayIntro();
  LoadStatus();
  DisplayStatus();
}

void loop()
{
  int p1DoDelay = 0, p2DoDelay = 0, i = 0, k = 0;
  int p1_bttn[3] = { HIGH, HIGH, HIGH };
  int p2_bttn[3] = { HIGH, HIGH, HIGH };
  int p1_bttn_last[3] = { HIGH, HIGH, HIGH };
  int p2_bttn_last[3] = { HIGH, HIGH, HIGH };

  while(1)
  {   
      p1_bttn[0] = digitalRead(P1_L);
      p1_bttn[1] = digitalRead(P1_C);
      p1_bttn[2] = digitalRead(P1_R);

      p2_bttn[0] = digitalRead(P2_L);
      p2_bttn[1] = digitalRead(P2_C);
      p2_bttn[2] = digitalRead(P2_R);

      if(p1_bttn_last[1] == HIGH && p1_bttn[1] == LOW)
        p1DoDelay = !p1DoDelay;
      if(!p1DoDelay)
      {
        if(p1_bttn_last[0] == HIGH &&  p1_bttn[0]== LOW)
          movePosition(rot_bus1, ROT_LEFT);
        
        if(p1_bttn_last[2] == HIGH && p1_bttn[2] == LOW)
          movePosition(rot_bus1, ROT_RIGHT);
      }
      else
      {
        if(p1_bttn[0] == LOW)
          movePosition(rot_bus1, ROT_LEFT);
        if(p1_bttn[2] == LOW)
          movePosition(rot_bus1, ROT_RIGHT);
          
        delay(DELAY_LEN);
      }

      if(p2_bttn_last[1] == HIGH && p2_bttn[1] == LOW)
        p2DoDelay = !p2DoDelay;
      if(!p2DoDelay)
      {
        if(p2_bttn_last[0] == HIGH && p2_bttn[0] == LOW)
          movePosition(rot_bus2, ROT_LEFT);
          
        if(p2_bttn_last[2] == HIGH && p2_bttn[2] == LOW)
          movePosition(rot_bus2, ROT_RIGHT);
      }
      else
      {
        if(p2_bttn[0] == LOW)
          movePosition(rot_bus2, ROT_LEFT);
        if(p2_bttn[2] == LOW)
          movePosition(rot_bus2, ROT_RIGHT);
          
        delay(DELAY_LEN);
      }

      for(i = 0; i < 3; i++)
        p1_bttn_last[i] = p1_bttn[i];
      for(i = 0; i < 3; i++)
        p2_bttn_last[i] = p2_bttn[i];
        

      // LCD keys 
      k = analogRead(0);        
      if(k < 1000)
          DisplayMenu();
  }
}

