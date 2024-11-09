#include "so_long.h"
#include <string.h>

#define MLX_ERROR       1
#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   800
// #define NUM_IMAGES      2

// typedef struct s_image
// {
//     void    *img;
//     char    *relative_path;
//     int     img_width;
//     int     img_height;
// }           t_image;

// typedef struct  s_data
// {
//     void        *mlx_ptr;
//     void        *win_ptr;
//     t_image     images[NUM_IMAGES];
// }               t_data;




int ft_clean(t_data *data)
{
    for(int i = 0; i < NUM_IMAGES; i++)
    {
        if (data->images[i].img)
            mlx_destroy_image(data->mlx_ptr, data->images[i].img);
    }
    if (data->win_ptr)
        mlx_destroy_window(data->mlx_ptr, data->win_ptr);
    if (data->mlx_ptr)
    {
        mlx_destroy_display(data->mlx_ptr);
        free(data->mlx_ptr);
        data->mlx_ptr = NULL;
    }
    exit(0);
}

void display_map(t_data *data, const char *file_path)
{
    int fd = open(file_path, O_RDONLY);
    if (fd < 0)
        return;

    char *line;
    int y = 0;
    while ((line = get_next_line(fd)) != NULL)
    {
        for (int x = 0; line[x] != '\0'; x++)
        {
            if (line[x] == '0')
                mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->images[0].img, x * data->images[0].img_width, y * data->images[0].img_height);
            else if(line[x] == '1')
                mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->images[1].img, x * data->images[1].img_width, y * data->images[1].img_height);
            else if(line[x] == 'C')
                mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->images[2].img, x * data->images[2].img_width, y * data->images[2].img_height);
            else if(line[x] == 'E')
                mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->images[3].img, x * data->images[3].img_width, y * data->images[3].img_height);
            else if(line[x] == 'P')
                mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->images[4].img, x * data->images[4].img_width, y * data->images[4].img_height);
        }
        free(line);
        y++;
    }
    close(fd);
}




int close_window_esc(int keysym, t_data *data)
{
    if (keysym == XK_Escape)
        ft_clean(data);


    return (0);
}



int init_image(t_data *data, int index, char *path) {
    data->images[index].relative_path = path;
    data->images[index].img_width = 100;
    data->images[index].img_height = 100;
    data->images[index].img = mlx_xpm_file_to_image(data->mlx_ptr, data->images[index].relative_path, &data->images[index].img_width, &data->images[index].img_height);
    if (data->images[index].img == NULL) {
        mlx_destroy_window(data->mlx_ptr, data->win_ptr);
        mlx_destroy_display(data->mlx_ptr);
        free(data->mlx_ptr);
        return (MLX_ERROR);
    }
    return 0;
}

void find_player_position(t_map *map, t_player *player) {
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            if (map->map[y][x] == 'P') {
                player->x = x;
                player->y = y;
                player->moves = 0; 
                printf("Position initiale du joueur trouvée : (%d, %d)\n", x, y);
                return;
            }
        }
    }
    perror("Erreur : Pas de position de départ pour le joueur.\n");
}


int main()
{
    t_data  data;
    t_map map;
    t_player player;

    ft_bzero(&data, sizeof(t_data));
    data.mlx_ptr = mlx_init();
    if (data.mlx_ptr == NULL)
        return (MLX_ERROR);

    data.win_ptr = mlx_new_window(data.mlx_ptr, WINDOW_WIDTH, WINDOW_HEIGHT, "MLX Window");
    if (data.win_ptr == NULL)
    {   
        mlx_destroy_display(data.mlx_ptr);
        free(data.mlx_ptr);
        return (MLX_ERROR);
    }

    init_image(&data, 0, "/home/ramir/Ramirez/so_long/images/block.xpm");
    init_image(&data, 1, "/home/ramir/Ramirez/so_long/images/wall.xpm");
    init_image(&data, COIN, "/home/ramir/Ramirez/so_long/images/collectible.xpm");
    init_image(&data, EXIT, "/home/ramir/Ramirez/so_long/images/exit.xpm");
    init_image(&data, PLAYER, "/home/ramir/Ramirez/so_long/images/player.xpm");

    display_map(&data, "/home/ramir/Ramirez/so_long/map/map.ber");

    //chargé `map.map`, `map.width` et `map.height`
    init_and_load_map(&map, "/home/ramir/Ramirez/so_long/map/map.ber");

    // Affiche la carte pour vérifier
    printf("Carte chargée avec dimensions : largeur = %d, hauteur = %d\n", map.width, map.height);
    for (int i = 0; i < map.height; i++)
        printf("%s", map.map[i]);
    
    find_player_position(&map, &player);

    // Libère la mémoire une fois terminé
    free_map(&map);

    mlx_hook(data.win_ptr, 2, 1L<<0, close_window_esc, &data); // KeyPress / KeyPressMask
    mlx_hook(data.win_ptr, 33, 1L<<17, ft_clean, &data); // ClientMessage / StructureNotifyMask
    mlx_loop(data.mlx_ptr);
    return (0);
}