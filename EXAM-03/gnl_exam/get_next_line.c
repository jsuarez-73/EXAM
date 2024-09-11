#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>

typedef struct s_gnl
{
        char    buf[BUFFER_SIZE];
        char    *line;
        char    *tmp;
        long    bread;
        char    *tip;
        char    *off; //Point to the s
        char    *etip;
}       t_gnl;

enum    e_bool
{
        FALSE,
        TRUE
} ;

#endif

int ft_strlen(char *s)
{
	int cn;

	cn = 0;
	while (s && *s++ != '\0')
		cn++;
	return (cn);
}

/*
int     ft_strlen(char *str)
{
        int     cn;

        if (!str)
                return (0);
        cn = 0;
        while (*str++ != '\0' && ++cn) ;
        return (cn);
} */

void    ft_init_gnl(t_gnl *gnl)
{
        if (!gnl)
                return ;
        gnl->line = NULL;
        gnl->tmp = NULL;
        if (!gnl->off)
                gnl->off = gnl->buf;
        if (!gnl->tip)
                gnl->tip = gnl->buf;
        if (!gnl->etip)
                gnl->etip = gnl->buf;
}
/*
int ft_push_tmp(t_gnl *gnl, char *off, char *end)
{
	int		nlen;
	char	*new_line;

	if (!gnl || !off || !end)
		return (FALSE);
	nlen = ft_strlen(gnl->tmp) + end - off + 2;
	new_line = (char *)malloc(sizeof(char) * nlen);
	if (!new_line)
		return (FALSE);
	*(new_line + --nlen) = '\0';
	while (end >= off)
		*(new_line + --nlen) = *end--;
	while (--nlen >= 0)
		*(new_line + nlen) = *(gnl->tmp + nlen);
	if (gnl->tmp)
		free(gnl->tmp);
	gnl->tmp = new_line;
	return (TRUE);
} */

int     ft_push_tmp(char **tmp, char *off, char *end) //Just make a ft_strjoin with pointers.
{
        char    *new;
        int     ltmp;
        int     new_size;

        if (!tmp || !off || !end)
                return (FALSE);
        ltmp = ft_strlen(*tmp);
        new_size = ltmp + end - off + 2;
        new = (char *)malloc(sizeof(char) * (new_size));
        if (!new)
                return (FALSE);
        *(new + --new_size) = '\0';
        while (new_size--)
        {
                if (off <= end)
                        *(new + new_size) = *end--;
                else if (ltmp)
                        *(new + new_size) = *(*tmp + new_size);
        }
        if (*tmp)
                free(*tmp);
        *tmp = new;
        return (TRUE);
}
 
int     ft_push_data(t_gnl *gnl, int jumps)
{
        if (jumps) 
        {
            /*If it's found a \n push to tmp from off up to tip
            and tip go one forward, off points to tip and line
            is set to tmp.*/    
                if(!ft_push_tmp(&gnl->tmp, gnl->off, gnl->tip))
                        return (FALSE);
                gnl->tip++;
                gnl->off = gnl->tip;
                gnl->line = gnl->tmp;
        }
        else if (!gnl->bread && gnl->tip == gnl->etip)
                gnl->line = gnl->tmp; /*In case bytes read is 0 and tip == etip, 
                this is equality must be fullfilled because the remaining data, must
                be pushed to the temporal allocated memory first, then points line to
                tmp to return the remaining chunck.*/
        else
        {
            /*In case we wont find a \n, we need to push to tmp the data, starting from
            off up to etip - 1, This is because etip is pointing to one greater than the index
            for the bytes read, then point tip to etip to start reading again.*/
                if (!ft_push_tmp(&gnl->tmp, gnl->off, gnl->etip - 1))
                        return (FALSE);
                gnl->tip = gnl->etip;
        }
        return (TRUE);
}

int     ft_look4jumps(t_gnl *gnl) //Looks for jumps, but in case reach etip go tip one back to point to etip.
{
	while (gnl->tip <= gnl->etip)
    {
            if (*gnl->tip == '\n')
                    return (TRUE);
            gnl->tip++;
    }
    gnl->tip--;
    return (FALSE);
}

char    *get_next_line(int fd)
{
        static t_gnl    gnl;
        
        ft_init_gnl(&gnl);//Any time get called, init line and tmp to NULL, the other fields just when are NULL.
        while (!gnl.line)
        {
                if (gnl.tip == gnl.etip) //In case we have read the whole buffer.
                {
                        gnl.bread = read(fd, gnl.buf, BUFFER_SIZE);
                        if (gnl.bread) // All went good.
                        {
                                gnl.tip = gnl.buf; //It's point at the start.
                                gnl.off = gnl.buf; // It's point a the start.
                                gnl.etip = gnl.buf + gnl.bread; //Points one greater to the bytes read (as Index)
                        }       
                        if (gnl.bread == -1) //Has ocurred an error in system call.
                        {
                                if (gnl.tmp)
                                        free(gnl.tmp);
                                return (NULL);
                        }
                        else if (!gnl.bread && gnl.tmp == NULL) //EOF condition.
                                return (NULL); 
                }
                if (!ft_push_data(&gnl, ft_look4jumps(&gnl))) //Otherwise look for jumps and push data.
                {
                        if (gnl.tmp)
                                free(gnl.tmp);
                        return (NULL);
                }
        }
        return (gnl.line);
}
