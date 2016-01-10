# OSXKeyboard
Go helper to detect the global keypress events on OSX, it requires the root permissions enabled

# Installation

```bash
go get github.com/GianlucaGuarini/OSXKeyboard
```

# API

## Listen

Call it once to start listening the global keyboard events

## Subscribe(ch *chan string)

Subscribe a channel receiving the keyboard letters on any keypress

# Usage

Build the following file from your `go/src` folder and then run it with `sudo`

```go

package main

import (
  "fmt"
  "github.com/GianlucaGuarini/OSXKeyboard"
  "sync"
)

func main() {

  var wg sync.WaitGroup
  ch := make(chan string)

  OSXKeyboard.Subscribe(&ch)
  go OSXKeyboard.Listen()

  for {
    select {
    case letter, _ := <-ch:
      fmt.Println(letter)
    }
  }

  wg.Wait()

}


```
