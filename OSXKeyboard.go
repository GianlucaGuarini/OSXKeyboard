package OSXKeyboard

/*
#cgo LDFLAGS: -framework ApplicationServices -framework Carbon
extern int listen();
*/
import "C"

import (
  "errors"
)

var (
  // flag variable to check whether the c code was already called
  isListening = false
  // Channels list of all the channels where the letters will be streamed
  Channels = make(map[int]*chan string)
)

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

// Subscribe add a channel to the Channels list
func Subscribe(ch *chan string) {
  // add che channel to the list
  Channels[len(Channels)] = ch
}

// GoKeypressCallback hook function called in the c code
//export GoKeypressCallback
func GoKeypressCallback(key *C.char) {
  // get the letter just received
  letter := C.GoString(key)

  for i := 0; i < len(Channels); i++ {
    *Channels[i] <- letter
  }

}
