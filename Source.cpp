////////////////////////////////Cabeceras/////////////////////////////////////
#define _USE_MATH_DEFINES
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
/////////////////////////////////////////////////////////////////////////////

cv::Mat escalaPorPromedio(int fila_original, int columna_original, cv::Mat imagen);
cv::Mat Sobel(cv::Mat imagenBN, int bandera);
cv::Mat MagnitudSobel(cv::Mat Gx, cv::Mat Gy, int fila_original, int columna_original);
cv::Mat expandirMatriz(cv::Mat imagenBN, int DimensionesFiltro);
cv::Mat filtrarImagen(cv::Mat imagenBNexpandida, float** filtro, int filaOriginal, int columnaOriginal, int filasFiltro);
cv::Mat filtrarImagen(cv::Mat imagenBNexpandida, int** filtro, int filaOriginal, int columnaOriginal, int filasFiltro);
float** Gauss(int dimension, float sigma);
cv::Mat ecualizarImagen(cv::Mat imagenBN);
cv::Mat DireccionSobel(cv::Mat Gx, cv::Mat Gy, int fila_original, int columna_original);
cv::Mat Canny(cv::Mat imagenBNSuavizada);


///////////////////////////////Espacio de nombres////////////////////////////
using namespace cv;
using namespace std;
/////////////////////////////////////////////////////////////////////////////


/////////////////////////Inicio de la funcion principal///////////////////
int main()
{

	/********Declaracion de variables generales*********/
	char NombreImagen[] = "./../src/lenna_main.jpg";
	Mat imagen; // Matriz que contiene nuestra imagen sin importar el formato
	
	
	/************************/
	float sigma;
	int dimensionFiltro;
	float** filtroGauss;

	cout << "tamano del filtro"; cin >> dimensionFiltro;
	cout << "valor de sigma"; cin >> sigma;

	cout << "tamano del filtro: " << dimensionFiltro;

	/*********Lectura de la imagen*********/
	imagen = imread(NombreImagen);

	if (!imagen.data)
	{
		cout << "Error al cargar la imagen: " << NombreImagen << endl;
		exit(1);
	}
	/************************/

	/************Procesos*********/
	int fila_original = imagen.rows;
	int columna_original = imagen.cols;//Lectur de cuantas columnas

	//imagen en blanco y negro
	Mat imagenBN(escalaPorPromedio(fila_original, columna_original, imagen));
	//cvtColor(imagen, imagenBN, COLOR_BGR2GRAY);



	//creamos el filtro agussiano
	filtroGauss = Gauss(dimensionFiltro, sigma);

	for (int i = 0; i < dimensionFiltro; i++) {
		for (int j = 0; j < dimensionFiltro; j++) {
			cout << filtroGauss[i][j] << "\t";
		}
		cout << "\n";
	}
	
	//expandimos la imagen
	
	Mat imagenBNExpandida(expandirMatriz(imagenBN, dimensionFiltro));

	//suavizamos la imagen en blanco y negro
	Mat imagenBNSuavizada(filtrarImagen(imagenBNExpandida, filtroGauss, fila_original, columna_original, dimensionFiltro));

	//Ecualizamos la imagen
	Mat ecualizada(ecualizarImagen(imagenBN));
	//equalizeHist(imagenBN, ecualizada);

	//Aplicamos filtrado de sobel y obtenemos |G|
	Mat SobelX(Sobel(imagenBN, 0));
	Mat SobelY(Sobel(imagenBN, 1));

	Mat sobelMagnitud(MagnitudSobel(SobelX, SobelY, fila_original, columna_original));


	//aplicamos Canny
	Mat filtroCanny(Canny(imagenBNSuavizada));
	/************************/

	namedWindow("original", WINDOW_AUTOSIZE);//Creación de una ventana
	imshow("original", imagen);

	cout << "imagen Original: " << fila_original << " x " << columna_original << endl;

	namedWindow("blanco y negro", WINDOW_AUTOSIZE);//Creación de una ventana
	imshow("blanco y negro", imagenBN);

	cout << "imagen a escala de grises: " << imagenBN.rows << " x " << imagenBN.cols << endl;

	namedWindow("expandida", WINDOW_AUTOSIZE);//Creaci n de una ventana
	imshow("expandida", imagenBNExpandida);

	cout << "imagen expandida : " << imagenBNExpandida.rows << " x " << imagenBNExpandida.cols << endl;

	namedWindow("suavizada (Gauss)", WINDOW_AUTOSIZE);//Creación de una ventana
	imshow("suavizada (Gauss)", imagenBNSuavizada);

	cout << "imagen bajo filtro Gauss: " << imagenBNSuavizada.rows << " x " << imagenBNSuavizada.cols << endl;

	namedWindow("Sobel", WINDOW_AUTOSIZE);//Creación de una ventana
	imshow("Sobel", sobelMagnitud);

	cout << "imagen a bajo |G|: " << sobelMagnitud.rows << " x " << sobelMagnitud.cols << endl;

	namedWindow("ecualizada", WINDOW_AUTOSIZE);//Creación de una ventana
	imshow("ecualizada", ecualizada);

	cout << "imagen ecualizada: " << ecualizada.rows << " x " << ecualizada.cols << endl;
	
	namedWindow("SobelGx", WINDOW_AUTOSIZE);//Creación de una ventana
	imshow("SobelGx", SobelX);
	
	namedWindow("SobelGy", WINDOW_AUTOSIZE);//Creación de una ventana
	imshow("SobelGy", SobelY);

	namedWindow("canny", WINDOW_AUTOSIZE);//Creación de una ventana
	imshow("canny", filtroCanny);
	
	cout << "imagen filtro Canny: " << filtroCanny.rows << " x " << filtroCanny.cols << endl;

	waitKey(0); //Función para esperar
	return 1;
}

