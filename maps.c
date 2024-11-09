#include "so_long.h"
#include <string.h>

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
        map->width = strlen(map->map[0]); // -1 pour ignorer le '\n'

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