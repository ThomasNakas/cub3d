/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sganiev <sganiev@student.42heilbronn.de>   #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024-09-12 13:18:41 by sganiev           #+#    #+#             */
/*   Updated: 2024-09-12 13:18:41 by sganiev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

int	calc_texture_x(t_ray *ray, t_cub3d *info)
{
	int	x;

	if (ray->is_v_intersec)
		x = (int)ray->v_intersec.y % info->game_dims.cube_size;
	else
		x = (int)ray->h_intersec.x % info->game_dims.cube_size;
	return (x);
}

uint32_t	calc_texture_color(int x, int y, t_ray *ray, t_cub3d *info)
{
	mlx_image_t	*img;
	uint32_t	index;
	uint32_t	color;
	uint8_t		rgba[4];

	if (ray->is_v_intersec && is_ray_right(ray->angle))
		img = info->input.ea.img;
	else if (ray->is_v_intersec && is_ray_left(ray->angle))
		img = info->input.we.img;
	else if (!ray->is_v_intersec && is_ray_up(ray->angle))
		img = info->input.no.img;
	else if (!ray->is_v_intersec && is_ray_down(ray->angle))
		img = info->input.so.img;
	else
		return (0x000000fc);
	index = (y * img->width + x) * BYTES_PER_PIXEL;
	rgba[0] = img->pixels[index];
	rgba[1] = img->pixels[index + 1];
	rgba[2] = img->pixels[index + 2];
	rgba[3] = img->pixels[index + 3];
	color = (rgba[0] << 24) | (rgba[1] << 16) | (rgba[2] << 8) | rgba[3];
	return (color);
}

/* Renders vertical wall slices on the projection plane */
void	render_wall_slices(t_cub3d *info)
{
	int		texture_x;
	double	texture_y;
	double	step;
	int		i;

	i = -1;
	while (++i < info->plane.width)
	{
		texture_y = 0;
		texture_x = calc_texture_x(&info->ray[i], info);
		step = (double)info->game_dims.cube_size
			/ (double)info->ray[i].proj_slice_len;
		if (info->ray[i].proj_slice_len > info->plane.height)
			texture_y = (info->ray[i].proj_slice_len - info->plane.height)
				/ 2 * step;
		while (info->ray[i].proj_slice_len
			&& info->ray[i].top_wall_y < info->plane.height)
		{
			mlx_put_pixel(info->img, i, info->ray[i].top_wall_y,
				calc_texture_color(texture_x, (int)texture_y,
					&info->ray[i], info));
			info->ray[i].proj_slice_len--;
			info->ray[i].top_wall_y++;
			texture_y += step;
		}
	}
}

void	rendering(t_cub3d *info)
{
	mlx_set_setting(MLX_STRETCH_IMAGE, true);
	info->mlx = mlx_init(PLANE_WIDTH, PLANE_HEIGHT, "cub3d", true);
	if (!info->mlx)
		exit(1);
	info->img = mlx_new_image(info->mlx, PLANE_WIDTH, PLANE_HEIGHT);
	if (!info->img)
		exit(1);
	load_textures(info);
	textures_to_img(info);
	resize_imgs(info);
	render_wall_slices(info);
	if (mlx_image_to_window(info->mlx, info->img, 0, 0) < 0)
		exit(1);
	mlx_close_hook(info->mlx, close_window, info);
	mlx_loop_hook(info->mlx, handle_keys, info);
	mlx_key_hook(info->mlx, handle_esc_key, info);
	mlx_loop(info->mlx);
}
