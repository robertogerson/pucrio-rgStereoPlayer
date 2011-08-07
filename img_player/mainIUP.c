/**
 *   @file CGT1Img.c Trabalho de processamento de imagem com a IUP e OpenGL.
 *
 *   @author 
 *         - Marcelo Gattass
 *         - Roberto Azevedo
 *
 *   @date
 *         Criado em:           12 de Agosto de 2004
 *         Última Modificação:    01 de Maio de 2011
 *
 */
/*- Bibliotecas padrao usadas: --------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

/*- Inclusao das bibliotecas IUP e CD: ------------------------------------*/
#include <iup.h>                    /* interface das funcoes IUP */
#include <iupgl.h>                  /* interface das funcoes do canvas do OpenGL no IUP */
#include <iupcontrols.h>            /* interface para as funcoes de botoes de controle */ 

#ifdef WIN32
  #include <windows.h>                /* inclui as definicoes do windows para o OpenGL */
  #include <gl/gl.h>                  /* prototypes do OpenGL */
  #include <gl/glu.h>                 /* prototypes do OpenGL */
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif /*WIN32*/

#include "image.h"                  /* TAD para imagem */

/*- Contexto do Programa: -------------------------------------------------*/
Image   *image1, *image2;                     /* imagens dos tres canvas */
Ihandle *left_canvas, *right_canvas, *anaglyph_canvas;  /* hadle dos dois canvas */
Ihandle *label;                           /* handle do label para colocar mensagens para usuario */
Ihandle *dialog;                          /* handle para o dialogo principal */

FILE *fpLog;            /* arquivo de log que registra as acoes do usuario */
extern char grupo[]; 
char pbuffer[300];

/*- Funcoes auxiliares ------------*/

/* Dialogo de selecao de arquivo  */
static char * get_file_name( void )
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  IupSetAttribute(getfile, IUP_TITLE, "Abertura de arquivo"  );
  IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_OPEN);
  IupSetAttribute(getfile, IUP_FILTER, "*.bmp;*.pfm");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Arquivo de imagem (*.bmp ou *.hdr)");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER);  /* o posicionamento nao esta sendo respeitado no Windows */

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}

static char * get_new_file_name( void )
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  IupSetAttribute(getfile, IUP_TITLE, "Salva arquivo"  );
  IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_SAVE);
  IupSetAttribute(getfile, IUP_FILTER, "*.bmp;*.pfm");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Arquivo de imagem (*.bmp ou *.pfm)");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER);  /* o posicionamento nao esta sendo respeitado no Windows */

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}


/* Ajusta o tamanho da janela do dialogo de acordo com o tamanho da imagem exibida */
static void update_dialog_size(void)
{
   if (image1) {
      int w,h;
      char buffer[512];
      
			w = imgGetWidth(image1);
      h = imgGetHeight(image1);
      sprintf(buffer,"%dx%d",w,h);

      IupSetAttribute(left_canvas, "SCROLLBAR", "VERTICAL");
			IupSetAttribute(right_canvas, "SCROLLBAR", "VERTICAL");
			IupSetAttribute(anaglyph_canvas, "SCROLLBAR", "VERTICAL");

      IupSetAttribute(left_canvas, "XMAX", "800");
      IupSetAttribute(left_canvas, "YMAX", "600");
			IupSetAttribute(right_canvas, "XMAX", "800");
			IupSetAttribute(right_canvas, "YMAX", "600");
			IupSetAttribute(anaglyph_canvas, "XMAX", "800");
			IupSetAttribute(anaglyph_canvas, "YMAX", "600");

     // IupSetAttribute(left_canvas, IUP_RASTERSIZE, buffer);
     // IupSetAttribute(right_canvas, IUP_RASTERSIZE, buffer);

//      IupSetAttribute(dialog, IUP_RASTERSIZE, NULL);
      IupShowXY(dialog, IUP_CENTER, IUP_CENTER);
   }
}
/*------------------------------------------*/
/* Callbacks do IUP.                        */
/*------------------------------------------*/
/* - Callback de mudanca de tamanho no canvas (mesma para ambos os canvas) */
int resize_cb(Ihandle *self, int width, int height)
{
 IupGLMakeCurrent(self);  /* torna o foco do OpenGL para este canvas */

 /* define a area do canvas que deve receber as primitivas do OpenGL */
 glViewport(0,0,width,height);

 /* transformacao de instanciacao dos objetos no sistema de coordenadas da camera */
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();           /* identidade,  ou seja nada */

 /* transformacao de projecao */
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluOrtho2D (0.0, (GLsizei)(width), 0.0, (GLsizei)(height));  /* ortografica no plano xy de [0,w]x[0,h] */

 return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
}

/* - Callback de repaint do canvas 1 */
int repaint_cb1(Ihandle *self)
{
  int w = imgGetWidth(image1);
  int h = imgGetHeight(image1);
  int type = imgGetDimColorSpace(image1);
  float* rgbData = imgGetData(image1);

  IupGLMakeCurrent(self);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2d(0.0,0.0);     /* define a origem da imagem */

  if (type==3)
     glDrawPixels (w, h, GL_RGB,GL_FLOAT, rgbData);
  else if (type==1) 
     glDrawPixels (w, h, GL_LUMINANCE,GL_FLOAT, rgbData);
  else 
     printf("Imagem de tipo desconhecida. Nao e' RGB nem Luminancia\n");

  glFlush();               /* forca atualizacao do OpenGL  (que pode ser bufferizado) */ 
  return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
}

