# On my Mac, I need: LDFLAGS=-L/usr/local/mysql/lib make sample
OBJS = Tableau.o TableauTest.o
OUT = tableau

test: $(OUT)
	./tableau

$(OUT): $(OBJS)
	g++ -o $(OUT) $(OBJS)

clean:
	rm -f $(OUT) $(OBJS)
