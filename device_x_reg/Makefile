CC = gcc
OUTPUT_DIR = build

CORE_OBJ = \
		   $(OUTPUT_DIR)/reg_desc_tbl.o \

TBL_PLUGGINS_OBJ = \
		   $(OUTPUT_DIR)/device_x_reg_desc_tbl.o \

APP1 = app_device_x_reg_test
APP1_OBJ = \
		   $(OUTPUT_DIR)/app_device_x_reg_test.o \

all: $(OUTPUT_DIR)/$(APP1)_no_tbl $(OUTPUT_DIR)/$(APP1)_tbl

$(OUTPUT_DIR)/$(APP1)_no_tbl: $(CORE_OBJ) $(APP1_OBJ)
	$(CC) -o $@ $^

$(OUTPUT_DIR)/$(APP1)_tbl: $(CORE_OBJ) $(APP1_OBJ) $(TBL_PLUGGINS_OBJ)
	$(CC) -o $@ $^

$(OUTPUT_DIR)/%.o: %.c $(OUTPUT_DIR)
	$(CC) -c $< -o $@

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)

dbg:
	gcc -E -o ./$(OUTPUT_DIR)/device_x_reg.h.E device_x_reg.h
	gcc -E -o ./$(OUTPUT_DIR)/device_x_reg_desc_tbl.c.E device_x_reg_desc_tbl.c

clean:
	rm -f ./$(OUTPUT_DIR)/*

.PHONY : clean
