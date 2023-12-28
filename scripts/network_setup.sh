#!/bin/sh
# yetkili çalıştır.

#[ $UID -ne 0 ] && echo "run as root!!!" && exit 1

c=10

for num in {1..$c}
do
  echo $num
done

exit 1
# tap ağı libvirt ağı üzerinden mevcut libvirt ağına bağlanma - yetkili
# tuntap aygıt ekleme
ip tuntap add dev tp1 mode tap user $1

# sanal köprü kurup tuntap tp1..n bu köprü ağa bağlanır.
# libvirt için kullanılan servis kullanılarak oto köprü oluşturulur.
# bu servisin başladığı varsayılır.
#[ -f /usr/milis/bin/servis ] && servis start libvirtd

# virbr0 ağyüzünün oluştuğuna emin olunur.
brctl addif virbr0 tp1

# ilkleme yapılır.
ifconfig tp1 0.0.0.0
