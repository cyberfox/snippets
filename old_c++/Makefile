# On my Mac, I need: LDFLAGS=-L/usr/local/mysql/lib make sample
CXXFLAGS = -I/usr/local/mysql/include
LDFLAGS += # -L/usr/local/mysql/lib
LIB_SRC = DB.cpp SimpleLog.cpp Regex.cpp
LIB_OBJ = DB.o SimpleLog.o Regex.o
SRC = sample.cpp
OBJ = sample.o
OUT = sample
LIB = mrs_tools.a

$(LIB): $(LIB_OBJ)
	ar rcs $(LIB) $(LIB_OBJ)

# To compile with the library, you need something like
$(OUT): $(LIB) $(OBJ)
	g++ $(LDFLAGS) -o $(OUT) $(OBJ) $(LIB) -lmysqlclient -lpcre -lpcreposix

clean:
	rm $(OUT) $(LIB_OBJ) $(OBJ)
