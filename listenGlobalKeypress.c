#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>
#include "_cgo_export.h"

// global constants
const UniChar *chars;

// return the CFStringRef from the keycode pressed
CFStringRef CFStringFromCGKeyCode(CGKeyCode keyCode) {
    // get the keyboard layout
    TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardLayoutInputSource();
    CFDataRef layoutData = TISGetInputSourceProperty(
                            currentKeyboard,
                            kTISPropertyUnicodeKeyLayoutData
                           );

    const UCKeyboardLayout *keyboardLayout = (
                                              const UCKeyboardLayout *)
                                              CFDataGetBytePtr(layoutData
                                             );
    // helper variables
    UInt32 keysDown = 0;
    UniChar chars[4];
    UniCharCount realLength;

    UCKeyTranslate(
      keyboardLayout,
      keyCode,
      kUCKeyActionDisplay,
      0,
      LMGetKbdType(),
      kUCKeyTranslateNoDeadKeysBit,
      &keysDown,
      sizeof(chars) / sizeof(chars[0]),
      &realLength,
      chars
    );

    // free the memory allocated for the currentKeyboard variable
    CFRelease(currentKeyboard);

    return CFStringCreateWithCharacters(kCFAllocatorDefault, chars, 1);
}

char * charCopyFromCFString(CFStringRef aString) {

  // make sure to receive always a valid string
  if (aString == NULL)
    return NULL;

  // get the string properties
  CFIndex length = CFStringGetLength(aString);
  CFIndex maxSize = CFStringGetMaximumSizeForEncoding(
                      length,
                      kCFStringEncodingUTF8
                    ) + 1;

  // allocate memory to store the char
  // it will be freed afterwards in the onKeyPress function
  char *buffer = (char *)malloc(maxSize);

  // return the string pointer
  if (CFStringGetCString(aString, buffer, maxSize, kCFStringEncodingUTF8))
    return buffer;

  return NULL;
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
  if ((type != kCGEventKeyDown) && (type != kCGEventKeyUp))
    return event;


  // The incoming keycode.
  CGKeyCode keycode = (CGKeyCode) CGEventGetIntegerValueField(
                                    event,
                                    kCGKeyboardEventKeycode
                                  );

  CFStringRef str = CFStringFromCGKeyCode(keycode);
  char * utf8Str = charCopyFromCFString(str);

  // go callback hook
  GoKeypressCallback(utf8Str);

  // free the memory allocated to store this pointer
  free(utf8Str);

  return event;

}

// this function will be called in go
int listen() {
  CFMachPortRef      eventTap;
  CGEventMask        eventMask;
  CFRunLoopSourceRef runLoopSource;

  // Create an event tap. We are interested in key presses.
  eventMask = ((1 << kCGEventKeyDown) | (1 << kCGEventKeyUp));

  // test if we have the permissions to detect the keypress events
  eventTap = CGEventTapCreate(
                kCGSessionEventTap,
                kCGHeadInsertEventTap,
                0,
                eventMask,
                onKeyPress,
                NULL
              );

  if (!eventTap)
    return 1;

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