cv::Mat escalaPorPromedio(int fila_original, int columna_original, cv::Mat imagen) {
	int val_es_prom;
	Mat grises_prom(fila_original, columna_original, CV_8UC1);

	for (int i = 0; i < fila_original; i++) {
		for (int j = 0, k = 0; j < columna_original * 3; j += 3, k++) {
			val_es_prom = ((int)imagen.at<uchar>(Point(j, i)) + (int)imagen.at<uchar>(Point(j + 1, i)) + (int)imagen.at<uchar>(Point(j + 2, i))) / 3;
			grises_prom.at<uchar>(Point(k, i)) = uchar(val_es_prom);
			
		}
	}
	return grises_prom;
	//imshow("escala", grises_prom);
}

cv::Mat Sobel(cv::Mat imagenBN, int bandera) {
	int **Gx, **Gy;
	

	Gx = (int**)malloc(3 * sizeof(int*));
	Gy = (int**)malloc(3 * sizeof(int*));

	for (int i = 0; i < 3; i++) {
		Gx[i] = (int*)malloc(3 * sizeof(int));
		Gy[i] = (int*)malloc(3 * sizeof(int));
	}


	//creamos los filtros de sobel
	Gx[0][0] = -1;
	Gx[0][1] = 0;
	Gx[0][2] = 1;
	Gx[1][0] = -2;
	Gx[1][1] = 0;
	Gx[1][2] = 2;
	Gx[2][0] = -1;
	Gx[2][1] = 0;
	Gx[2][2] = 1;

	Gy[0][0] = -1;
	Gy[0][1] = -2;
	Gy[0][2] = -1;
	Gy[1][0] = 0;
	Gy[1][1] = 0;
	Gy[1][2] = 0;
	Gy[2][0] = 1;
	Gy[2][1] = 2;
	Gy[2][2] = 1;
	//Mat imagenExpandida(expandirMatriz(imagenBN, filtro));
	//expandimos la matriz nuevamente
	Mat imagenBNExpandida(expandirMatriz(imagenBN, 3));
	//aplicamos el filtro de sobel
	if (bandera == 0) {
		Mat filtroSobel(filtrarImagen(imagenBNExpandida, Gx, imagenBN.rows, imagenBN.cols, 3));
		return filtroSobel;
	}
	else {
		Mat filtroSobel(filtrarImagen(imagenBNExpandida, Gy, imagenBN.rows, imagenBN.cols, 3));
		return filtroSobel;
	}
	
}

cv::Mat MagnitudSobel(cv::Mat Gx, cv::Mat Gy, int fila_original, int columna_original) {
	Mat resultado(fila_original, columna_original, CV_8UC1);
	double raiz;

	for (int i = 0; i < fila_original; i++) {
		for (int j = 0; j < columna_original; j++) {
			raiz = sqrt(pow((double)Gx.at<uchar>(Point(j, i)), 2) + pow((double)Gy.at<uchar>(Point(j, i)), 2));
			resultado.at<uchar>(Point(j, i)) = (uchar)(raiz);
		}
	}
	return resultado;
}

