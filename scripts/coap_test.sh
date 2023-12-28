export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/work/libcoap/.libs
#coap-client -m get coap://[::1]/
#coap-client -m get coap://[::1]/.well-known/core
#coap-client -m get coap+tcp://[::1]/.well-known/core
#coap-client -m get coaps://[::1]/.well-known/core
#coap-client -m get coaps+tcp://[::1]/.well-known/core
#coap-client -m get -T cafe coap://[::1]/time
#echo -n 1000 | coap-client -m put -T cafe coap://[::1]/time -f -
coap-client -m get coap://192.168.122.66:5699/time
