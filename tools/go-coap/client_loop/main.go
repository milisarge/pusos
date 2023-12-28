package main

import (
	"context"
	"log"
	"os"
	"time"
	"unsafe"
	"math"
	"encoding/binary"
	"fmt"
	"strconv"
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
	
	FS := 0
	DELAY := 100
	COUNT := 100
	
	if _, mevcut := os.LookupEnv("fs"); mevcut == true {
	  FS, _ = strconv.Atoi(os.Getenv("fs"))
	}
	if _, mevcut := os.LookupEnv("delay"); mevcut == true {
	  DELAY, _ = strconv.Atoi(os.Getenv("delay"))
	}
	if _, mevcut := os.LookupEnv("count"); mevcut == true {
	  COUNT, _ = strconv.Atoi(os.Getenv("count"))
	}
	
	for i := 0 ; i < COUNT ; i++ {
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
			fmt.Printf("response: %v\n", Float32frombytes(resp_data) )
			
			//dosya yazma
			if FS == 1 {
				f, err := os.OpenFile("sensor.log",
				os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
				if err != nil {
					log.Println(err)
				}
				defer f.Close()
				if _, err := f.WriteString(fmt.Sprintf("%f", Float32frombytes(resp_data))+"\n"); err != nil {
					log.Println(err)
				}
			}
		} 
		time.Sleep(time.Duration(DELAY) * time.Millisecond)
	}
}