/* - Callback de repaint do canvas 2 */
/* - poderia ser a mesma do 1, feita diferente para exemplificar OpenGL */
int repaint_cb2(Ihandle *self)
{
  int w = imgGetWidth(image2);
  int h = imgGetHeight(image2);
  int x,y;

  IupGLMakeCurrent(self);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_POINTS);
    for (y=0;y<h;y++) {
      for (x=0; x<w; x++) {
         float rgb[3];
         imgGetPixel3fv(image2, x, y, rgb );
         glColor3fv(rgb);
         glVertex2i(x,y);
       }
    }
  glEnd();
  
  glFlush();
  return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
} 

/* - Callback de repaint do anaglyph canvas */
/* - poderia ser a mesma do 1, feita diferente para exemplificar OpenGL */
int repaint_cb3(Ihandle *self)
{
  if(image1 && image2)
  {
    int w = imgGetWidth(image1);
    int h = imgGetHeight(image1);
    int x,y;

    IupGLMakeCurrent(self);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POINTS);
      for (y=0;y<h;y++) {
        for (x=0; x<w; x++) {
          float rgbl[3], rgbr[3], rgbf[3];
          imgGetPixel3fv(image1, x, y, rgbl);
          imgGetPixel3fv(image2, x, y, rgbr);

//True Anaglyphs
          rgbf[0] = 0.299*rgbl[0] + 0.587*rgbl[1] + 0.114*rgbl[2];
          rgbf[1] = 0;
          rgbf[2] = 0.299*rgbr[0] + 0.587*rgbr[1] + 0.114*rgbr[2];

//Gray Anaglyphs
//            rgbf[0] = 0.299*rgbl[0] + 0.587*rgbl[1] + 0.114*rgbl[2];
//            rgbf[1] = 0.299*rgbr[0] + 0.587*rgbr[1] + 0.114*rgbr[2];
//            rgbf[2] = 0.299*rgbr[0] + 0.587*rgbr[1] + 0.114*rgbr[2];
//Color Anaglyphs
//            rgbf[0] = rgbl[0];
//            rgbf[1] = rgbr[1];
//            rgbf[2] = rgbr[2];
//Half color Anaglyphs
//            rgbf[0] = 0.299*rgbl[0] + 0.587*rgbl[1] + 0.114*rgbl[2];
//            rgbf[1] = rgbr[1];
//            rgbf[2] = rgbr[2];
//Optimized Anaglyphs
//              rgbf[0] = 0.7*rgbl[1] + 0.3*rgbl[2];
//              rgbf[1] = rgbr[1];
//              rgbf[2] = rgbr[2];

          glColor3fv(rgbf);
          glVertex2i(x,y);
        }
      }
    glEnd();
  
    glFlush();
  }
  return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
} 

int save_cb(Ihandle *self)
{
  char* filename = get_new_file_name( );  /* chama o dialogo de abertura de arquivo */
  if (filename==NULL) return 0;
 if (strstr(filename,".bmp")||strstr(filename,".BMP"))

   imgWriteBMP(filename,image2);
 else 
   imgWritePFM(filename,image2);

  fprintf(fpLog,"SAVE %s\n",filename);
  return IUP_DEFAULT;
}

int load_left_cb(void) {
  char* filename = get_file_name();  /* chama o dialogo de abertura de arquivo */

  if (filename==NULL) return 0;

  if (strstr(filename,".bmp")||strstr(filename,".BMP"))
	{
      /* le nova imagem */
    if (image1) imgDestroy(image1);
      image1 = imgReadBMP(filename);
  }
  else if (strstr(filename,".pfm")||strstr(filename,".PFM"))
	{
       /* le nova imagem */
    if (image1) imgDestroy(image1);
      image1 = imgReadPFM(filename);
  }
  else
      IupMessage("Aviso","Formato do arquivo de imagem não reconhecido\n");

  IupSetFunction("repaint_cb1", (Icallback) repaint_cb1);

  update_dialog_size( );
  repaint_cb1(left_canvas);  /* redesenha o canvas 2 */

  IupSetfAttribute( label, "TITLE", "%3dx%3d", imgGetWidth(image1), 
	                  imgGetHeight(image1));

  IupSetFunction("save_cb", (Icallback)save_cb);
  fprintf(fpLog,"OPEN %s\n",filename);

  return IUP_DEFAULT;
}

