#include "so_long.h"
#include <string.h>

#define MLX_ERROR       1

// #ifndef NUM_IMAGES
// #  define NUM_IMAGES 4
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
//     char **copy_map;
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
    free_sprites(data);
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
    free_copy_map(&data->map);
    exit(0);
}

int ft_clean_without_copy_map(t_data *data)
{
    free_sprites(data);
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


void free_sprites(t_data *data)
{
    for (int i = 0; i < 3; i++)
    {
        if (data->animation.sprite[i])
        {
            mlx_destroy_image(data->mlx_ptr, data->animation.sprite[i]);
            data->animation.sprite[i] = NULL;
        }
    }
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
        }
    }
}

int close_window_esc(int keysym, t_data *data)
{
    if (keysym == XK_Escape)
        ft_clean(data);

    return (0);
}

int init_image(t_data *data, int index, char *path)
{
    data->images[index].relative_path = path;
    data->images[index].img_width = 100;
    data->images[index].img_height = 100;
    data->images[index].img = mlx_xpm_file_to_image(data->mlx_ptr, data->images[index].relative_path, &data->images[index].img_width, &data->images[index].img_height);
    if (data->images[index].img == NULL)
    {
        mlx_destroy_window(data->mlx_ptr, data->win_ptr);
        mlx_destroy_display(data->mlx_ptr);
        free(data->mlx_ptr);
        return (MLX_ERROR);
    }
    return 0;
}

void find_player_position(t_map *map, t_player *player)
{
    for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < map->width; x++) 
        {
            if (map->map[y][x] == 'P')
            {
                player->x = x;
                player->y = y;
                player->moves = 0; 
                printf("Position initiale du joueur trouvée : (%d, %d)\n", x, y);
                return;
            }
        }
    }
    printf("Erreur : Pas de position de départ pour le joueur.\n");
}

int find_number_coins(t_map *map)
{
    int total_coins = 0;
    for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < map->width; x++)
        {
            if (map->map[y][x] == 'C')
            {
                total_coins++;
            }
        }
    }
    printf("nombre total de pieces : %d\n", total_coins);
    return total_coins;
}

int valid_map(t_map *map)
{
    //verif que toutes les lignes soient de la meme tailles (pour le -1 c'est qu'on ne veut pas prendre la derniere ligne de la carte dans cette boucle(si pas compris regarder le prochain commentaire) et pour ca on met tant que y < que la derniere ligne ca veut dire que la boucle ne passera jamais sur la derinere ligne car ce n est pas =<)
    for(int y = 0; y < map->height -1; y++)
    {
        if(ft_strlen(map->map[0]) -1 != ft_strlen(map->map[y])-1)
        {
            printf("invalid map pas carré\n");
            return -1;
        }
    }

    //il y avait un bug sur toutes lignes sauf la derniere ou ca détectais un caractère en trop alors c'est pour ca qu'il faut faire ce if a part pour la derniere ligne avec strlen -2 au lieu de strlen -1
    if(ft_strlen(map->map[0]) -2 != ft_strlen(map->map[map->height - 1]))
    {
        printf("invalid map pas carré1\n");
        return -1;
    }

    //premiere et derniere ligne remplie de 1
    for(int x = 0; x < (int)ft_strlen(map->map[0]) - 2; x++)
    {
        //verification premiere ligne remplie de 1 
        if(map->map[0][x] != '1')
        {
            printf("Premiere ligne de mur incorrecte\n");
            return -1;
        }
        // verification derniere ligne remplie de 1 
        if(map->map[map->height-1][x] != '1')
        {
            printf("Derniere ligne de mur incorrecte\n");
            return -1;
        }
    }

    //premiere et derniere colonne remplie de 1
    for (int y = 0; y < map->height - 1; y++)
    {
        // verification premiere colonne remplie de 1 
        if (map->map[y][0] != '1') 
        {
            printf("Premiere colonne de mur incorrecte\n");
            return -1;
        }
        // verification derniere colonne remplie de 1 
        if(map->map[y][(map->width) - 1] != '1')
        {
            printf("Derniere colonne de mur incorrecte\n");
            return -1;
        }
    }

    return 0;
}

