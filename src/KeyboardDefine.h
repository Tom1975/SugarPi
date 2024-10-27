#pragma once
//

#ifdef  __circle__
#include <circle/usb/usbgamepad.h>
#else

#define MAX_GAMEPADS	2

#define BIT(x) (1 << (x))

enum TGamePadProperty
{
   GamePadPropertyIsKnown = BIT(0),	// has known mapping of controls
   GamePadPropertyHasLED = BIT(1),	// supports SetLEDMode()
   GamePadPropertyHasRGBLED = BIT(2),	// if set, GamePadPropertyHasLED is set too
   GamePadPropertyHasRumble = BIT(3),	// supports SetRumbleMode()
   GamePadPropertyHasGyroscope = BIT(4),	// provides sensor info in TGamePadState
   GamePadPropertyHasTouchpad = BIT(5),	// has touchpad with button
   GamePadPropertyHasAlternativeMapping = BIT(6)	// additional +/- buttons, no START button
};

// The following enums are valid for known gamepads only!

enum TGamePadButton		// Digital button (bit masks)
{
   GamePadButtonGuide = BIT(0),
#define GamePadButtonXbox	GamePadButtonGuide
#define GamePadButtonPS		GamePadButtonGuide
#define GamePadButtonHome	GamePadButtonGuide
   GamePadButtonLT = BIT(3),
#define GamePadButtonL2		GamePadButtonLT
#define GamePadButtonLZ		GamePadButtonLT
   GamePadButtonRT = BIT(4),
#define GamePadButtonR2		GamePadButtonRT
#define GamePadButtonRZ		GamePadButtonRT
   GamePadButtonLB = BIT(5),
#define GamePadButtonL1		GamePadButtonLB
#define GamePadButtonL		GamePadButtonLB
   GamePadButtonRB = BIT(6),
#define GamePadButtonR1		GamePadButtonRB
#define GamePadButtonR		GamePadButtonRB
   GamePadButtonY = BIT(7),
#define GamePadButtonTriangle	GamePadButtonY
   GamePadButtonB = BIT(8),
#define GamePadButtonCircle	GamePadButtonB
   GamePadButtonA = BIT(9),
#define GamePadButtonCross	GamePadButtonA
   GamePadButtonX = BIT(10),
#define GamePadButtonSquare	GamePadButtonX
   GamePadButtonSelect = BIT(11),
#define GamePadButtonBack	GamePadButtonSelect
#define GamePadButtonShare	GamePadButtonSelect
#define GamePadButtonCapture	GamePadButtonSelect
   GamePadButtonL3 = BIT(12),		// Left axis button
#define GamePadButtonSL		GamePadButtonL3
   GamePadButtonR3 = BIT(13),		// Right axis button
#define GamePadButtonSR		GamePadButtonR3
   GamePadButtonStart = BIT(14),		// optional
#define GamePadButtonOptions	GamePadButtonStart
   GamePadButtonUp = BIT(15),
   GamePadButtonRight = BIT(16),
   GamePadButtonDown = BIT(17),
   GamePadButtonLeft = BIT(18),
   GamePadButtonPlus = BIT(19),		// optional
   GamePadButtonMinus = BIT(20),		// optional
   GamePadButtonTouchpad = BIT(21)		// optional
};

// Number of digital buttons for known gamepads:
#define GAMEPAD_BUTTONS_STANDARD	19
#define GAMEPAD_BUTTONS_ALTERNATIVE	21
#define GAMEPAD_BUTTONS_WITH_TOUCHPAD	22

enum TGamePadAxis		// Axis or analog button
{
   GamePadAxisLeftX,
   GamePadAxisLeftY,
   GamePadAxisRightX,
   GamePadAxisRightY,
   GamePadAxisButtonLT,
#define GamePadAxisButtonL2	GamePadAxisButtonLT
   GamePadAxisButtonRT,
#define GamePadAxisButtonR2	GamePadAxisButtonRT
   GamePadAxisButtonUp,
   GamePadAxisButtonRight,
   GamePadAxisButtonDown,
   GamePadAxisButtonLeft,
   GamePadAxisButtonL1,
   GamePadAxisButtonR1,
   GamePadAxisButtonTriangle,
   GamePadAxisButtonCircle,
   GamePadAxisButtonCross,
   GamePadAxisButtonSquare,
   GamePadAxisUnknown
};

enum TGamePadLEDMode
{
   GamePadLEDModeOff,
   GamePadLEDModeOn1,
   GamePadLEDModeOn2,
   GamePadLEDModeOn3,
   GamePadLEDModeOn4,
   GamePadLEDModeOn5,
   GamePadLEDModeOn6,
   GamePadLEDModeOn7,
   GamePadLEDModeOn8,
   GamePadLEDModeOn9,
   GamePadLEDModeOn10,
   GamePadLEDModeUnknown
};

enum TGamePadRumbleMode
{
   GamePadRumbleModeOff,
   GamePadRumbleModeLow,
   GamePadRumbleModeHigh,
   GamePadRumbleModeUnknown
};

#define MAX_AXIS    16
#define MAX_HATS    6

struct TGamePadState		/// Current state of a gamepad
{
   int naxes;		// Number of available axes and analog buttons
   struct
   {
      int value;	// Current position value
      int minimum;	// Minimum position value (normally 0)
      int maximum;	// Maximum position value (normally 255)
   }
   axes[MAX_AXIS];		// Array of axes and analog buttons

   int nhats;		// Number of available hat controls
   int hats[MAX_HATS];	// Current position value of hat controls

   int nbuttons;		// Number of available digital buttons
   unsigned buttons;	// Current status of digital buttons (bit mask)

   int acceleration[3];	// Current state of acceleration sensor (x, y, z)
   int gyroscope[3];	// Current state of gyroscope sensor (x, y, z)
};
#endif