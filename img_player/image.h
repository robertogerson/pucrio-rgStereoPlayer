/**
 *	@file image.h Image: operações imagens. Escreve e le formato TGA e BMP.
 *
 *
 *	@author 
 *			- Marcelo Gattass
 *			- Maira Noronha
 *			- Thiago Bastos
 *			- Fabíola Maffra
 *      - Roberto Azevedo
 *
 *	@date
 *			Criado em:		 1 de Dezembro de 2002
 *			Última Modificação:	01 de Maio de 2011
 *
 *	@version 3.0
 */

#ifndef IMAGE_H
#define IMAGE_H

#define ROUND(_) (int)floor( (_) + 0.5 )

/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/
/**
 *   Imagem com um buffer rgb.
 */

typedef struct Image_imp Image;


typedef enum {
  IMG_NO_FILTER=0,
	IMG_BILLINEAR_FILTER,
	IMG_EWA_FILTER

} Image_Filter;

/************************************************************************/
/* Funcoes Exportadas                                                   */
/************************************************************************/

/**
 *	Cria uma nova imagem com as dimensoes especificadas.
 *
 *	@param w Largura da imagem.
 *	@param h Altura da imagem.
 *	@param dcs Dimensao do espaco de cor de cada pixel (1=luminancia ou 3=RGB).
 *
 *	@return Handle da imagem criada.
 */
Image  * imgCreate (int w, int h, int dcs);

/**
 *	Destroi a imagem.
 *
 *	@param image imagem a ser destruida.
 */
void    imgDestroy (Image*image);

/**
 *	Cria uma nova nova copia imagem dada.
 *
 *	@param image imagem a ser copiada.
 *
 *	@return Handle da imagem criada.
 */
Image* imgCopy(Image* image);

/**
 *	Cria uma nova nova copia imagem dada em tons de cinza.
 *
 *	@param image imagem a ser copiada em tons de cinza.
 *
 *	@return Handle da imagem criada.
 */
Image* imgGrey(Image* image);

/**
 *	Obtem a largura (width) de uma imagem.
 *
 *	@param image Handle para uma imagem.
 *	@return  a largura em pixels (width) da imagem.
 */
int imgGetWidth(Image* image);

/**
 *	Obtem a altura (heigth) de uma imagem.
 *
 *	@param image Handle para uma imagem.
 *	@return  a altura em pixels (height) da imagem.
 */
int imgGetHeight(Image* image);

/**
 *	Obtem a dimensao do espaco de cor de cada pixel (1=lminancia ou 3=RGB).
 *
 *	@param image Handle para uma imagem.
 *	@return  dimensao do espaco de cor de cada pixel (1=lminancia ou 3=RGB) da imagem.
 */
int imgGetDimColorSpace(Image* image);

/**
 *	Obtem as dimensoes de uma imagem.
 *
 *	@param image Handle para uma imagem.
 *	@param w [out]Retorna a largura da imagem.
 *	@param h [out]Retorna a altura da imagem.
 */
float*  imgGetData(Image* image);

/**
 *	Ajusta o pixel de uma imagem com a cor especificada.
 *
 *	@param image Handle para uma imagem.
 *	@param x Posicao x na imagem.
 *	@param y Posicao y na imagem.
 *	@param color Cor do pixel(valor em float [0,1]).
 */
void imgSetPixel3fv(Image* image, int x, int y, float*  color);

/**
 *	Ajusta o pixel de uma imagem com a cor especificada.
 *
 *	@param image Handle para uma imagem.
 *	@param x Posicao x na imagem.
 *	@param y Posicao y na imagem.
 *	@param color Cor do pixel (valor em unsigend char[0,255]).
 */
void imgSetPixel3ubv(Image* image, int x, int y, unsigned char * color);

/**
 *	Obtem o pixel de uma imagem na posicao especificada.
 *
 *	@param image Handle para uma imagem.
 *	@param x Posicao x na imagem.
 *	@param y Posicao y na imagem.
 *	@param color [out] Pixel da posicao especificada(valor em float [0,1]).
 */
void imgGetPixel3fv(Image* image, int x, int y, float* color);

/**
 *	Obtem o pixel de uma imagem na posicao especificada.
 *
 *	@param image Handle para uma imagem.
 *	@param x Posicao x na imagem.
 *	@param y Posicao y na imagem.
 *	@param color [out] Pixel da posicao especificada (valor em unsigend char[0,255]).
 */
void imgGetPixel3ubv(Image* image, int x, int y, unsigned char *color);

/**
*	Calcula a diferenca entre duas imagens.
*
*	@param img0 img0 =  |img0-img1|^1/gamma
*	@param img1.
*
*	@return retorna o valor medio dos canais rgb de todos pixels (antes da correcao gamma).
*
*/
float imgDif(Image*img0, Image*img1, float gamma);


/**
 *	Le a imagem a partir do arquivo especificado.
 *
 *	@param filename Nome do arquivo de imagem.
 *
 *	@return imagem criada.
 */
Image* imgReadTGA(char *filename);

/**
 *	Salva a imagem no arquivo especificado em formato TGA.
 *
 *	@param filename Nome do arquivo de imagem.
 *	@param image Handle para uma imagem.
 *
 *	@return retorna 1 caso nao haja erros.
 */
