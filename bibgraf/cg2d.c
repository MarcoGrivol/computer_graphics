#include "cg2d.h"

point *SetPoint(float x, float y, float w, int color)
{
  point *pnt;

  pnt = (point *)malloc(sizeof(point));
  pnt->x = x / w;
  pnt->y = y / w;
  pnt->w = w;
  pnt->color = color;

  return pnt;
}

object *CreateObject(int numbers_of_points)
{
  object *ob;

  ob = (object *)malloc(sizeof(object));
  ob->numbers_of_points = 0;
  ob->points = (point *)malloc(numbers_of_points * sizeof(point));

  return ob;
}

int SetObject(point *pnt, object *ob)
{
  ob->points[ob->numbers_of_points] = *pnt;
  ob->numbers_of_points = ob->numbers_of_points + 1;

  return 0;
}

palette *CreatePalette(int numbers_of_colors)
{
  palette *pal;

  pal = (palette *)malloc(sizeof(palette));
  pal->numbers_of_colors = 0;
  pal->colors = (ColorValues *)malloc(numbers_of_colors * sizeof(ColorValues));

  return pal;
}

int SetColor(float red, float green, float blue, palette *pal)
{
  pal->colors[pal->numbers_of_colors].red = red;
  pal->colors[pal->numbers_of_colors].green = green;
  pal->colors[pal->numbers_of_colors].blue = blue;
  pal->numbers_of_colors = pal->numbers_of_colors + 1;

  return 0;
}

ColorValues *GetColor(int value, palette *pal)
{
  ColorValues *color;

  color = (ColorValues *)malloc(sizeof(ColorValues));
  color->red = pal->colors[value].red;
  color->green = pal->colors[value].green;
  color->blue = pal->colors[value].blue;

  return color;
}

window *CreateWindow(float xmin, float ymin, float xmax, float ymax)
{
  window *win;

  win = (window *)malloc(sizeof(window));
  win->xmin = xmin;
  win->xmax = xmax;
  win->ymin = ymin;
  win->ymax = ymax;

  return win;
}

viewport *CreateViewPort(int xmin, int ymin, int xmax, int ymax)
{
  viewport *port;

  port = (viewport *)malloc(sizeof(viewport));
  port->xmin = xmin;
  port->xmax = xmax;
  port->ymin = ymin;
  port->ymax = ymax;

  return port;
}

// O aluno atento irá perceber que nas próximas duas funções utilizamos as respostas da primeira atividade!
point *Sru2Srn(point *ponto, window *janela)
{
  point *np;

  np = (point *)malloc(sizeof(point));
  np->x = (ponto->x - janela->xmin) / (janela->xmax - janela->xmin);
  np->y = (ponto->y - janela->ymin) / (janela->ymax - janela->ymin);
  np->w = ponto->w;
  np->color = ponto->color;

  return np;
}

point *Srn2Srd(point *ponto, viewport *port)
{
  point *dpt;

  dpt = (point *)malloc(sizeof(point));
  dpt->x = port->xmin + round((ponto->x) * ((port->xmax - port->xmin) - 1));
  dpt->y = port->ymin + round((ponto->y) * ((port->ymax - port->ymin) - 1));
  dpt->w = ponto->w;
  dpt->color = ponto->color;

  return dpt;
}

bufferdevice *CreateBuffer(int maxx, int maxy)
{ // O buffer nesse caso se refere a um SRD
  bufferdevice *dev;

  dev = (bufferdevice *)malloc(sizeof(bufferdevice));
  dev->MaxX = maxx;
  dev->MaxY = maxy;
  dev->buffer = (int *)malloc(maxx * maxy * sizeof(int));

  return dev;
}

