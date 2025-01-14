# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tnakas <tnakas@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/08/27 11:32:04 by sganiev           #+#    #+#              #
#    Updated: 2024/10/19 00:19:36 by tnakas           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			:= cub3D

LIBFT_DIR 		:= libft
INCDIRS			:= -I./include/ -I./mlx42/include -I$(LIBFT_DIR)

CC				:= gcc
CFLAGS			:= -g -Wall -Wextra -Werror $(INCDIRS)

VPATH			:=  ./src/main ./src/parser ./src/ray_caster ./src/renderer

SRC_MAIN		:= main.c

SRC_PARSER		:= p_cub_file.c p_map.c p_nsew.c parser.c \
				   p_cub_file_utils.c p_texture.c \
				   p_texture_utils_one.c p_texture_utils_two.c \
				   p_texture_utils_three.c p_map_utils_one.c \
				   p_map_utils_two.c p_cub_file_name_validation.c \
				   p_map_colors_connection.c \

SRC_RAY_CASTER	:= boundary_checks.c convert.c direction_checks.c \
				   horizontal_intersec.c move_intersec_point.c \
				   player_orientation.c ray_caster_init.c ray_caster.c \
				   ray_len.c sector_checks.c step_inside_grid.c \
				   vertical_intersec.c wall_projection.c \

SRC_RENDERER	:= draw_elements.c events.c handle_textures.c \
				   move_keys.c renderer.c \

SRC				:= $(SRC_MAIN) $(SRC_PARSER) $(SRC_RAY_CASTER) $(SRC_RENDERER)

BUILDDIR		:= ./build
ODIR			:= $(BUILDDIR)/obj
DDIR			:= $(BUILDDIR)/deps
OBJ				:= $(patsubst %.c,$(ODIR)/%.o,$(SRC))
LIBFT 			:= $(LIBFT_DIR)/libft.a
DEPFILES		:= $(patsubst %.c,$(DDIR)/%.d,$(SRC))
DEPFLAGS		=  -MMD -MP -MF $(DDIR)/$*.d

#****************************************************************************#
#                                   MLX42                                    #
#****************************************************************************#
MLXBUILDDIR		:= ./mlx42/build
MLX42LIB		:= $(MLXBUILDDIR)/libmlx42.a
MLX42FLAGS		:= -lglfw -framework Cocoa -framework OpenGL -framework IOKit
#for Ubuntu:
# MLX42FLAGS		:= -lglfw -ldl -lGL

#****************************************************************************#
#                                   COLORS                                   #
#****************************************************************************#
RED				:= \033[0;31m
GREEN			:= \033[0;32m
YELLOW			:= \033[1;33m
BLUE			:= \033[0;34m
RESET			:= \033[0m

#****************************************************************************#
#                                   RULES                                    #
#****************************************************************************#
all: $(BUILDDIR) $(NAME)

$(NAME): $(MLX42LIB) $(OBJ) $(LIBFT)
	@echo "$(BLUE)Linking $@...$(RESET)"
	@$(CC) $(OBJ) $(LIBFT) $(MLX42LIB) $(MLX42FLAGS) -lm -o $@
	@echo "$(GREEN)Executable $(NAME) created successfully!$(RESET)"

$(LIBFT):
	@echo "$(BLUE)Compiling libft...$(RESET)"
	@$(MAKE) -C $(LIBFT_DIR) > /dev/null 2>&1
	@echo "$(GREEN)Library libft created successfully!$(RESET)"

$(ODIR)/%.o: %.c | $(ODIR) $(DDIR)
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c -o $@ $< > /dev/null 2>&1

$(MLX42LIB):
	@$(MAKE) submodules_init
	@echo "$(BLUE)Building mlx42 library...$(RESET)"
	@cmake -S ./mlx42 -B $(MLXBUILDDIR)
	@cmake --build $(MLXBUILDDIR) -j4
	@echo "$(GREEN)Mlx42 library created successfully!$(RESET)"

$(BUILDDIR):
	@mkdir -p $(BUILDDIR) $(ODIR) $(DDIR)

submodules_init:
	@if git submodule status | egrep -q '^[-+]' ; then \
		echo "$(BLUE)Initializing and updating submodules...$(RESET)"; \
		git submodule update --init; \
		echo "$(GREEN)Submodules initialized and updated!$(RESET)"; \
	fi

clean:
	@echo "$(YELLOW)Cleaning object and dependency files...$(RESET)"
	@$(MAKE) -C $(LIBFT_DIR) clean
	@rm -f $(OBJ) $(DEPFILES)
	@echo "$(YELLOW)Cleaned up object files and temporary files.$(RESET)"

fclean: clean
	@echo "$(RED)Cleaning all build files...$(RESET)"
	@rm -rf $(BUILDDIR)
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@rm -rf $(ODIR)
	@rm -f $(NAME)
	@rm -rf $(MLXBUILDDIR)
	@echo "$(RED)Fully cleaned including executable and libraries.$(RESET)"

re: fclean all

-include $(DEPFILES)

.PHONY: all, clean, fclean, re, submodules_init