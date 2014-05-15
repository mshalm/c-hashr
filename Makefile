EXENAME = md5

COMPILER = gcc
WARNINGS = -Wchar-subscripts -Wparentheses -Wreturn-type -Wmissing-braces -Wundef -Wshadow
COMPILER_OPTS = -c -g -O0 -Wfatal-errors -Werror $(WARNINGS)
LINKER = gcc

MAIN_OBJS = main.o md5.o string_handler.o http_interface.o
MAIN_DEPS = md5.h md5.c string_handler.h string_handler.c http_interface.h http_interface.c


CODE_CLN = *.o $(EXENAME)


all: md5

md5: $(MAIN_OBJS)
	$(LINKER) $(MAIN_OBJS) -lpthread -lcurl -ljson -o md5

main.o : main.c md5.h string_handler.h http_interface.h 
	$(COMPILER) $(COMPILER_OPTS) -lpthread -lcurl -ljson main.c

md5.o : md5.c md5.h
	$(COMPILER) $(COMPILER_OPTS) -lpthread -lcurl -ljson md5.c
	
string_handler.o : string_handler.c string_handler.h
	$(COMPILER) $(COMPILER_OPTS) -lpthread -lcurl -ljson string_handler.c
	
http_interface.o : http_interface.c http_interface.h
	$(COMPILER) $(COMPILER_OPTS) -lpthread -lcurl -ljson http_interface.c
	
clean:
	-rm -f $(CODE_CLN) 
 
