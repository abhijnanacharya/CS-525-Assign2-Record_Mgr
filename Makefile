CFLAGS = -g -Wall -Wextra -Wpedantic -Wno-unused-parameter

.PHONY: all
all: test_assign1

test_assign1: test_assign3_1.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c buffer_mgr_helper.c expr.c record_mgr.c rm_serializer.c
	$(CC) $(CFLAGS) -o test_assign1 test_assign3_1.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c buffer_mgr_helper.c expr.c record_mgr.c rm_serializer.c

.PHONY: clean
clean:
	rm -f test_assign1 *.o result.txt