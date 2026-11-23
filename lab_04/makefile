CC := gcc
FLAGS := -std=c99 -Wall -Werror -Wextra -Wpedantic -Iinc -O2
SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=out/%.o)
DEPS := $(SRCS:src/%.c=out/%.d)

.PHONY: clean out

app.exe: $(OBJS) | out
	$(CC) $(FLAGS) -o $@ $^

$(DEPS): out/%.d : src/%.c | out
	$(CC) -c $(FLAGS) -MM $^ > $@

$(OBJS): out/%.o : src/%.c | out
	$(CC) -c $(FLAGS) -o $@ $^

-include $(DEPS)

out: 
	@mkdir -p out

clean:
	rm -rf *.exe out