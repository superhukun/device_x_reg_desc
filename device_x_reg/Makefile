CC = gcc
OUTPUT_DIR = build

CORE_OBJ = \
		   $(OUTPUT_DIR)/reg_desc_tbl.o \

TBL_PLUGGINS_OBJ = \
		   $(OUTPUT_DIR)/device_x_reg_desc_tbl.o \

APP_OBJ = \
		   $(OUTPUT_DIR)/device_x_reg.o \

all: $(OUTPUT_DIR)/reg_parse_no_tbl $(OUTPUT_DIR)/reg_parse_with_tbl

$(OUTPUT_DIR)/reg_parse_no_tbl: $(CORE_OBJ) $(APP_OBJ)
	$(CC) -o $@ $^

$(OUTPUT_DIR)/reg_parse_with_tbl: $(CORE_OBJ) $(APP_OBJ) $(TBL_PLUGGINS_OBJ)
	$(CC) -o $@ $^

$(OUTPUT_DIR)/%.o: %.c $(OUTPUT_DIR)
	$(CC) -c $< -o $@

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)

E:
	gcc -E -o device_x_reg.E device_x_reg.c
clean:
	rm -i $(OUTPUT_DIR)/*

.PHONY : clean
