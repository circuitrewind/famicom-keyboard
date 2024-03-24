

////////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES
////////////////////////////////////////////////////////////////////////////////
#include <Keyboard.h>




////////////////////////////////////////////////////////////////////////////////
// CONFIG ALL THE THINGS
////////////////////////////////////////////////////////////////////////////////
#define KB_RESET    (0x01)
#define KB_ENABLE   (0x02)
#define KB_COL_SEL  (0x03)

#define KB_C2_D1    (0x05)
#define KB_C2_D2    (0x06)
#define KB_C2_D3    (0x07)
#define KB_C2_D4    (0x08)

#define COLUMNS     (18)

#define DEBOUNCE    (10)




////////////////////////////////////////////////////////////////////////////////
// KEY CODE STATUS
////////////////////////////////////////////////////////////////////////////////
byte key_state[COLUMNS];
byte key_clean[COLUMNS*4];




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
  KEY_RIGHT_ALT,    // KANA
  KEY_RIGHT_SHIFT,
  '\\',             // ¥
  KEY_BACKSPACE,    // STOP

  // COL 03
  KEY_F7,
  '\'',             // @
  '\'',             // :
  ';',              // ;

  // COL 04
  '_',
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
// SLOW DOWN CLOCK PULSES TO NOT OVERWHELM THE YE OLD FAMICOM KEYBOARD!
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
  for (auto i=0; i<COLUMNS*4; i++) {
    key_clean[i] = 0;
  }

  // SETUP DATA INPUT REGISTERS
  pinMode(KB_C2_D1,       INPUT);
  pinMode(KB_C2_D2,       INPUT);
  pinMode(KB_C2_D3,       INPUT);
  pinMode(KB_C2_D4,       INPUT);

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

  for (auto i=0; i<COLUMNS; i++) {
    new_state = digitalRead(KB_C2_D1) << 0
              | digitalRead(KB_C2_D2) << 1
              | digitalRead(KB_C2_D3) << 2
              | digitalRead(KB_C2_D4) << 3;

    // PROCESS KEY INPUT
    for (auto x=0; x<4; x++) {
      byte *clean = &key_clean[(i * 4) + x];
      byte bit1   = (new_state    >> x) & 0x01;
      byte bit2   = (key_state[i] >> x) & 0x01;

      // KEY IS UNCHANGED, DO NOTHING
      if (bit1 == bit2) {
        *clean = 0;

      // KEY IS CHANGED,
      // DO SOMETHING IF CONSISTENT FOR (DEBOUNCE) "TICKS"
      // THIS THRESHOLD HANDLES THE "DEBOUNCE" MECHANIC
      } else if (++*clean >= DEBOUNCE) {
        *clean = 0;

        byte key_code = KEYS[(i << 2) + x];

        // KEY IS NOW PRESSED
        if (bit1) {
          Keyboard.press(key_code);
          key_state[i] |= (1 << x);
            
        // KEY IS RELEASED
        } else {
          Keyboard.release(key_code);
          key_state[i] &= ~(1 << x);
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
