# OSXKeyboard
Go helper to detect the global keypress events on OSX, it requires the root permissions enabled

# Installation

```bash
go get github.com/GianlucaGuarini/OSXKeyboard
```

# API

## Listen

Call it once to start listening the global keyboard events

## Subscribe(interface {})

Subscribe to the keypress events receiving the letter pressed as string

## Unsubscribe(interface {})

Unsubscribe a specific callback to the keypress events

# Usage

Build the following file from your `go/src` folder and then run it with `sudo`

```go
package main

import (
  "fmt"
  "github.com/GianlucaGuarini/OSXKeyboard"
)

func main() {
  go OSXKeyboard.Listen()

  OSXKeyboard.Subscribe(func(letter string) {
    fmt.Println(letter)
  })

  select {} // block forever
}
```
