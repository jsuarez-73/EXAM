#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

char	*get_next_line(int );

int	main(void)
{
	int	fd;
	char	*line;
	int	null;

	fd = open("test", O_RDONLY, NULL);
	if (fd == -1)
		return (-1);
	null = 1;
	while (null)
	{
		line = get_next_line(fd);	
		if (!line)
			break;
		printf("%s", line);
		if (line)
			free(line);
	}
	return (0);
}