int valid_number_elements(t_map *map)
{
    int coins = 0;
    int exit = 0;
    int player = 0;

    //pas besoin de mettre map->height - 1 car c'est plus petit que map->height donc on n'atteinds jamais la derniere ligne qui a été initialisé à NULL au préalable
    for(int y = 0; y < map->height; y++)
    {
        for(int x = 0; map->map[y][x] != '\0'; x++)
        {
            if(map->map[y][x] == 'C')coins++;
            if(map->map[y][x] == 'E')exit++;
            if(map->map[y][x] == 'P')player++;
        }
    }

    if(coins == 0 || exit != 1 || player != 1)
        return -1;

    return 0;
}

int count_lines(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return -1; 

    int lines = 0;
    char *line;

    while ((line = get_next_line(fd)) != NULL)
    {
        lines++;
        free(line); 
    }
    close(fd);
    return lines;
}

int init_map(t_map *map, const char *filename)
{
    map->height = count_lines(filename);
    if (map->height <= 0)
        return -1; 

    map->map = malloc(sizeof(char *) * (map->height + 1)); //pas sur d'avoir compris pourquoi on doit mettre sizeif(char *) et pas sizeof(char **) ?
    if (!map->map)
        return -1; 

    map->map[map->height] = NULL; // Pour marquer la fin du tableau (Dans cet exemple, height est égal à 5, donc map->map contiendra 5 pointeurs vers des chaînes de caractères valides, et le 6ème pointeur (index 5) sera NULL)
    // Voici un exemple : si map->height = 7 (par exemple une carte de 7 lignes) :
        // la mémoire alloué par le malloc (map->map = malloc(sizeof(char *) * (map->height + 1));) pour map->map contient 8 pointeurs (map->height + 1) == ( 7 + 1 ) == ( 8 ).
        // les indices accessibles dans le tableau vont de 0 à 7 :
            //Indices 0 à 6 : servent à stocker les 7 lignes de la carte.
            //indice 7 : est l'espace supplémentaire pour le NULL.
    //Ainsi, pour marquer la fin du tableau, on place le NULL dans map->map[map->height], soit le 8ème pointeur (indice 7 dans cet exemple).
    return 0; 
}

int load_map(t_map *map, const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return -1; 

    char *line;
    int row = 0;

    while ((line = get_next_line(fd)) != NULL)
    {
        map->map[row] = line; 
        row++;
    }
    close(fd);

    // Détermine la largeur (nombre de colonnes) de la première ligne
    if (map->height > 0 && map->map[0] != NULL)
        map->width = ft_strlen(map->map[0]) - 2; // il fallait faire -2 car il y avait toujours un caractère en trop je ne sais pas trop pourquoi, mais en faisaint -2 ca fonctionne bien

    return 0;
}

int init_and_load_map(t_map *map, const char *filename)
{
    //défini map->height + alloue de la méméoir pour map->map et met le dernier tableau de map->map à NULL
    if (init_map(map, filename) < 0)
    {
        printf("Erreur lors de l'initialisation de la carte");
        return -1;
    }
    //remplie map->map et défini map->width
    if (load_map(map, filename) < 0)
    {
        printf("Erreur lors du chargement de la carte");
        return -1;
    }
    return 0;
}

// Libère la mémoire allouée pour la carte
void free_map(t_map *map)
{
    for (int i = 0; i < map->height; i++)
    {
        free(map->map[i]);
    }
    free(map->map);
}

// Libère la mémoire allouée pour la carte temporaire utilisé pour le flood fill
void free_copy_map(t_map *map)
{
    for (int i = 0; i < map->height; i++)
    {
        free(map->copy_map[i]);
    }
    free(map->copy_map);
}


void display_moves(t_data *data)
{
    char *move_count;
    char *message;
    // Convertir le nombre de mouvements en chaîne de caractères
    move_count = ft_itoa(data->player.moves); 
    if (!move_count)
        return;

    message = ft_strjoin("Moves: ", move_count); 
    if (!message)
    {
        free(move_count);
        return;
    }

    mlx_string_put(data->mlx_ptr, data->win_ptr, 10, 10, 0xFFFFFF, message);

    // Libérer les chaînes de caractères allouées dynamiquement
    free(move_count);
    free(message);
}




