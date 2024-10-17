/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray_caster.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tnakas <tnakas@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 16:08:05 by sganiev           #+#    #+#             */
/*   Updated: 2024/10/11 04:09:33 by tnakas           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

/* you need to add checking of return values of mlx functions */

/*---Used_coordinate_systems----*/
/*  1:                          */
/*         90°                  */
/*          N                   */
/*          |                   */
/* 180°     |                   */
/*  W ------+----------- E 0°   */
/*          |                   */
/*          |                   */
/*         270°                 */
/*          S                   */
/*  2:                          */
/*          Y                   */
/*          ^                   */
/*          |         increases */
/*  (0,0) --+----------> X      */
/*          |                   */
/*          |                   */
/*          v                   */
/*     Y increases              */
/*------------------------------*/

/* Normalizes the angle to be within 0 to 360 degrees */
double	normalize_angle(double angle)
{
	angle = fmod(angle, 360.0);
	if (angle < 0.0)
		angle += 360.0;
	return (angle);
}

/* Sets the angle for the i-th ray. For the first ray, 
*  the angle is based on the player's viewing angle
*  and FOV. For subsequent rays, the angle is adjusted
*  based on the ray angle step. The angle is then
*  normalized  to be within 0 to 360 degrees. */
void	set_ray_angle(int i, t_cub3d *info)
{
	if (i == 0)
		info->ray[i].angle = info->player.viewing_angle
			+ (info->player.fov_angle / 2);
	else
		info->ray[i].angle = info->ray[i - 1].angle
			- info->game_dims.ray_angle_step;
	info->ray[i].angle = normalize_angle(info->ray[i].angle);
}

/* void	save_calculated_data(t_cub3d *info); */

/* Casts rays and checks both horizontal
*  and vertical intersections.
*
* 'step' - var for step_inside_grid() in
*  check_points_h() and check_points_v() */
void	cast_rays(t_cub3d *info)
{
	int	i;

	i = -1;
	while (++i < info->plane.width)
	{
		set_ray_angle(i, info);
		check_points_h(&info->ray[i], &info->player.pixel, info);
		check_points_v(&info->ray[i], &info->player.pixel, info);
		find_ray_len(&info->ray[i], &info->player.pixel);
		remove_distortion(&info->ray[i], info);
		calc_proj_slice_len(&info->ray[i], info);
		calc_top_wall_y(&info->ray[i], info);
	}
}

/* function for debugging */
/*	void	save_calculated_data(t_cub3d *info)
	{
		FILE	*fp;
		int		i;

		i = -1;
		fp = fopen("calc_data.txt", "w");
		if (!fp)
			return (perror("Error opening file for writing"));
		while (++i < info->plane.width)
		{
			fprintf(fp, "%3d-ray (angle: %6.1f): h_pixel.x: %4.1f, h_pixel.y: %4.1f"
				"\th_grid.x: %d, h_grid.y: %d", i, info->ray[i].angle,
				info->ray[i].h_intersec.x, info->ray[i].h_intersec.y,
				pixel_to_grid(info->ray[i].h_intersec.x, info->game_dims.cube_size),
				pixel_to_grid(info->ray[i].h_intersec.y,
					info->game_dims.cube_size));
			fprintf(fp, "\n\t\t\t\t\t\t v_pixel.x: %4.1f, v_pixel.y: %4.1f"
				"\tv_grid.x: %d, v_grid.y: %d",
				info->ray[i].v_intersec.x, info->ray[i].v_intersec.y,
				pixel_to_grid(info->ray[i].v_intersec.x, info->game_dims.cube_size),
				pixel_to_grid(info->ray[i].v_intersec.y,
					info->game_dims.cube_size));
			fprintf(fp, "\n\t\t\t\t\t\t ray_dist:       %4.1f", info->ray[i].dist);
			fprintf(fp, "\n\t\t\t\t\t\t proj_slice_len:%4d\n\n",
				info->ray[i].proj_slice_len);
		}
		fclose(fp);
	} */

/* apt-get update -y
*  apt-get install cmake -y
*  apt install build-essential libx11-dev libglfw3-dev libglfw3 xorg-dev -y */
