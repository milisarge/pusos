package main

import (
	"context"
	"log"
	"os"
	"time"
	"unsafe"
	"math"
	"encoding/binary"

	"github.com/plgd-dev/go-coap/v3/udp"
)

func ByteArrayToInt(arr []byte) int{
    val := int(0)
    size := len(arr)
    for i := 0 ; i < size ; i++ {
        *(*uint8)(unsafe.Pointer(uintptr(unsafe.Pointer(&val)) + uintptr(i))) = arr[i]
    }
    return val
}

func Float32frombytes(bytes []byte) float32 {
    bits := binary.LittleEndian.Uint32(bytes)
    float := math.Float32frombits(bits)
    return float
}

func main() {
	addr := "localhost:5683"
	if len(os.Args) > 1 {
		addr = os.Args[1]
	}
	
	co, err := udp.Dial(addr)
	if err != nil {
		log.Fatalf("Error dialing: %v", err)
	}
	
	path := "/temp"
	if len(os.Args) > 2 {
		path = os.Args[2]
	}

	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()
	resp, err := co.Get(ctx, path)
	if err != nil {
		log.Fatalf("Error sending request: %v", err)
	}
	resp_data, _ := resp.ReadBody()
	if path == "/humidity" {
		log.Printf("Server Response: %v", ByteArrayToInt(resp_data) )
	}
	if path == "/temp" {
		log.Printf("Server Response: %v", Float32frombytes(resp_data) )
	} 
}
