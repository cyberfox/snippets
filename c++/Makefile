# On my Mac, I need: LDFLAGS=-L/usr/local/mysql/lib make sample
CXXFLAGS = -I/usr/local/mysql/include
LDFLAGS += `mysql_config --libs`
LIB_SRC = DB.cpp SimpleLog.cpp Regex.cpp Config.cpp
LIB_OBJ = DB.o SimpleLog.o Regex.o Config.o
SRC = sample.cpp
OBJ = sample.o
OUT = sample
LIB = mrs_tools.a

$(LIB): $(LIB_OBJ)
	ar rcs $(LIB) $(LIB_OBJ)

# To compile with the library, you need something like
$(OUT): $(LIB) $(OBJ)
	g++ $(LDFLAGS) -o $(OUT) $(OBJ) $(LIB) -lmysqlclient -lpcre -lpcreposix

regtest: $(LIB) RegTest.o
	g++ $(LDFLAGS) -o regtest RegTest.o $(LIB) -lpcre -lpcreposix

urltest: $(LIB) URLTest.o
	g++ $(LDFLAGS) -o urltest URLTest.o $(LIB)

tests: sample regtest urltest

clean:
	rm -f $(OUT) $(LIB) $(LIB_OBJ) $(OBJ) regtest urltest URLTest.o RegTest.o