int imgWriteTGA(char *filename, Image* image);

/**
 *	Salva a imagem no arquivo especificado em formato BMP.
 *
 *	@param filename Nome do arquivo de imagem.
 *	@param bmp Handle para uma imagem.
 *
 *	@return retorna 1 caso nao haja erros.
 */
int imgWriteBMP(char *filename, Image* bmp);

/**
 *	Le a imagem a partir do arquivo especificado.
 *
 *	@param filename Nome do arquivo de imagem.
 *
 *	@return imagem criada.
 */
Image* imgReadBMP (char *filename);


/**
 *	Le a imagem a partir do arquivo especificado.
 *  A imagem e' armazenada como um arquivo binario
 *  onde os tres campos da .
 *
 *	@param filename Nome do arquivo de imagem.
 *
 *	@return imagem criada.
 */
Image* imgReadPFM(char *filename);

/**
 *	Salva a imagem no arquivo especificado .
 *
 *	@param filename Nome do arquivo de imagem.
 *	@param image Handle para uma imagem.
 *
 *	@return retorna 1 caso nao haja erros.
 */
int imgWritePFM(char *filename, Image* image);


/**
 *	Conta o numero de cores diferentes na imagem
 *
 *	@param image Handle para uma imagem.
 *	@param w Nova largura da imagem.
 *	@param h Nova altura da imagem.
 */
int imgCountColor(Image* image, float);


/*********************************************************/
/*** FUNCOES QUE DEVEM SER IMPLEMENTADAS NO TRABALHO 1 ***/
/*********************************************************/

/**
 *	 Aplica o filtro de Gauss para eliminar o ruido branco da imagem.
 *
 *	@param image Handle para uma imagem que vai receber a imagem filtrada.
 *	@param image Handle para uma imagem que vai ser filtrada.
 *
 */
void imgGauss(Image* img_dst, Image* img_src) ;

/**
 *	 Aplica o filtro de Mediana para eliminar o ruido sal e pimenta da imagem.
 *
 *	@param image Handle para uma imagem a ser filtrada. A resposta esta na própria imagem.
 *
 */
void imgMedian(Image*  image);

/**
 *	 Calcula uma imagem com pixels nas arestas 
 *  da imagem dada.
 *
 *	@param image Handle para uma imagem.
 *
 * @return Handle para a image de luminosidade onde o branco destaca as arestas.
 */
Image* imgEdges(Image* image);

/*
 * Reduz o numero de cores distintas de uma imagem para ncolors.
 *
 * @param image Handle para a imagem original.
 *
 * @return Handle para a imagem que tem apenas ncolors distintas.
 */
Image* imgReduceColors(Image * img, int ncolors);

/************** Transformações Geométricas *************************/
/*																																 */
/* Adicionadas por Roberto Azevedo                                 */
/*******************************************************************/
/*
 * Interpolação bilinear.
 *
 * @param xWeight parametro de interpolação x [0..1]
 * @param yWeight parametro de interpolação y [0..1]
 * @param nw cor a noroeste da posição atual (float[3])
 * @param ne cor a nordeste da posição atual (float[3])
 * @param sw cor a sudoeste da posição atual (float[3])
 * @param se cor a sudeste da posição atual (float[3])
*
 * @param result cor resultante da interpolção bilinear.
 *
 */
void imgBillinearInterpolate(float xWeight, float yWeight, float* nw, 
                             float *ne, float *sw, float *se, float *result);

/*
 * Aplica a transformação geométrica Twirl
 *
 * @param img Handle para a image original.
 * @param xc TODO
 * @param yc TODO
 * @param angle TODO
 * @param rad TODO
 *
 * @return Handle para a imagem deformada.
 */
Image* imgTwirl( Image *img, double xc, double yc, double angle, double rad);

/*
 * Aplica a transformação esférica na imagem.
 *
 * @param img Handle para a image original.
 * @param xc posição x do centro da lente
 * @param yc posição y do centro da lente
 * @param radius raio da lente
 * @param refr índice de refração da lente
 *
 * @return Handle para a imagem deformada.
 */
Image* imgSphere( Image *img, double xc, double yc, double radius,
                  double refr);

/*
 * Aplica a transformação perspectiva (ou projetiva) na imagem.
 *
 * @param img Handle para a image original.
 * @param x0, y0, x1, y1, x2, y2, x3, y3 pontos de controle de destino.
 *
 * @return Handle para a imagem deformada.
 */
Image *imgPerspective(Image *img,
				double x0, double y0,
				double x1, double y1,
				double x2, double y2,
			  double x3, double y3,
				Image_Filter filter);

/*
 * Aplica a transformação perspectiva (ou projetiva) na imagem.
 * (Implementação anterior)
 * 
 * @deprecated
 * @param img Handle para a image original.
 * @param x0, y0, x1, y1, x2, y2, x3, y3 pontos de controle de destino.
 *
 * @return Handle para a imagem deformada.
 */
Image *imgPerspective_Prev(Image *img,
				double x0, double y0,
				double x1, double y1,
				double x2, double y2,
			double x3, double y3);

#endif

