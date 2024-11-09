#include "so_long.h"

char *clean_stash(char *stash)
{
    int i;
    i = 0;
    int j;
    j = 0;
    char *clearedStash;
    if(stash[i] == '\0')
    {   
        free(stash);
        return NULL;
    }
    while(stash[i] && stash[i] !='\n')
    {
        i++;
    }
    clearedStash = malloc(sizeof(char)* (ft_strlen(stash) - i + 1));
    if (clearedStash == NULL)
    {
        free(stash);
        return NULL;
    }
    if(stash[i] == '\n')
        i++;

    while(stash[i])
    {
        clearedStash[j] = stash[i];
        i++;
        j++;
    }
    clearedStash[j] = '\0';
    free(stash);
    return clearedStash;
}

char *buftostash(int fd, char *stash)
{
    char *buf = malloc(sizeof(char) * (BUFFER_SIZE + 1)); 
        if(!buf)
            return NULL;    
    int ret;
    char *tmp = NULL;
    if (stash == NULL)
    {
        stash = malloc(sizeof(char) * 1);
        if(stash == NULL)
            return NULL;
        stash[0] = '\0';
    }
    while((ret = read(fd, buf, BUFFER_SIZE)) > 0)
    {
        buf[ret] = '\0'; 
        tmp = stash; 
        stash = ft_strjoin(stash, buf);
        free(tmp);
        if(stash == NULL)
            return NULL;
         if(ft_strchr(stash, '\n')) 
        break;
    }
    free(buf);
    return stash;
}

char *stashtoline(int i, char **stash)  //pointeur vers un char*
{ 
    char *line = NULL;
    int j;
    j = 0;

    if((*stash)[i] == '\n')  // on déréférence afin de réavoir le stash
    {
        line = malloc(sizeof(char) * (i + 2)); // +2 pour inclure '\n' et '\0' qui sont apres le premier '\n'
        if(line == NULL) 
            return NULL;
        ft_bzero(line, i + 2);
        while(j <= i) // <= et pas < pour que ca prenne le '\n' qui est a la toute fin après le premier '\n' 
        {
            line[j] = (*stash)[j]; ///line[i] = pointeur de caractere / *line[i] = caractère directement
            j++;
        }
        line[j] = '\0';
        *stash = clean_stash(*stash);
    }
    return line;
}

char *ifstashnotnull(char *stash)
{
	int i = 0;
	int j = 0;
	char *line = NULL;
	while(stash[i] != '\0')
	{
		i++;    
	}
	line = malloc(sizeof(char) * (i + 1));
    if(line == NULL)
    {
        return NULL;
    }
	while(j < i) 
	{
		line[j] = stash[j];
		j++;
	}
	line[j] = '\0';
	free(stash);
	stash = NULL; //sinon boucle infini car stash contiendra toujours quelque chose peut importe ce que c'est et ne sera jamais null
	return line;
}

char *get_next_line(int fd)
{
    static char *stash = NULL;
    char *line = NULL; 
    int i = 0;
    if(fd < 0 || BUFFER_SIZE <= 0 || read(fd, &line, 0) < 0)
    return NULL;
    // Extract from buff to stash
    stash = buftostash(fd, stash);
        if(stash == NULL){
            free(line);
            return NULL;
        }
    // Extract line from stash
     while((stash)[i] && (stash)[i] != '\n')
        i++;    
    line = stashtoline(i,&stash); 
    //si stash n'est pas null : 
    if(line == NULL && stash[0] != '\0')
    { 
		line = ifstashnotnull(stash);
		stash = NULL;
    }
    if (line == NULL && stash != NULL && stash[0] == '\0') {// Lorsque get_next_line termine, si le fichier a été entièrement lu et que la fonction retourne NULL, il peut rester un bloc de mémoire alloué pour stash qui n'est jamais libéré. Cela peut se produire si stash est vide ("") mais toujours alloué.
        free(stash);
        stash = NULL;
    }
    return line;
}

