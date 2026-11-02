COMPILER_NAME := gcc
CFLAGS := -std=c99 -Wall -Werror -Wextra -Wpedantic -Iinc

SRCS := src/main.c src/benchmark.c src/io.c src/utils.c src/csc.c src/csr.c src/errors.c src/dense.c src/app_context.c src/sparse_ops.c src/ui.c
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

style:
	clang-format -i -style=Microsoft src/*.c
	clang-format -i -style=Microsoft inc/*.h

clean:
	rm -rf out
	rm -f *.exe *.gcov
