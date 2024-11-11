#include "so_long.h"
#include <string.h>

#define MLX_ERROR       1
#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   800

// #ifndef NUM_IMAGES
// #  define NUM_IMAGES 5
// #endif


// enum e_image_type
// {
//     FLOOR,
//     WALL,
//     COIN,
//     EXIT,
//     PLAYER
// };

// typedef struct s_image
// {
//     void    *img;
//     char    *relative_path;
//     int     img_width;
//     int     img_height;
// }           t_image;

// typedef struct s_map {
//     char **map;
//     int width;
//     int height;
// } t_map;

// typedef struct s_player {
//     int x;
//     int y;
//     int moves;
// } t_player;

// typedef struct  s_data
// {
//     void        *mlx_ptr;
//     void        *win_ptr;
//     int         total_coins;
//     int         coins;
//     t_image     images[NUM_IMAGES];
//     t_map       map;
//     t_player    player;
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
    free_map(&data->map);
    exit(0);
}

void display_map(t_data *data)
{
    for (int y = 0; y < data->map.height; y++)
    {
        for (int x = 0; data->map.map[y][x] != '\0'; x++)
        {
            if (data->map.map[y][x] == '0')
                mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->images[0].img, x * data->images[0].img_width, y * data->images[0].img_height);
            else if (data->map.map[y][x] == '1')
                mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->images[1].img, x * data->images[1].img_width, y * data->images[1].img_height);
            else if (data->map.map[y][x] == 'C')
                mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->images[2].img, x * data->images[2].img_width, y * data->images[2].img_height);
            else if (data->map.map[y][x] == 'E')
                mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->images[3].img, x * data->images[3].img_width, y * data->images[3].img_height);
            else if (data->map.map[y][x] == 'P')
                mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->images[4].img, x * data->images[4].img_width, y * data->images[4].img_height);
        }
    }
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

int find_number_coins(t_map *map) {
    int total_coins = 0;
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            if (map->map[y][x] == 'C') {
                total_coins++;
            }
        }
    }
    printf("nombre total de pieces : %d\n", total_coins);
    return total_coins;
}

int valid_map(t_map *map){
    //toutes les lignes de la meme tailles
    for(int y = 0; y < map->height -1; y++){
        if(ft_strlen(map->map[0]) -1 != ft_strlen(map->map[y])-1)
        {
            printf("invalid map pas carré\n");
            return -1;
        }
    }
    if(ft_strlen(map->map[0]) -2 != ft_strlen(map->map[map->height - 1]))
    {
        printf("invalid map pas carré1\n");
        return -1;
    }
    //premiere et derniere ligne remplie de 1 

    printf("CCCCCCC%cCCCCCCC\n",map->map[0][0]);
    printf("CCCCCCC%cCCCCCCC\n",map->map[0][1]);
    printf("CCCCCCC%cCCCCCCC\n",map->map[0][2]);
    printf("CCCCCCC%cCCCCCCC\n",map->map[0][3]);
    printf("CCCCCCC%cCCCCCCC\n",map->map[0][4]);
    printf("CCCCCCC%cCCCCCCC\n",map->map[0][5]);
    printf("CCCCCCC%cCCCCCCC\n",map->map[0][6]);
    printf("CCCCCCC%cCCCCCCC\n",map->map[0][7]);
    printf("CCCCCCC%ldCCCCCCC\n",ft_strlen(map->map[0]));


    for(int x = 0; x < (int)strlen(map->map[0]) - 2; x++){
        if(map->map[0][x] != '1')
        {
            printf("pas mur\n");
            return -1;
        }
    }


    //Derniere ligne remplie de 1

    for(int x = 0; x < (int)ft_strlen(map->map[0]) - 2; x++){
        if(map->map[map->height-1][x] != '1')
        {
            printf("pas mur\n");
            return -1;
        }
    }


    //premiere colonne remplie de 1 
    for (int y = 0; y < map->height - 1; y++) {
        if (map->map[y][0] != '1') 
        {
        printf("BUG\n");
        return -1;
        }
    }

    //derniere colonne remplie de 1
        // printf("BBBBB%dBBB\n", map->width);
        // // printf("CCCCCCC%sCCCCCCC\n",map->map[0]);
        // // printf("CCCCCCC%cCCCCCCC\n",map->map[0][0]);
        // // printf("CCCCCCC%cCCCCCCC\n",map->map[0][1]);
        // // printf("CCCCCCC%cCCCCCCC\n",map->map[0][2]);
        // // printf("CCCCCCC%cCCCCCCC\n",map->map[0][3]);
        // // printf("CCCCCCC%cCCCCCCC\n",map->map[0][4]);
        // // printf("CCCCCCC%cCCCCCCC\n",map->map[0][5]);
        // // printf("CCCCCCC%cCCCCCCC\n",map->map[0][6]);
        // // printf("CCCCCCC%cCCCCCCC\n",map->map[0][7]);


        // printf("CCCCCCC%cCCCCCCC\n",map->map[1][6]);
        // printf("CCCCCCC%cCCCCCCC\n",map->map[2][7]);
        // printf("CCCCCCC%cCCCCCCC\n",map->map[3][7]);
        //  for (int i = 0; i < map->height; i++)
        // printf("%s", map->map[i]);

    for ( int y = 0; y < map->height - 1; y++)
    {
        // printf("AAAAAAA%cAAAAAAA",map->map[y][(map->width) - 1]);
        if(map->map[y][(map->width) - 1] != '1')
        {
        printf("ALLED\n");
        return -1;
        }

    }
    return 0;
}



