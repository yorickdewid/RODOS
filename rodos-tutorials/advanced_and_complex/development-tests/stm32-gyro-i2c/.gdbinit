file tst

target remote localhost:3333

#monitor halt

monitor reset halt
break main
cont
