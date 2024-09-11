
#ifndef PRINTF_H
# define PRINTF_H
# include <unistd.h>
# include <stdlib.h>
# include <stdarg.h>
//# include <stdio.h>

typedef struct s_print
{
	char		*s;
	int		d;
	unsigned int	x;
}	t_print;

#endif

void	ft_push_string(char *s, int *printed)
{
	if (!s)
	{
		write(1, "(null)", 6);
		(*printed) += 6;
		return ;
	}
	while (*s != '\0')
	{
		write(1, s++, 1);
		(*printed)++;
	}
}

int	ft_numlen(int base, long d)
{
	int	cn;

	if (d == 0)
		return (1);
	cn = 0;
	while (d)
	{
		d = (d - d % base) / base;
		cn++;
	}
	return (cn);
}

int	ft_pow(int base, int exp)
{
	int	res;
	
	if (!base && !exp)
		return (0);
	res = 1;
	while (exp--)
		res *= base;	
	return (res);
}

void	ft_push_decimal(long d, int *printed)
{
	int	dig;
	int	lnum;

	if (!d)
	{
		write(1, "0", 1);
		(*printed)++;
		return ;
	}
	dig = 0;
	lnum = ft_numlen(10, d);
	if (d < 0)
	{
		d *= -1;
		write(1, "-", 1);
		(*printed)++;
	}
	while (lnum)
	{
		dig = d / ft_pow(10, --lnum) + '0'; 
		write(1, &dig, 1);
		d =  d % ft_pow(10, lnum);
		(*printed)++;
	}
}

void	ft_push_hexa(unsigned int x, int *printed)
{
	int	hex;
	char	*letters;
	int	lnum;

	if (!x)
	{
		write(1, "0", 1);
		(*printed)++;
		return ;
	}
	hex = 0;
	lnum = ft_numlen(16, x);
	letters = "abcdef";
	if (x > 0xFFFFFFFF)
		return ;
	while (lnum)
	{
		hex = x / ft_pow(16, --lnum);
		x %= ft_pow(16, lnum);
		if (hex <= 9)
		{
			hex += '0';
			write(1, &hex, 1);
			(*printed)++;
		}
		else if (hex >= 10 && hex <= 15)
		{
			write(1, letters + hex - 10, 1);
			(*printed)++;
		}			
	}
}

void	ft_choose_type(va_list *list, char **fstr, int *printed)
{
	if (!*fstr && !(*fstr + 1) && *(*fstr + 1) == '\0')
		return ;
	if (*(*fstr + 1) == 's')
	{
		ft_push_string(va_arg(*list, char *), printed);
		(*fstr)++;
	}
	else if (*(*fstr + 1) == 'd')
	{
		ft_push_decimal(va_arg(*list, int), printed);
		(*fstr)++;
	}
	else if (*(*fstr + 1) == 'x')
	{
		ft_push_hexa(va_arg(*list, unsigned int), printed);
		(*fstr)++;
	}
}

int	ft_printf(char *fstr, ...)
{
	
	va_list	list;
	int	printed;

	va_start(list, fstr);
	printed = 0;
	while (*fstr != '\0')
	{
		if (*fstr == '%')
			ft_choose_type(&list, &fstr, &printed);
		else
		{	
			write(1, fstr, 1);
			printed++;
		}
		fstr++;
	}
	va_end(list);
	return (printed);
}
/*
int	main(int argc, char **argv)
{
	int	printed;

	printed = ft_printf("Este es un test: %s, %d, %x, %x, %x, %d, %x\n", "Hola mundo", 42, 4294967295, 2147483647, 2147483648, 0, 0);
	printf("Hex: %lx, %x, %lx\n", 4294967295, 2147483647, 2147483648);
	ft_printf("printed: %d\n", printed);
	return (0);
}*/
