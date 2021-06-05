GCC=gcc -Wall
FILES=main.c copy_list.c copy_list.h

all: ${FILES}
	@${GCC} ${FILES} -o exec
	@-./exec 2>/dev/null

clean:
	@rm exec