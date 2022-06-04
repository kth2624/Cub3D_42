NAME = cub3d

SRCS_FILES = 01.c

SRCS_OBJS = ${SRCS_FILES:.c=.o}

MLX_DIR = ../mlx/

CC = gcc

RM = rm -f

CFLAG = -Wall -Wextra -Werror

MLX =  -L./mlx -lmlx -framework OpenGL -framework AppKit

${NAME}: ${SRCS_OBJS}
	make -C ${MLX_DIR}
	${CC} -o ${NAME} ${SRCS_OBJS} ${MLX}

all : ${NAME}

clean :
	${RM} ${SRCS_OBJS}

fclean : clean
	${RM} ${NAME}

re : fclean all

.PHONY : all clean fclean re