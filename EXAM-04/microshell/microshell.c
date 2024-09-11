#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

typedef struct s_cmd
{
	char				**off;
	char				**end;
	enum {PIPE, SIMPLE}	e_type;
	int					npipes;
}	t_cmd;

typedef struct s_shell
{
	t_cmd	cmd;
	int		pipes[2][2];
	int		subsh;
	int		nchild;
	int		status;
	char	**envp;
}	t_shell;

int	ft_strlen(char *s)
{
	int	cn;

	cn = 0;
	if (!s)
		return (cn);
	while (s && *s++ != 0)
		cn++;
	return (cn);
}

int	ft_length_args(char **off, char **end)
{
	int	cn;

	cn = 0;
	if (!off || !*off)
		return (cn);
	while (off++ != end)
		cn++;
	return (cn);
}

void	ft_stderr(char *s)
{
	if (!s)
		return ;
	write(2, s, ft_strlen(s));
}

void	ft_fatal_termination(void)
{
	ft_stderr("error: fatal\n");
	exit(1);
}

void	ft_path_no_executable(char *s)
{
	if (!s)
		return ;
	ft_stderr("error: cannot execute ");
	ft_stderr(s);
	ft_stderr("\n");
	exit(1);
}

void	ft_cd(t_shell *sh)
{
	if (!sh)
		return ;
	if (ft_length_args(sh->cmd.off + 1, sh->cmd.end) != 1)
	{
		ft_stderr("error: cd: bad arguments\n");
		sh->status = 1;
		return ;
	}
	else if (chdir(*(sh->cmd.off + 1)))
	{
		ft_stderr("error: cd: cannot change directory to ");
		ft_stderr(*(sh->cmd.off + 1));
		ft_stderr("\n");
		sh->status = 1;
		return ;
	}
	sh->status = 0;
}

void	ft_arrange_pipe(t_shell *sh)
{
	int	cn;
	if (!sh)
		return ;
	cn = sh->subsh - 1;
	if (sh->subsh == 1 && dup2(sh->pipes[0][1], 1) == -1)
		ft_fatal_termination();
	else if (sh->subsh > 1 && sh->subsh < sh->cmd.npipes + 1)
	{
		if (dup2(sh->pipes[cn % 2][1], 1) == -1)
			ft_fatal_termination();
		if (dup2(sh->pipes[!(cn % 2)][0], 0) == -1)
			ft_fatal_termination();
	}
	else if (sh->subsh == sh->cmd.npipes + 1)
	{
		if (dup2(sh->pipes[!(cn % 2)][0], 0) == -1)
			ft_fatal_termination();
	}
}

void	ft_wait_all_children(t_shell *sh)
{
	int	back;

	if (!sh)
		return ;
	back = 0;
	while(sh->nchild--)
	{
		back = waitpid(0, &sh->status, 0);
		if (back == -1)
			ft_fatal_termination();
		if (WIFEXITED(sh->status))
			sh->status = WEXITSTATUS(sh->status);
	}
	sh->nchild = 0;
}

void	ft_set_args_pipe(t_shell *sh)
{
	char	**off;
	int		npipes;

	if (!sh)
		return ;
	npipes = 0;
	off = sh->cmd.off;
	while (off != sh->cmd.end)
	{
		if (strcmp("|", *off) == 0)
		{
			if (sh->subsh > 1 && (npipes + 1) < sh->subsh)
				sh->cmd.off = off + 1;
			npipes++;
		}
		if (sh->subsh == npipes)
		{
			sh->cmd.end = off - 1;
			*(sh->cmd.end + 1) = NULL;
			return ;
		}
		off++;
	}
	if (off == sh->cmd.end)
	{
		sh->cmd.end -= 1;
		*(sh->cmd.end + 1) = NULL;
	}
}

void	ft_close_all_fd(t_shell *sh)
{
	if (!sh)
		return ;
	if (close(sh->pipes[0][0]) || close(sh->pipes[0][1]))
		ft_fatal_termination();
	if (sh->subsh > 1 && sh->cmd.npipes > 1)
	{
		if (close(sh->pipes[1][0]) || close(sh->pipes[1][1]))
			ft_fatal_termination();
	}
}

void	ft_execute_pipe(t_shell *sh)
{
	int	cn;

	cn = 0;
	sh->subsh = 0;
	if (!sh)
		return ;
	while (cn <= sh->cmd.npipes)
	{
		if (cn > 1 && cn < sh->cmd.npipes)
		{
			if (close(sh->pipes[cn % 2][0]) || close(sh->pipes[cn % 2][1]))
				ft_fatal_termination();
		}
		if (cn < sh->cmd.npipes && pipe(sh->pipes[cn % 2]))
			ft_fatal_termination();
		sh->subsh++;
		if (!fork())
		{
			ft_arrange_pipe(sh);
			ft_set_args_pipe(sh);
			ft_close_all_fd(sh);
			if (execve(*sh->cmd.off, sh->cmd.off, sh->envp))
				ft_path_no_executable(*sh->cmd.off);
		}
		cn++;
	}
	sh->nchild = cn;
	ft_close_all_fd(sh);
	ft_wait_all_children(sh);
}

void	ft_execute_simple(t_shell *sh)
{
	if (!sh)
		return ;
	sh->nchild++;
	if(!fork())
	{
		sh->cmd.end -= 1;
		*(sh->cmd.end + 1) = NULL;
		if (execve(*sh->cmd.off, sh->cmd.off, sh->envp))
			ft_path_no_executable(*sh->cmd.off);
	}
	ft_wait_all_children(sh);
}

void	ft_execute_cmd(t_shell *sh)
{
	if (!sh)
		return ;
	if (strcmp("cd", *sh->cmd.off) == 0)
		ft_cd(sh);
	else if (strcmp(";", *sh->cmd.off) == 0)
		return ;
	else if (sh->cmd.e_type == PIPE)
		ft_execute_pipe(sh);
	else if (sh->cmd.e_type == SIMPLE)
		ft_execute_simple(sh);
}

t_cmd	ft_command(char **off, char **end)
{
	int		npipes;
	char	**it;
	if (!off || !*off)
		return ((t_cmd){0});
	npipes = 0;
	it = off;
	while (it != end)
	{
		if (strcmp("|", *it) == 0)
			npipes++;
		else if (strcmp(";", *it) == 0 && npipes > 0)
			return ((t_cmd){off, it, PIPE, npipes});
		else if (it + 1 == end && npipes > 0)
			return ((t_cmd){off, end, PIPE, npipes});
		else if (strcmp(";", *it) == 0 && npipes == 0)
			return ((t_cmd){off, it, SIMPLE, npipes});
		else if (it + 1 == end && npipes == 0)
			return ((t_cmd){off, end, SIMPLE, npipes});
		it++;
	}
	return ((t_cmd){0});
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	sh;
	char	**off;

	sh.subsh = 0;
	sh.status = 0;
	sh.nchild = 0;
	sh.envp = envp;
	off = argv;
	while (off != argv + argc)
	{
		sh.cmd = ft_command(off + 1, argv + argc);
		if (!sh.cmd.off && !sh.cmd.end)
			return (sh.status);
		ft_execute_cmd(&sh);
		off = sh.cmd.end;
	}
	return (sh.status);
}
