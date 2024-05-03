//#define USBCON


////////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES
////////////////////////////////////////////////////////////////////////////////
#include <Keyboard.h>
//#include <HID-Project.h>




////////////////////////////////////////////////////////////////////////////////
// CONFIG ALL THE THINGS
////////////////////////////////////////////////////////////////////////////////
#define KB_RESET    (11)
#define KB_COL_SEL  (12)
#define KB_ENABLE   (13)

#define KB_C2_D1    (26)
#define KB_C2_D2    ( 7)
#define KB_C2_D3    ( 6)
#define KB_C2_D4    ( 5)

#define COLUMNS     (18)
#define ROWS        ( 4)

#define DEBOUNCE    ( 5)




////////////////////////////////////////////////////////////////////////////////
// KEY CODE STATUS
////////////////////////////////////////////////////////////////////////////////
byte key_state[COLUMNS];
byte key_clean[COLUMNS * ROWS];




////////////////////////////////////////////////////////////////////////////////
// CUSTOM KEY CODES NOT DEFINED BY ARDUINO
////////////////////////////////////////////////////////////////////////////////
#ifndef KEY_KANA
#define KEY_KANA (0x88)
#endif




////////////////////////////////////////////////////////////////////////////////
// KEY CODES
////////////////////////////////////////////////////////////////////////////////
const byte KEYS[]={

  // COL 01
  KEY_F8,
  KEY_RETURN,
  '[',
  ']',

  // COL 02
  KEY_KANA,         // KANA
  KEY_RIGHT_SHIFT,
  '\\',             // ¥ |
  KEY_BACKSPACE,    // STOP

  // COL 03
  KEY_F7,
  '`',              // @
  '\'',             // :
  ';',              // ;

  // COL 04
  KEY_RIGHT_ALT,    // _
  '/',              // /
  '-',              // - =
  '=',              // ^

  // COL 05
  KEY_F6,
  'o',
  'l',
  'k',

  // COL 06
  '.',
  ',',
  'p',
  '0',

  // COL 07
  KEY_F5,
  'i',
  'u',
  'j',

  // COL 08
  'm',
  'n',
  '9',
  '8',

  // COL 09
  KEY_F4,
  'y',
  'g',
  'h',

  // COL 10
  'b',
  'v',
  '7',
  '6',

  // COL 11
  KEY_F3,
  't',
  'r',
  'd',

  // COL 12
  'f',
  'c',
  '5',
  '4',

  // COL 13
  KEY_F2,
  'w',
  's',
  'a',

  // COL 14
  'x',
  'z',
  'e',
  '3',

  // COL 15
  KEY_F1,
  KEY_ESC,
  'q',
  KEY_LEFT_CTRL,

  // COL 16
  KEY_LEFT_SHIFT,
  KEY_LEFT_GUI,   // GRPH
  '1',
  '2',

  // COL 17
  KEY_HOME,
  KEY_UP_ARROW,
  KEY_RIGHT_ARROW,
  KEY_LEFT_ARROW,

  // COL 18
  KEY_DOWN_ARROW,
  ' ',
  KEY_DELETE,
  KEY_INSERT
};




////////////////////////////////////////////////////////////////////////////////
// INIT ALL THE THINGS
// NOTE: lower delay values caused very frequent read errors from the keyboard
////////////////////////////////////////////////////////////////////////////////
void delayWrite(int pin, int value) {
  digitalWrite(pin, value);
  delayMicroseconds(50);
}




////////////////////////////////////////////////////////////////////////////////
// INIT ALL THE THINGS
////////////////////////////////////////////////////////////////////////////////
void setup() {
  // SETUP USB HID KEYBOARD
  Keyboard.begin();

  // ZERO OUT ALL EXISTING KEY STATES
  for (auto i=0; i<COLUMNS; i++) {
    key_state[i] = 0;
  }

  // ZERO OUT ALL DEBOUNCE DATA
  for (auto i=0; i<COLUMNS*ROWS; i++) {
    key_clean[i] = 0;
  }


  // FIXING HARDWARE BUGS
  pinMode(15, INPUT_PULLDOWN);
  pinMode(8,  OUTPUT);
  digitalWrite(8, HIGH);
  

  // SETUP DATA INPUT REGISTERS
  pinMode(KB_C2_D1,       INPUT_PULLDOWN);
  pinMode(KB_C2_D2,       INPUT_PULLDOWN);
  pinMode(KB_C2_D3,       INPUT_PULLDOWN);
  pinMode(KB_C2_D4,       INPUT_PULLDOWN);

  // SETUP CONTROL REGISTERS
  pinMode(KB_RESET,       OUTPUT);
  pinMode(KB_ENABLE,      OUTPUT);
  pinMode(KB_COL_SEL,     OUTPUT);

  // INITIALIZE KEYBOARD
  delayWrite(KB_RESET,    HIGH);
  delayWrite(KB_RESET,    LOW);  
  delayWrite(KB_COL_SEL,  LOW);  
  delayWrite(KB_ENABLE,   HIGH);
}




////////////////////////////////////////////////////////////////////////////////
// READ ME SOME BUTT-ONZ
////////////////////////////////////////////////////////////////////////////////
void loop() {
  byte new_state;
  byte col = 0;
  char buff[32];

  for (auto i=0; i<COLUMNS; i++) {
    new_state = digitalRead(KB_C2_D1) << 0
              | digitalRead(KB_C2_D2) << 1
              | digitalRead(KB_C2_D3) << 2
              | digitalRead(KB_C2_D4) << 3;

    // PROCESS KEY INPUT
    for (auto x=0; x<ROWS; x++) {
      byte *clean = &key_clean[(i * ROWS) + x];
      byte bit1   = (new_state    >> x) & 0x01;
      byte bit2   = (key_state[i] >> x) & 0x01;

      // KEY IS UNCHANGED, DO NOTHING
      if (bit1 == bit2) {
        *clean = 0;

      // KEY IS CHANGED,
      // DO SOMETHING IF CONSISTENT FOR (DEBOUNCE) "TICKS"
      // THIS THRESHOLD HANDLES THE "DEBOUNCE" MECHANIC
      } else if (++*clean == DEBOUNCE) {
        *clean = 0;

        byte key_code = KEYS[(i << 2) + x];


        // KEY IS NOW PRESSED
        if (bit1) {
          Keyboard.press(key_code);
          key_state[i] |= (1 << x);

          sprintf(buff, "%02X DOWN\n", key_code);
          Serial.write(buff);


        // KEY IS RELEASED
        } else {
          Keyboard.release(key_code);
          key_state[i] &= ~(1 << x);

          sprintf(buff, "%02X UP\n", key_code);
          Serial.write(buff);
        }
      }
    }

    // TOGGLE COL SELECT PULSE LINE
    delayWrite(KB_COL_SEL, (++col & 0x01));
  }

  // RESET COLUMN
  delayWrite(KB_COL_SEL,  LOW);

  // RESET ROW
  delayWrite(KB_RESET,    HIGH);
  delayWrite(KB_RESET,    LOW);
}
