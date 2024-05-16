A: C
	gcc -o device_x_reg device_x_reg.o reg_desc_tbl.o
C:
	gcc -c reg_desc_tbl.c
	gcc -c device_x_reg.c
E:
	gcc -E -o device_x_reg.E device_x_reg.c
clean:
	rm -i *.E *.o device_x_reg