cv::Mat DireccionSobel(cv::Mat Gx, cv::Mat Gy, int fila_original, int columna_original) {
	Mat resultado(fila_original, columna_original, CV_8UC1);
	double arco;

	for (int i = 0; i < fila_original; i++) {
		for (int j = 0; j < columna_original; j++) {
			arco = atan((double)Gy.at<uchar>(Point(j, i)) / (double)Gx.at<uchar>(Point(j, i)));
			resultado.at<uchar>(Point(j, i)) = (uchar)(arco);
		}
	}
	return resultado;
}

float** Gauss(int dimension, float sigma) {

	int filas_filtro = dimension;
	int limiteFilasFiltro = (filas_filtro - 1) / 2;
	Mat nuevoFiltro(filas_filtro, filas_filtro, CV_8UC1);
	float** matriz = (float**)malloc(filas_filtro * sizeof(float*));

	for (int i = 0; i < filas_filtro; i++) {
		matriz[i] = (float*)malloc(filas_filtro * sizeof(float));
	}

	//int i = 0, j = 0;

	//cout << "mi filtro mientras se va creando, va imprimiendo conforme va agregando los valores al filtro\n\n";
	for (int x = -limiteFilasFiltro, i = 0; x <= limiteFilasFiltro; x++, i++) {
		for (int y = -limiteFilasFiltro, j = 0; y <= limiteFilasFiltro; y++, j++) {
			matriz[i][j] = exp(-((pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2)))) / (2 * M_PI * pow(sigma, 2));
			//cout << matriz[i][j] << '\t';
			//cout << "(" << i << "," << j << ")";
		}
		//cout << '\n';
	}
	/*cout << "mi filtro volviendolo a imprimir una vez que termino de agregar los datos al mismo\n\n";
	for (int i = 0; i < filtro.rows; i++) {
		for (int j = 0; j < filtro.cols; j++) {
			cout << matriz[i][j] << '\t';
		}
		cout << "\n";
	}

	cout << "\nA partir de aqui son otras veces que imprimos el filtro, todas son iguales\n\n";*/

	return matriz;
}

cv::Mat expandirMatriz(cv::Mat imagenBN, int DimensionesFiltro) {
	int expansionFilas = imagenBN.rows + DimensionesFiltro - 1;
	int expansionColumnas = imagenBN.cols + DimensionesFiltro - 1;

	Mat matrizExpandida(expansionFilas, expansionColumnas, CV_8UC1, Scalar(0));

	for (int i = 0; i < expansionFilas; i++) {
		for (int j = 0; j < expansionColumnas; j++) {
			if ((i > DimensionesFiltro - 1 && j > DimensionesFiltro - 1) && (i <= imagenBN.rows && j <= imagenBN.cols)) {
				matrizExpandida.at<uchar>(Point(j, i)) = imagenBN.at<uchar>(Point(j, i));
			}
			//cout << static_cast<int>(matrizExpandida.at<uchar>(Point(j, i))) << '\t';
		}
		//cout << '\n';
	}

	//cout << "imagen expandida: " << matrizExpandida.rows << "x" << matrizExpandida.cols << endl;

	return matrizExpandida;
}

cv::Mat filtrarImagen(cv::Mat imagenBNexpandida, float** filtro, int filaOriginal, int columnaOriginal, int filasFiltro) {
	Mat filtrada(filaOriginal, columnaOriginal, CV_8UC1);
	//Mat filtroPrueba(filtro.rows, filtro.cols, CV_8UC1, Scalar(1));

	float convolucion = 0;
	int x = 0, y = 0;

	/*for (int i = 0; i < filtro.rows; i++) {
		for (int j = 0; j < filtro.cols; j++) {
			cout << filtro.at<float>(Point(j, i)) <<"\t";
		}
		cout << "\n";
	}*/

	for (int i = 0; i < filaOriginal; i++) {
		for (int j = 0; j < columnaOriginal; j++) {


			for (int k = 0, x = i; k < filasFiltro; k++, x++) {
				for (int l = 0, y = j; l < filasFiltro; l++, y++) {
					convolucion += float(imagenBNexpandida.at<uchar>(Point(y, x))) * filtro[k][l];
				}
			}
			//cout << convolucion << "\t";
			filtrada.at<uchar>(Point(j, i)) = uchar(convolucion);
			convolucion = 0;
		}
	}

	//namedWindow("gaussFiltro", WINDOW_AUTOSIZE);//Creaci n de una ventana
	//imshow("gaussFiltro", filtrada);

	//cout << "imagen filtrada: " << filtrada.rows << "x" << filtrada.cols << endl;
	return filtrada;
}