void move_player(t_data *data, int new_x, int new_y)
{
    if (new_x >= 0 && new_x < data->map.width && new_y >= 0 && new_y < data->map.height)
    {
        if (data->map.map[new_y][new_x] != '1' && data->map.map[new_y][new_x] != 'E') // Vérifie que la nouvelle position n'est pas un mur
        { 
            if(data->map.map[new_y][new_x] == 'C')
            {
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
            // display_moves(data); // Affiche les mouvements du joueur
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

int handle_keypress(int keysym, t_data *data)
{
    if (keysym == XK_w || keysym == XK_Up){
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


// Copie la carte pour que le flood fill puisse fonctionner sans modifier l'original
char **copy_map(t_map *map)
{
    char **copy = malloc(sizeof(char *) * (map->height + 1));
    if (!copy) return NULL;

    for (int i = 0; i < map->height; i++)
    {
        copy[i] = strdup(map->map[i]);
        if (!copy[i])
        {
            for (int j = 0; j < i; j++) free(copy[j]);
            free(copy);
            return NULL;
        }
    }
    copy[map->height] = NULL;
    return copy;
}


void flood_fill_coins(char **copy_map, int x, int y)
{
    // Si la position actuelle est hors de la carte ou déjà visitée, on quitte
    if (y < 0 || x < 0 || copy_map[y] == NULL || copy_map[y][x] == '\0' || copy_map[y][x] == '1' || copy_map[y][x] == 'V' || copy_map[y][x] == 'E')
    {
        return;
    }

    // Marquer la case comme visitée
    copy_map[y][x] = 'V';

    // Remplir les cases adjacentes
    flood_fill_coins(copy_map, x + 1, y);  // Droite
    flood_fill_coins(copy_map, x - 1, y);  // Gauche
    flood_fill_coins(copy_map, x, y + 1);  // Bas
    flood_fill_coins(copy_map, x, y - 1);  // Haut
}

void flood_fill_exit(char **copy_map, int x, int y)
{
    // Si la position actuelle est hors de la carte ou déjà visitée, on quitte
    if (y < 0 || x < 0 || copy_map[y] == NULL || copy_map[y][x] == '\0' || copy_map[y][x] == '1' || copy_map[y][x] == 'V')
    {
        return;
    }

    // Marquer la case comme visitée
    copy_map[y][x] = 'V';

    // Remplir les cases adjacentes
    flood_fill_exit(copy_map, x + 1, y);  // Droite
    flood_fill_exit(copy_map, x - 1, y);  // Gauche
    flood_fill_exit(copy_map, x, y + 1);  // Bas
    flood_fill_exit(copy_map, x, y - 1);  // Haut
}


int is_path_valid(t_map *map, int player_x, int player_y)
{
    // Crée une copie de la carte
    map->copy_map = copy_map(map);
    if (!map->copy_map) return 0;

    flood_fill_coins(map->copy_map, player_x, player_y);

    // Vérifie si tous les 'C' ont été atteints en comptant 'E' comme un mur
    int valid = 1;
    for (int y = 0; y < map->height && valid == 1; y++)
    {
        for (int x = 0; map->copy_map[y][x] != '\0'; x++)
        {
            if (map->copy_map[y][x] == 'C')
            {
                valid = 0;  // Il reste un élément inatteignable
                break;
            }
        }
    }

    free_copy_map(map);
    map->copy_map = copy_map(map);
    if (!map->copy_map) return 0;

    flood_fill_exit(map->copy_map, player_x, player_y);

    for (int y = 0; y < map->height && valid == 1; y++)
    {
        for (int x = 0; map->copy_map[y][x] != '\0'; x++)
        {
            if (map->copy_map[y][x] == 'E')
            {
                valid = 0;  // Il reste un élément inatteignable
                break;
            }
        }
    }

    return valid;
}


void load_sprites(t_data *data)
{
    data->animation.sprite[0] = mlx_xpm_file_to_image(data->mlx_ptr,
        "/home/ramir/Ramirez/so_long/images/player1.xpm",
        &data->animation.sprite_width, &data->animation.sprite_height);
    data->animation.sprite[1] = mlx_xpm_file_to_image(data->mlx_ptr,
        "/home/ramir/Ramirez/so_long/images/player2.xpm",
        &data->animation.sprite_width, &data->animation.sprite_height);
    data->animation.sprite[2] = mlx_xpm_file_to_image(data->mlx_ptr,
        "/home/ramir/Ramirez/so_long/images/player3.xpm",
        &data->animation.sprite_width, &data->animation.sprite_height);
    if (!data->animation.sprite[0] || !data->animation.sprite[1] || !data->animation.sprite[2])
    {
        write(2, "Error loading sprites\n", 22);
        exit(EXIT_FAILURE);
    }
    data->animation.current_frame = 0; // Initialiser à la première frame
}


int animate_sprite_display_moves_appel(t_data *data)
{
    static int frame_count = 0;

    frame_count++;
    if (frame_count >= 50) // Change la frame toutes les 10 itérations
    {
        frame_count = 0;
        data->animation.current_frame = (data->animation.current_frame + 1) % 3;
        //        		game->current_frame	|game->current_frame + 1|(game->current_frame + 1)%3
        //intialisation : 	    0     		|            1 		    |               1		    | FRAME 1
        //2eme animation:		1		    |	      	 2	        |		        2		    | FRAME 2
        //3eme animation:		2		    |	      	 3	        |    0 (car 3 mod 3 = 0)    | FRAME 0 (on revient au début)
        //4eme animation:		0		    |            1		    |		        1	       	| FRAME 1
    }

    display_moves(data);
    
    // Dessiner le sprite animé au-dessus de la carte
    mlx_put_image_to_window(data->mlx_ptr, data->win_ptr,
        data->animation.sprite[data->animation.current_frame],
        data->player.x * data->images[0].img_width,
        data->player.y * data->images[0].img_height);

    return (0);
}




int main()
{
    t_data  data;

    ft_bzero(&data, sizeof(t_data));
    data.mlx_ptr = mlx_init();
    if (data.mlx_ptr == NULL)
        return (MLX_ERROR);

    //chargé `map.map`, `map.width` et `map.height`
    if (init_and_load_map(&data.map, "/home/ramir/Ramirez/so_long/map/map.ber") < 0)
    {
        return (MLX_ERROR);
    }

    // Affiche la carte pour vérifier
    printf("Carte chargée avec dimensions : largeur = %d, hauteur = %d\n", data.map.width, data.map.height);

    int winodw_width = data.map.width * 42;
    int window_height = data.map.height * 42;

    data.win_ptr = mlx_new_window(data.mlx_ptr, winodw_width, window_height, "MLX Window");
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

    //print la map original
    printf("\nla map de base : \n");
    for (int i = 0; i < data.map.height; i++)
        printf("%s", data.map.map[i]);


    load_sprites(&data);



    if(valid_map(&data.map) < 0)
    {
        ft_clean_without_copy_map(&data);
        return (MLX_ERROR);
    }

    if(valid_number_elements(&data.map) < 0)
    {
        printf("pas player ou pas coins ou pas exit looser");
        ft_clean_without_copy_map(&data);
        return (MLX_ERROR);
    }

    find_player_position(&data.map, &data.player);
    data.total_coins = find_number_coins(&data.map);

    if(is_path_valid(&data.map, data.player.x, data.player.y) == 0)
    {
    printf("Erreur: Impossible d'atteindre tous les objectifs depuis la position actuelle.\n");
    ft_clean(&data);
    return (MLX_ERROR);
    }

    display_map(&data);
   
    mlx_hook(data.win_ptr, 2, 1L<<0, handle_keypress, &data); // KeyPress / KeyPressMask
    mlx_hook(data.win_ptr, 33, 1L<<17, ft_clean, &data); // ClientMessage / StructureNotifyMask (en gros quand on appuies sur la croix)  //pas compris comment on lis CLientMessage à ON_DESTROY dans la doc ?
    mlx_loop_hook(data.mlx_ptr, (int (*)())animate_sprite_display_moves_appel, &data);  // mlx hook est prototypé comme ca : int mlx_loop_hook(void *mlx_ptr, int (*f)(void *), void *param);   alors on doit avoir une fonction qui retourne un int et qui prends en paramètre un void mais vu que ce n'est pas notre cas on doit le caster. d'ou le (int (*)())
    
    mlx_loop(data.mlx_ptr);
    return (0);
}