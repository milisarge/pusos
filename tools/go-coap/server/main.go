package main

import (
	"bytes"
	"os"
	"log"
	"math/rand"
	"encoding/binary"
	"unsafe"
	coap "github.com/plgd-dev/go-coap/v3"
	"github.com/plgd-dev/go-coap/v3/message"
	"github.com/plgd-dev/go-coap/v3/message/codes"
	"github.com/plgd-dev/go-coap/v3/mux"
)

func IntToByteArray(num int) []byte {
    size := int(unsafe.Sizeof(num))
    arr := make([]byte, size)
    for i := 0 ; i < size ; i++ {
        byt := *(*uint8)(unsafe.Pointer(uintptr(unsafe.Pointer(&num)) + uintptr(i)))
        arr[i] = byt
    }
    return arr
}

func Float32ToByte(f float32) []byte {
    var buf bytes.Buffer
    err := binary.Write(&buf, binary.LittleEndian, f)
    if err != nil {
        log.Printf("binary.Write failed:", err)
    }
    //log.Printf("len %d \n", len(buf.Bytes()))
    return buf.Bytes()
}

func randFloats(min, max float32) float32 {
   	//s := rand.NewSource(time.Now().Unix())
    	//r := rand.New(s)	
	//var res float64
	res := min + rand.Float32() * (max - min)
	log.Printf("generated value %f \n", res)
	return res
}

func loggingMiddleware(next mux.Handler) mux.Handler {
	return mux.HandlerFunc(func(w mux.ResponseWriter, r *mux.Message) {
		log.Printf("ClientAddress %v, %v\n", w.Conn().RemoteAddr(), r.String())
		next.ServeCOAP(w, r)
	})
}

func handleA(w mux.ResponseWriter, r *mux.Message) {
	err := w.SetResponse(codes.Content, message.TextPlain, bytes.NewReader([]byte( Float32ToByte(randFloats(1.0,99.99)) )))
	if err != nil {
		log.Printf("cannot set response: %v", err)
	}
}

func handleB(w mux.ResponseWriter, r *mux.Message) {
	min := 35
	max := 75
	// ideal home humidity 40-70
	res := rand.Intn(max - min) + min
	log.Printf("generated value %v \n", res)
	err := w.SetResponse(codes.Content, message.TextPlain, bytes.NewReader( IntToByteArray(res) ))
	if err != nil {
		log.Printf("cannot set response: %v", err)
	}
}

func main() {
	r := mux.NewRouter()
	r.Use(loggingMiddleware)
	r.Handle("/temp", mux.HandlerFunc(handleA))
	r.Handle("/humidity", mux.HandlerFunc(handleB))
	
	port := "5683"
	if len(os.Args) > 1 {
		port = os.Args[1]
	}
	
	log.Fatal(coap.ListenAndServe("udp", ":"+port, r))
}