int DrawObject(object *ob, window *win, viewport *port, bufferdevice *dev, int color)
{

  // A cor do objeto pode ser alterada no parâmetro color ou
  // pode-se implementar uma função que faça isso separadamente

  int i;
  float aux;
  point *p1, *p2, *paux;

  // Os limites das viewports devem ser traçados
  // diretamente no SRD e não no SRU.
  // O uso dessa função foi sugerido pelo professor
  // como alternativa e foi implementada nesta versão
  // para fins de ilustração
  DrawViewPort(port, dev, 1);

  for (i = 0; i < ob->numbers_of_points; i++)
  {
    p1 = SetPoint(ob->points[i].x, ob->points[i].y, ob->points[i].w, ob->points[i].color);
    p2 = SetPoint(ob->points[(i + 1) % ob->numbers_of_points].x, ob->points[(i + 1) % ob->numbers_of_points].y, ob->points[(i + 1) % ob->numbers_of_points].w, ob->points[(i + 1) % ob->numbers_of_points].color);

    if (p1->y > p2->y)
    {
      aux = p1->y;
      p1->y = p2->y;
      p2->y = aux;
      aux = p1->x;
      p1->x = p2->x;
      p2->x = aux;
    }
    if ((p1->y < win->ymax) && (p2->y > win->ymax))
    {
      paux = InterY(p1, p2, win->ymax);
      if (InWin(paux, win))
      {
        p2 = paux;
      }
    }
    if ((p1->y < win->ymin) && (p2->y > win->ymin))
    {
      paux = InterY(p1, p2, win->ymin);
      if (InWin(paux, win))
      {
        p1 = paux;
      }
    }

    if (p1->x > p2->x)
    {
      aux = p1->y;
      p1->y = p2->y;
      p2->y = aux;
      aux = p1->x;
      p1->x = p2->x;
      p2->x = aux;
    }
    if ((p1->x < win->xmax) && (p2->x > win->xmax))
    {
      paux = InterX(p1, p2, win->xmax);
      if (InWin(paux, win))
      {
        p2 = paux;
      }
    }
    if ((p1->x < win->xmin) && (p2->x > win->xmin))
    {
      paux = InterX(p1, p2, win->xmin);
      if (InWin(paux, win))
      {
        p1 = paux;
      }
    }

    if ((InWin(p1, win)) && (InWin(p2, win)))
    {
      // Cuidado! A função DrawLine() foi escrita de modo a trabalhar apenas se chamada pela DrawObject()
      // DrawLine(p1, p2, win, port, dev, color);

      float a, b, aux;
      int i, j;
      point *pn1, *pd1, *pn2, *pd2;

      pn1 = Sru2Srn(p1, win);
      pd1 = Srn2Srd(pn1, port);
      pn2 = Sru2Srn(p2, win);
      pd2 = Srn2Srd(pn2, port);

      if (pd1->x > pd2->x)
      { // O swap entre os pontos pode ser necessário. Por quê?
        aux = pd1->x;
        pd1->x = pd2->x;
        pd2->x = aux;
        aux = pd1->y;
        pd1->y = pd2->y;
        pd2->y = aux;
      }

      j = pd1->y;
      i = pd1->x;

      // // Agora é com vocês! Completem a função! O resultado deve ser similar ao encontrado quando o programa exemplo1,
      // // que está disponível na seção Biblioteca Gráfica, é executado.

   if (pd1->x == pd2->x) {
     while (j < pd2->y) {
       dev->buffer[(dev->MaxY - j - 1) * dev->MaxX + i] = color;
       j++;
       }
     }
   else {
     a = (pd2->y - pd1->y)/(pd2->x - pd1->x);
     b = pd1->y - a*pd1->x;
     while (i < pd2->x) {
       dev->buffer[(dev->MaxY - j - 1) * dev->MaxX + i] = color;
       aux = j;
       j = round(a*(++i) + b);
       
       if (j > aux) {
	 while (aux < j) {
	   dev->buffer[(dev->MaxY - (int)aux - 1) * dev->MaxX + i] = color; 
	   aux++;
	   }
         }
       if (j < aux) {
	 while (aux > j) { 
	   dev->buffer[(dev->MaxY - (int)aux - 1) * dev->MaxX + i] = color;
	   aux--;
	   }
         }
        
       }
     }

      // int srd[dev->MaxY][dev->MaxX];
      // for (i = 0; i < dev->MaxY; i++)
      // {
      //   for (j = 0; j < dev->MaxX; j++)
      //   {
      //     srd[i][j] = 0;
      //   }
      // }

      // int y_offset = port->ymax - pd2->y; // viewport - pontoy_max
      // printf("%d offset\n", y_offset);
      // j = pd1->y - y_offset;
      // i = pd1->x;
      // srd[j][i] = color;
      // printf("(%d, %d) index = %d\n", i, j, j*x+i);
      // i = pd2->x;
      // j = pd1->y - y_offset;
      // srd[j][i] = color;
      // printf("(%d, %d) index = %d\n", i, j, j*x+i);

      // x = dev->MaxX, y = dev->MaxY;
      // index = 0;
      // for (j = 0; j < y; j++)
      // {
      //   for (i = 0; i < x; i++)
      //   {
      //     if (srd[j][i] == color)
      //     {
      //       // dev->buffer[index] = color;
      //       printf("(%d, %d) -> %d\n", i, j, index);
      //     }
      //     index++;
      //   }
      // }
      // printf("*********************************\n");
      // printf("p1=(%f, %f) -> pn1=(%f, %f) -> pd1=(%f, %f)\n",
      //        p1->x, p1->y, pn1->x, pn1->y, pd1->x, pd1->y);
      // printf("p2=(%f, %f) -> pn2=(%f, %f) -> pd2=(%f, %f)\n",
      //        p2->x, p2->y, pn2->x, pn2->y, pd2->x, pd2->y);

    }
  }

  return 0;
}
