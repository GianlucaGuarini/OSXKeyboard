package OSXKeyboard

/*
#cgo LDFLAGS: -framework Carbon
extern int listen();
*/
import "C"

import (
  "errors"
  "github.com/GianlucaGuarini/go-observable"
)

// flag variable to check whether the c code was already called
var isListening = false
var o = observable.New()

// Listen start listening the keyboard events
func Listen() {
  // start the c code listeners to call the go function hook
  if isListening == false {
    ok := C.listen()

    if ok == 1 {
      err := errors.New("It was not possible to listen the global keyboard events, make sure to call the progam with 'sudo'")
      panic(err)
    }

    // change the flag to true to avoid entering this condition again
    isListening = true
  }
}

// Subscribe a callback to the "keypress" events
func Subscribe(fn interface{}) {
  o.On("keypress", fn)
}

// Unsubscribe a function to the "keypress" events
func Unsubscribe(fn interface{}) {
  o.Off("keypress", fn)
}

// GoKeypressCallback hook function called in the c code
//export GoKeypressCallback
func GoKeypressCallback(key *C.char) {
  // get the letter just received
  letter := C.GoString(key)
  o.Trigger("keypress", letter)
}