cv::Mat filtrarImagen(cv::Mat imagenBNexpandida, int** filtro, int filaOriginal, int columnaOriginal, int filasFiltro) {
	Mat filtrada(filaOriginal, columnaOriginal, CV_8UC1);
	//Mat filtroPrueba(filtro.rows, filtro.cols, CV_8UC1, Scalar(1));

	int convolucion = 0;
	int x = 0, y = 0;

	/*for (int i = 0; i < filtro.rows; i++) {
		for (int j = 0; j < filtro.cols; j++) {
			cout << filtro.at<float>(Point(j, i)) <<"\t";
		}
		cout << "\n";
	}*/

	for (int i = 0; i < filaOriginal; i++) {
		for (int j = 0; j < columnaOriginal; j++) {


			for (int k = 0, x = i; k < filasFiltro; k++, x++) {
				for (int l = 0, y = j; l < filasFiltro; l++, y++) {
					convolucion += int(imagenBNexpandida.at<uchar>(Point(y, x))) * filtro[k][l];
				}
			}
			//cout << convolucion << "\t";
			filtrada.at<uchar>(Point(j, i)) = uchar(convolucion);
			convolucion = 0;
		}
	}

	//namedWindow("gaussFiltro", WINDOW_AUTOSIZE);//Creaci n de una ventana
	//imshow("gaussFiltro", filtrada);

	//cout << "imagen filtrada: " << filtrada.rows << "x" << filtrada.cols << endl;
	return filtrada;
}


cv::Mat ecualizarImagen(cv::Mat imagenBN) {
	int suma = 0;
	int contador = 0;
	int colorOriginal = 0;
	int* histograma, *sumah;
	double valorConstante = 0;
	double tono=0;
	double auxiliar;
	Mat ecualizacion(imagenBN.rows, imagenBN.cols, CV_8UC1);

	histograma = (int*)malloc(256 * sizeof(int));
	sumah = (int*)malloc(256 * sizeof(int));

	for (int i = 0; i < 256; i++) {
		
		contador = 0;

		for (int j = 0; j < imagenBN.rows; j++) {
			for (int k = 0; k < imagenBN.cols; k++){

				if ((int)imagenBN.at<uchar>(Point(k, j)) == i) {
					contador++;
				}

			}
		}
		histograma[i] = contador;

	}

	for (int i = 0; i < 256; i++){
		
		suma += histograma[i];
		sumah[i] = suma;
	}

	//valorConstante = suma / (imagenBN.rows * imagenBN.cols);
	//cout <<  sumah[255] << endl;
	//cout << histograma[125] << endl;

	for (int i = 0; i < imagenBN.rows; i++) {
		for (int j = 0; j < imagenBN.cols; j++) {

			colorOriginal = (int)imagenBN.at<uchar>(Point(j, i));

			int divisor = sumah[255];
			tono = (sumah[colorOriginal] * 255/ 317471);

			/*
			if (i == 125 && j==125){
				cout << "El valor de la sumah actual es: " << sumah[colorOriginal] << endl;
				cout << "El tono actual es: " << tono << endl;
			}*/
			//if (tono > 255) {
				//tono = 255;
			//}

			ecualizacion.at<uchar>(Point(j, i)) = (uchar)tono;
			//auxiliar = (double)imagenBN.at<uchar>(Point(j, i)) * valorConstante;
			//ecualizacion.at<uchar>(Point(j, i)) = (uchar)auxiliar;
		}
	}

	//cout << tono << endl;

	return ecualizacion;
}

cv::Mat Canny(cv::Mat imagenBNSuavizada) {


	Mat canny(imagenBNSuavizada.rows, imagenBNSuavizada.cols, CV_8UC1);

	//ecualizamos la imagen
	Mat ecualizada(ecualizarImagen(imagenBNSuavizada));
	//a la imagen suavizada le aplicamos Sobel
	Mat sobelX(Sobel(ecualizada, 0));
	Mat sobelY(Sobel(ecualizada, 1));

	Mat magnitudSobel(MagnitudSobel(sobelX, sobelY, imagenBNSuavizada.rows, imagenBNSuavizada.cols));
	Mat direccionSobel(DireccionSobel(sobelX, sobelY, imagenBNSuavizada.rows, imagenBNSuavizada.cols));

	for (int i = 0; i < imagenBNSuavizada.rows; i++){
		for (int j = 0; j < imagenBNSuavizada.cols; j++){
			canny.at<uchar>(Point(j, i)) = magnitudSobel.at<uchar>(Point(j, i)) * direccionSobel.at<uchar>(Point(j, i));
		}
	}

	return canny;
}

/////////////////////////////////////////////////////////////////////////