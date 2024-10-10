.PHONY:

USRLIB := /usr/lib

CUR_DIR := .
INC_DIR := $(CUR_DIR)/inc
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
BIN_DIR := $(CUR_DIR)/bin
LIBS_DIR := $(CUR_DIR)/libs

LIBS_STATIC_DIR := $(LIBS_DIR)/static
LIBS_SHARED_DIR := $(LIBS_DIR)/shared

LIB_NAME := hello

#CREATE SHARED LIB
# B1 Create the object files
shared_objs:
	gcc -c -o $(OBJ_DIR)/main.o main.c -I $(INC_DIR)
	gcc -c -fPIC -o $(OBJ_DIR)/hello_minh.o $(SRC_DIR)/hello_minh.c -I $(INC_DIR)
	gcc -c -fPIC -o $(OBJ_DIR)/hello_world.o $(SRC_DIR)/hello_world.c -I $(INC_DIR)

# B2 Create .So Library
create_shared_lib: 
	gcc -shared $(OBJ_DIR)/hello_minh.o $(OBJ_DIR)/hello_world.o -o $(LIBS_SHARED_DIR)/lib$(LIB_NAME).so


# B3 Linking file main.o with .so library
#-L$(LIBS_SHARED_DIR) : Lib address
#-l$(LIB_NAME) : Lib name
link_share_lib: 
	gcc $(OBJ_DIR)/main.o -L$(LIBS_SHARED_DIR) -l$(LIB_NAME) -o $(BIN_DIR)/use_shared_lib

# B4 Install shared lib into user lib ( systerm library ) 
#sudo make install_shared_lib
install_shared_lib:
	install $(LIBS_SHARED_DIR)/lib$(LIB_NAME).so $(USRLIB)

#B5 run binary file in bin folder 
#CREATE STATIC LIB

shared_all: shared_objs create_shared_lib link_share_lib install_shared_lib
	@echo "Make All Target"

#B6: Run in terminal but not installing share lib into usr/lib
#LD_LIBRARY_PATH=$(pwd)/libs/shared bin/use_shared_library
dontinstall: shared_objs create_shared_lib link_share_lib
	@echo "Make All Target But Don't Install In USR LIB"

#CREATE STATIC LIB
#B1
static_objs:
	gcc -c -o $(OBJ_DIR)/main.o main.c -I $(INC_DIR)
	gcc -c -o $(OBJ_DIR)/hello_minh.o $(SRC_DIR)/hello_minh.c -I $(INC_DIR)
	gcc -c -o $(OBJ_DIR)/hello_world.o $(SRC_DIR)/hello_world.c -I $(INC_DIR)

# B2 Create .a Library
create_static_libs: 
	ar rcs $(LIBS_STATIC_DIR)/lib$(LIB_NAME).a $(OBJ_DIR)/hello_minh.o $(OBJ_DIR)/hello_world.o

# B3 Linking file main.o with .a library
link_static_lib:
	gcc   $(OBJ_DIR)/main.o  -L$(LIBS_STATIC_DIR) -l$(LIB_NAME) -o $(BIN_DIR)/statically-linked

static_all: static_objs create_static_libs link_static_lib
	@echo "Make All Target For Static Libs"

clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(LIBS_SHARED_DIR)/lib$(LIB_NAME).so
	rm -rf $(BIN_DIR)/use_shared_lib
	rm -rf $(USRLIB)/lib$(LIB_NAME).so
	rm -rf $(LIBS_STATIC_DIR)/lib$(LIB_NAME).a
	rm -rf $(BIN_DIR)/statically-linked