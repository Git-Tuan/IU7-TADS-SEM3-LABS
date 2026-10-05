COMPILER_NAME := gcc
CFLAGS := -std=c99 -Wall -Werror -Wextra -Wpedantic -Iinc

SRCS := src/main.c src/table_core.c src/table_io.c src/table_utils.c src/errors.c src/file_utils.c src/measure_sorts.c src/sorts.c
OBJS := $(SRCS:src/%.c=out/%.o)
DEPS := $(SRCS:src/%.c=out/%.d)

ifeq ($(m), release)
	CFLAGS += -O2
else
	CFLAGS += -O0 -g
endif

app.exe: $(OBJS) | out
	$(COMPILER_NAME) $(CFLAGS) -o $@ $^

out/%.o: src/%.c | out
	$(COMPILER_NAME) $(CFLAGS) -c $< -o $@

out/%.d: src/%.c | out
	$(COMPILER_NAME) $(CFLAGS) -MM $< > $@

out:
	@mkdir -p out

-include $(DEPS)

clean:
	rm -rf out
	rm -f *.exe *.gcov