int count_lines(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return -1; 

    int lines = 0;
    char *line;

    while ((line = get_next_line(fd)) != NULL) {
        lines++;
        free(line); 
    }
    close(fd);
    return lines;
}

int init_map(t_map *map, const char *filename) {
    map->height = count_lines(filename);
    if (map->height <= 0)
        return -1; 

    map->map = malloc(sizeof(char *) * (map->height + 1));
    if (!map->map)
        return -1; 

    map->map[map->height] = NULL; // Pour marquer la fin du tableau (Dans cet exemple, height est égal à 5, donc map->map contiendra 5 pointeurs vers des chaînes de caractères valides, et le 6ème pointeur (index 5) sera NULL)
    return 0; 
}

int load_map(t_map *map, const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return -1; 

    char *line;
    int row = 0;

    while ((line = get_next_line(fd)) != NULL) {
        map->map[row] = line; 
        row++;
    }
    close(fd);

    // Détermine la largeur (nombre de colonnes) de la première ligne
    if (map->height > 0 && map->map[0] != NULL)
        map->width = strlen(map->map[0]) - 2; // -1 pour ignorer le '\n' AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

    return 0;
}

int init_and_load_map(t_map *map, const char *filename) {
    if (init_map(map, filename) < 0) {
        perror("Erreur lors de l'initialisation de la carte");
        return -1;
    }
    if (load_map(map, filename) < 0) {
        perror("Erreur lors du chargement de la carte");
        return -1;
    }
    return 0;
}

// Libère la mémoire allouée pour la carte
void free_map(t_map *map) {
    for (int i = 0; i < map->height; i++) {
        free(map->map[i]);
    }
    free(map->map);
}

void move_player(t_data *data, int new_x, int new_y) {
    if (new_x >= 0 && new_x < data->map.width && new_y >= 0 && new_y < data->map.height) {
        if (data->map.map[new_y][new_x] != '1' && data->map.map[new_y][new_x] != 'E') { // Vérifie que la nouvelle position n'est pas un mur
            if(data->map.map[new_y][new_x] == 'C'){
            data->coins++;
            printf("\x1b[33m" "A new coin collected u got %d on %d\n" "\x1b[0m", data->coins, data->total_coins);
            }
            
            data->map.map[data->player.y][data->player.x] = '0'; // Efface l'ancienne position du joueur
            data->player.x = new_x;
            data->player.y = new_y;
            data->map.map[new_y][new_x] = 'P'; // Met à jour la nouvelle position du joueur
            data->player.moves++;
            printf("Player moved to (%d, %d). Total moves: %d\n", new_x, new_y, data->player.moves);
            display_map(data); // Redessine la carte
        }
        if (data->map.map[new_y][new_x] == 'E' && data->coins == data->total_coins)
        {
            data->player.moves++;
            printf("Player moved to (%d, %d). Total moves: %d\n", new_x, new_y, data->player.moves);
            printf("\x1b[32m" "GAME END\n" "\x1b[0m");
            ft_clean(data);
        }
    }
}

int handle_keypress(int keysym, t_data *data) {
    if (keysym == XK_w || keysym == XK_Up) {
        move_player(data, data->player.x, data->player.y - 1); // Déplace vers le haut
    } else if (keysym == XK_a || keysym == XK_Left) {
        move_player(data, data->player.x - 1, data->player.y); // Déplace vers la gauche
    } else if (keysym == XK_s || keysym == XK_Down) {
        move_player(data, data->player.x, data->player.y + 1); // Déplace vers le bas
    } else if (keysym == XK_d || keysym == XK_Right) {
        move_player(data, data->player.x + 1, data->player.y); // Déplace vers la droite  DATA EST DEJA UN POINTEUR VERS DATA ALORS PAS BESOIN DE DONNER L ADRESSE
    } else if (keysym == XK_Escape) {
        ft_clean(data); // Ferme la fenêtre si la touche Échap est pressée
    }
    return 0;
}

int main()
{
    t_data  data;

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



    //chargé `map.map`, `map.width` et `map.height`
    if (init_and_load_map(&data.map, "/home/ramir/Ramirez/so_long/map/map.ber") < 0) {
        return (MLX_ERROR);
    }

    // Affiche la carte pour vérifier
    printf("Carte chargée avec dimensions : largeur = %d, hauteur = %d\n", data.map.width, data.map.height);
    for (int i = 0; i < data.map.height; i++)
        printf("%s", data.map.map[i]);


    if(valid_map(&data.map) < 0){
        return (MLX_ERROR);
    }

    find_player_position(&data.map, &data.player);
    data.total_coins = find_number_coins(&data.map);

    display_map(&data);
   

    mlx_hook(data.win_ptr, 2, 1L<<0, handle_keypress, &data); // KeyPress / KeyPressMask
    mlx_hook(data.win_ptr, 33, 1L<<17, ft_clean, &data); // ClientMessage / StructureNotifyMask
    mlx_loop(data.mlx_ptr);
    return (0);
}