int load_right_cb(void) {
  char* filename = get_file_name();  /* chama o dialogo de abertura de arquivo */

  if (filename==NULL) return 0;

  if (strstr(filename,".bmp")||strstr(filename,".BMP"))
	{
      /* le nova imagem */
    if (image2) imgDestroy(image1);
      image2 = imgReadBMP(filename);
  }
  else if (strstr(filename,".pfm")||strstr(filename,".PFM"))
	{
       /* le nova imagem */
    if (image2) imgDestroy(image1);
      image2 = imgReadPFM(filename);
  }
  else
      IupMessage("Aviso","Formato do arquivo de imagem não reconhecido\n");

  IupSetFunction("repaint_cb2", (Icallback) repaint_cb2);
  IupSetFunction("repaint_cb3", (Icallback) repaint_cb3);

  update_dialog_size( );
  repaint_cb2(right_canvas);  /* redesenha o canvas 2 */
  repaint_cb3(anaglyph_canvas);  /* redesenha o canvas 2 */

  IupSetfAttribute( label, "TITLE", "%3dx%3d", imgGetWidth(image2), 
	                  imgGetHeight(image2));

  IupSetFunction("save_cb", (Icallback)save_cb);
  fprintf(fpLog,"OPEN %s\n", filename);

  return IUP_DEFAULT;
}

/*-------------------------------------------------------------------------*/
/* Incializa o programa.                                                   */
/*-------------------------------------------------------------------------*/
int init(void)
{
  Ihandle *toolbar, *statusbar, *box;
  Ihandle *loadleft, *loadright, *transf, *save;

  time_t now = time(NULL);  /* obtem a data corrente para registro no arquivo de log */

  /* creates the toolbar and its buttons */
  loadleft = IupButton("", "load_left_cb");
  IupSetAttribute(loadleft,"TIP","Carrega uma imagem.");
  IupSetAttribute(loadleft,"IMAGE","IUP_FileOpen");
  IupSetFunction("load_left_cb", (Icallback)load_left_cb);
 
  loadright = IupButton("", "load_right_cb");
  IupSetAttribute(loadright,"TIP","Carrega uma imagem.");
  IupSetAttribute(loadright,"IMAGE","IUP_FileOpen");
  IupSetFunction("load_right_cb", (Icallback)load_right_cb);

  save = IupButton("", "save_cb");
  IupSetAttribute(save,"TIP","Salva imagem processada.");
  IupSetAttribute(save,"IMAGE","IUP_FileSave");

  toolbar = IupHbox(
      loadleft, 
      IupFill(),
      loadright,
      IupFill(),
      save,
      NULL);

  IupSetAttribute(toolbar, "FONT", "HELVETICA_NORMAL_8");
  IupSetAttribute(toolbar, "ALIGNMENT", "ACENTER");

  /* cria dois canvas */
  left_canvas = IupGLCanvas("repaint_cb1"); 
  IupSetAttribute(left_canvas,IUP_RASTERSIZE,"320x240");
  IupSetAttribute(left_canvas, "RESIZE_CB", "resize_cb");

  right_canvas = IupGLCanvas("repaint_cb2"); 
  IupSetAttribute(right_canvas,IUP_RASTERSIZE,"320x240");
  IupSetAttribute(right_canvas, "RESIZE_CB", "resize_cb");

  anaglyph_canvas = IupGLCanvas("repaint_cb3"); 
  IupSetAttribute(anaglyph_canvas,IUP_RASTERSIZE,"320x240");
  IupSetAttribute(anaglyph_canvas, "RESIZE_CB", "resize_cb");

  /* associa o evento de repaint a funccao resize_cb */
  IupSetFunction("repaint_cb1", NULL);
  IupSetFunction("repaint_cb2", NULL );
  IupSetFunction("repaint_cb3", NULL );
  IupSetFunction("resize_cb", (Icallback) resize_cb);

  /* the status bar is just a label to put some usefull information in run time */
  label = IupLabel("status");
  IupSetAttribute(label, "EXPAND", "HORIZONTAL");
  IupSetAttribute(label, "FONT", "HELVETICA_NORMAL_8");
  statusbar = IupSetAttributes(IupHbox(
                IupFrame(IupHbox(label, NULL)), 
                NULL), "MARGIN=5x5");

  /* this is the most external box that puts together
     the toolbar, the two canvas and the status bar */
  box = IupVbox(
          toolbar,
          IupSetAttributes(IupHbox(
            left_canvas, 
            right_canvas,
            anaglyph_canvas,
            NULL), "GAP=5"),
          statusbar, 
          NULL);

  /* create the dialog and set its attributes */
  dialog = IupDialog(box);
  IupSetAttribute(dialog, "CLOSE_CB", "app_exit_cb");
  IupSetAttribute(dialog, "TITLE", "FCG:Trabalho Processamento de Imagens");

  IupShowXY(dialog, IUP_CENTER, IUP_CENTER);

  fpLog = fopen("inf1761T1.log","at");
  fprintf(fpLog,"****************************************************************\n");
  fprintf(fpLog,"%s\n",grupo);
  fprintf(fpLog,"Data: %s\n",ctime(&now));
  
  return 1;
}

/*-------------------------------------------------------------------------*/
/* Rotina principal.                                                       */
/*-------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    IupOpen(&argc,&argv);
    IupGLCanvasOpen();
    IupImageLibOpen(); 
    if ( init() ) 
      IupMainLoop();
    IupClose();
}
