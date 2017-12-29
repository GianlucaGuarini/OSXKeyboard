#include <Carbon/Carbon.h>
#include "_cgo_export.h"

// detect the special keys
char * specialChar(CGKeyCode keycode) {
  switch (keycode) {
    case 36: return "RETURN";
    case 48: return "TAB";
    case 71: return "CLEAR";
    case 49: return "SPACE";
    case 51: return "DELETE";
    case 52: return "ENTER";
    case 53: return "ESCAPE";
    case 96: return "F5";
    case 97: return "F6";
    case 98: return "F7";
    case 99: return "F3";
    case 100: return "F8";
    case 101: return "F9";
    case 103: return "F11";
    case 105: return "F13";
    case 107: return "F14";
    case 109: return "F10";
    case 111: return "F12";
    case 113: return "F15";
    case 114: return "HELP";
    case 115: return "HOME";
    case 116: return "PGUP";
    case 117: return "DELETE";
    case 118: return "F4";
    case 119: return "END";
    case 120: return "F2";
    case 121: return "PGDN";
    case 122: return "F1";
    case 123: return "LEFT";
    case 124: return "RIGHT";
    case 125: return "DOWN";
    case 126: return "UP";
    default: return NULL;
  }
}

/**
 * Callback function called whenever there is a keypress event
 */
CGEventRef onKeyPress(
    CGEventRef proxy,
    CGEventType type,
    CGEventRef event,
    void *refcon
) {

  // allow only the keydown key upevents
  // skip the others
  if (type != kCGEventKeyDown) return event;

  // get the incoming keycode.
  CGKeyCode keycode = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

  // check if it's a special key
  char * utf8Str = specialChar(keycode);

  // get eventually the key pressed
  if (utf8Str == NULL) {
    UniChar chars[1];
    UniCharCount maxStringLength = sizeof(chars);
    UniCharCount realLength;

    CGEventKeyboardGetUnicodeString(event, maxStringLength, &realLength, &chars[0]);
    utf8Str = (char *) chars;
  }

  // go callback hook
  GoKeypressCallback(utf8Str);

  return event;
}

// this function will be eventually called in go
int listen() {
  CFMachPortRef      eventTap;
  CGEventMask        eventMask;
  CFRunLoopSourceRef runLoopSource;

  // Create an event tap. We are interested in key presses.
  eventMask = (1 << kCGEventKeyDown) | (1 << kCGEventKeyUp);

  // test if we have the permissions to detect the keypress events
  eventTap = CGEventTapCreate(
    kCGSessionEventTap,
    kCGHeadInsertEventTap,
    0,
    eventMask,
    (CGEventTapCallBack) onKeyPress,
    NULL
  );

  if (!eventTap) return 1;

  // Create a run loop source.
  runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);

  // Add to the current run loop.
  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

  // Enable the event tap.
  CGEventTapEnable(eventTap, true);

  // Set it all running.
  CFRunLoopRun();

  return 0;
}