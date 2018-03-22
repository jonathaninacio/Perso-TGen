NAME := TGen


all : $(NAME);

$(NAME):
	gcc `SDL2-config --cflags --libs` main.c -o $(NAME)

clean:
	rm -rf $(NAME);

re: clean all
