cmd_/home/debian/ds1307-driver/my_ds1307.ko := ld -EL -r  -T ./scripts/module-common.lds --build-id  -o /home/debian/ds1307-driver/my_ds1307.ko /home/debian/ds1307-driver/my_ds1307.o /home/debian/ds1307-driver/my_ds1307.mod.o