CC ?= gcc
CFLAGS_common ?= -Wall -std=gnu99 -g
CFLAGS_orig = -O0 
CFLAGS_opt  = -O0 

EXEC = phonebook_orig phonebook_opt phonebook_fuzzy

GIT_HOOKS := .git/hooks/applied
.PHONY: all
all: $(GIT_HOOKS) $(EXEC)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

SRCS_common = main.c mpool.c

phonebook_orig: $(SRCS_common) phonebook_orig.c phonebook_orig.h 
	$(CC) $(CFLAGS_common) $(CFLAGS_orig) \
		-DIMPL="\"$@.h\"" -o $@ \
		$(SRCS_common) $@.c

phonebook_opt: $(SRCS_common) phonebook_opt.c phonebook_opt.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt) \
		-DIMPL="\"$@.h\"" -DOPT -o $@ \
		$(SRCS_common) $@.c

phonebook_fuzzy: $(SRCS_common) phonebook_fuzzy.c phonebook_fuzzy.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt) \
		-DIMPL="\"$@.h\"" -o $@ \
		$(SRCS_common) $@.c

run: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_orig && echo 3 | sudo tee /proc/sys/vm/drop_caches"

cache-test: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	$(RM) orig.txt opt.txt output.txt
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./phonebook_orig
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./phonebook_opt

output.txt: cache-test calculate
	./calculate

plot: output.txt
	gnuplot scripts/runtime.gp

calculate: calculate.c
	$(CC) $(CFLAGS_common) $^ -o $@


.PHONY: clean style
clean:
	$(RM) $(EXEC) *.o perf.* \
	      	calculate orig.txt opt.txt output.txt runtime.png

style:
	astyle --style=kr --indent=spaces=4 --suffix=none *.c
	astyle --style=kr --indent=spaces=4 --suffix=none main.c