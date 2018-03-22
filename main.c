#include <SDL2/SDL.h>
#include <unistd.h>
#include <time.h>

typedef struct				s_perlin
{
	SDL_Surface				*perlin;
	int						r;
	int						tmp[512];
	unsigned char			size;
}							t_perlin;

typedef struct				s_duotone
{
	SDL_Color				c1;
	SDL_Color				c2;
}							t_duotone;

int		set_color(unsigned char a, unsigned char r, unsigned char v,
				unsigned char b)
{
	int color;

	color = (a * 16711680) + (r * 65536) + (v * 256) + b;
	return (color);
}

void	put_pixel32(SDL_Surface *surface, int x, int y, Uint32 colors)
{
	Uint32	*pixels;

	pixels = (Uint32*)surface->pixels;
	pixels[(y * surface->w) + x] = colors;
}

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}

void	ft_putstr_fd(char const *s, int fd)
{
	int i;

	i = 0;
	if (!s || !fd)
		return ;
	while (s[i])
	{
		ft_putchar_fd(s[i++], fd);
	}
}

void	exit_custom_error(char *prog, char *err)
{
	ft_putstr_fd(prog, 2);
	ft_putstr_fd(err, 2);
	write(2, "\n", 1);
	exit(1);
}

int		rand_between(int min, int max)
{
	int i;

	if (min > max)
		return (min);
	i = (rand() % (max - min + 1)) + min;
	return (i);
}

int		randbet255(int r, int c, int val)
{
	if (c <= val || c < 1)
		r = (c + rand_between(0, val));
	else if (c >= 255 - val || c > 254)
		r = (c + rand_between(-val, 0));
	else
		r = (c + rand_between(-val, val));
	return (r);
}

void	perlinmaker(t_perlin *p, int x, int y)
{
	while (y < 1 && x < 512)
	{
		p->r = randbet255(p->r, p->r, p->size);
		p->tmp[x] = p->r;
		put_pixel32(p->perlin, x, y, set_color(255, p->r, p->r, p->r));
		x++;
	}
	while (x < 512)
	{
		if (x > 0 && x < 511)
			p->r = randbet255(p->r,
							(p->tmp[x] + p->tmp[x - 1] + p->tmp[x + 1]) / 3,
							p->size);
		else
			p->r = randbet255(p->r, p->tmp[x], p->size);
		p->tmp[x] = p->r;
		put_pixel32(p->perlin, x, y, set_color(255, p->r, p->r, p->r));
		x++;
	}
}

Uint32				getpixel(SDL_Surface *surface, int x, int y, int *err)
{
	int		bpp;
	Uint8	*p;

	if (x >= surface->w || y >= surface->h || x < 0 || y < 0)
	{
		*err = 1;
		return (0);
	}
	else
		*err = 0;
	bpp = surface->format->BytesPerPixel;
	p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	if (bpp == 1)
		return (*p);
	else if (bpp == 2)
		return (*(Uint16 *)p);
	else if (bpp == 3)
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return (p[0] << 16 | p[1] << 8 | p[2]);
		else
			return (p[0] | p[1] << 8 | p[2] << 16);
	else if (bpp == 4)
		return (*(Uint32 *)p);
	*err = 1;
	return (0);
}

SDL_Color	get_pixel_colors(SDL_Surface *screen, int x, int y)
{
	SDL_Color	rgba;

	rgba = (SDL_Color){0, 0, 0, 0};
	SDL_GetRGBA(getpixel(screen, x, y, &(int){0}), screen->format, &rgba.r,
						&rgba.g, &rgba.b, &rgba.a);
	return (rgba);
}

void	apply_duotone(SDL_Surface *screen, int x, int y, t_duotone t)
{
	SDL_Color curr;

	while (y < 512)
	{
		while (x < 512)
		{
			curr = get_pixel_colors(screen, x, y);
			curr.r = (curr.r * t.c1.r + (256 - curr.r) * t.c2.r) / 256;
			curr.g = (curr.g * t.c1.g + (256 - curr.g) * t.c2.g) / 256;
			curr.b = (curr.b * t.c1.b + (256 - curr.b) * t.c2.b) / 256;
			curr.a = 255;
			put_pixel32(screen, x, y, set_color(curr.a, curr.r, curr.g, curr.b));
			x++;
		}
		x = 0;
		y++;
	}
}

SDL_Color	randcolor(void)
{
	SDL_Color	color;

	color.r = rand_between(0, 255);
	color.g = rand_between(0, 255);
	color.b = rand_between(0, 255);
	color.a = 255;
	return(color);
}

t_duotone	tduotonemkr(void)
{
	t_duotone duo;

	duo.c1 = randcolor();
	duo.c2 = randcolor();
	return(duo);
}

void	perlin_pinpin(int x, int y)
{
	t_perlin	p;

	p.r = rand_between(0, 255);
	p.size = 40;
	if ((p.perlin = SDL_CreateRGBSurface(0, 512, 512, 32, 0, 0, 0, 0)) == NULL)
		exit_custom_error("Erreur SDL2 : ", (char*)SDL_GetError());
	while (y < 512)
	{
		perlinmaker(&p, x, y);
		x = 0;
		y++;
	}
	apply_duotone(p.perlin, 0, 0, tduotonemkr());
	if (SDL_SaveBMP(p.perlin, "texture.bmp") != 0)
		exit_custom_error("Erreur SDL2 : ", (char*)SDL_GetError());
}

int 	main(int ac, char** av)
{
	srand(time(NULL));
	perlin_pinpin(0,0);
	return(ac);
}
