#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void printmask(Mat &m){
  for(int i=0; i<m.size().height; i++){
    for(int j=0; j<m.size().width; j++){
      cout << m.at<float>(i,j) << ",";
    }
    cout << endl;
  }
}

void menu(){
  cout << "\npressione a tecla para ativar o filtro: \n"
      "a - calcular modulo\n"
      "m - media\n"
      "g - gauss\n"
      "v - vertical\n"
      "h - horizontal\n"
      "l - laplaciano\n"
      "z - laplaciano do gaussiano 2\n"
      "p - laplaciano do gaussiano\n"
      "c - adicionar LG\n"
      "d - adicionar LG2\n"
      "f - printar\n"
      "esc - sair\n";
}

int main(int argvc, char** argv){
  VideoCapture video;
    //Mascaras
  float media[] = {1,1,1,
				           1,1,1,
				           1,1,1};
  float gauss[] = {1,2,1,
				           2,4,2,
				           1,2,1};
  float horizontal[]={-1,0,1,
					            -2,0,2,
					            -1,0,1};
  float vertical[]={-1,-2,-1,
					           0, 0, 0,
					           1, 2, 1};
  float laplacian[]={ 0,-1, 0,
					           -1, 4,-1,
					            0,-1, 0};
  float lapgauss[]={0,0,1,0,0,
                     0,1,2,1,0,
                    1,2,-16,2,1,
                     0,1,2,1,0,
                     0,0,1,0,0};
  // lapgauss1[] é um metodo de implementar laplace ultilizando um kernel 5x5 e variando o sigma
  float lapgauss1[]={0,1,1,2,2,2,1,1,0,
                     1,2,4,5,5,5,4,2,1,
                     1,4,5,3,0,3,5,4,1,
                     2,5,3,-12,-24,-12,3,5,2,
                     2,5,0,-24,-40,-24,0,5,2,
                     2,5,3,-12,-24,-12,3,5,2,
                     1,4,5,3,0,3,5,4,1,
                     1,2,4,5,5,5,4,2,1,
                     0,1,1,2,2,2,1,1,0};

  float noMask[] = {0,0,0,
                    0,1,0,
                    0,0,0};

  Mat cap, frame, frame32f, frameFiltered;
  Mat mask(3,3,CV_32F), mask1;
  Mat result, result1;
  double width, height;
  int absolut;
  char key;
  bool lg=0,addLG=0,addLG2=0,lp = 0;

  video.open(0);
  if(!video.isOpened())
    return -1;
  width=video.get(CV_CAP_PROP_FRAME_WIDTH);
  height=video.get(CV_CAP_PROP_FRAME_HEIGHT);
  std::cout << "largura=" << width << "\n";;
  std::cout << "altura =" << height<< "\n";;

  namedWindow("filtroespacial",1);

  mask = Mat(3, 3, CV_32F, media);
  absolut=1; // calcs abs of the image

  menu();
  for(;;){
    video >> cap;
    cvtColor(cap, frame, CV_BGR2GRAY);
    flip(frame, frame, 1);
    imshow("original", frame);
    frame.convertTo(frame32f, CV_32F);
    filter2D(frame32f, frameFiltered, frame32f.depth(), mask, Point(1,1), 0);
    if(absolut){
      frameFiltered=abs(frameFiltered);
    }
    if(lg){
      filter2D(frame32f,frameFiltered,frame32f.depth(),mask,Point(2,2),0);
      if(addLG2) frameFiltered = frameFiltered + frame32f;
    }
      //Aqui implementamos o fitro gaussiano seguido do laplaciano
      /*
       Os parâmetros lp e addLG são variáveis booleanas, cujo valor é alterado quando pressionamos uma tecla. Caso a addLG seja verdadeiro, somamos a matriz filtrada com a matriz original. Utilizamos a matriz mask como máscara de convolução (ou kernel). As máscaras são definidas no começo do código. Como sempre é rodado um filtro com uma máscara pré-estabelecida, definimos uma máscara nula (apenas um elemento com valor 1), que não tem efeito na convolução (como um impulso).
       */
    if(lp){
      mask = Mat(3, 3, CV_32F, gauss);
      scaleAdd(mask, 1/16.0, Mat::zeros(3,3,CV_32F), mask1);
      mask = mask1;
      filter2D(frame32f, frameFiltered, frame32f.depth(), mask, Point(1,1), 0);
      mask = Mat(3, 3, CV_32F, laplacian);
      filter2D(frameFiltered, frameFiltered, frame32f.depth(), mask, Point(1,1), 0);
      if (addLG) frameFiltered = frameFiltered + frame32f;
    }
    frameFiltered.convertTo(result, CV_8U);
    imshow("filtroespacial", result);
    key = (char) waitKey(10);
    if( key == 27 ) break; // esc pressed!
    switch(key){
    case 'a':
	  menu();
      absolut=!absolut;
      break;
    case 'm':
	  menu();
      mask = Mat(3, 3, CV_32F, media);
      scaleAdd(mask, 1/9.0, Mat::zeros(3,3,CV_32F), mask1);
      mask = mask1;
      printmask(mask);
      break;
    case 'g':
	  menu();
      mask = Mat(3, 3, CV_32F, gauss);
      scaleAdd(mask, 1/16.0, Mat::zeros(3,3,CV_32F), mask1);
      mask = mask1;
      printmask(mask);
      break;
    case 'h':
	  menu();
      mask = Mat(3, 3, CV_32F, horizontal);
      printmask(mask);
      break;
    case 'v':
	  menu();
      mask = Mat(3, 3, CV_32F, vertical);
      printmask(mask);
      break;
    case 'l':
	  menu();
      mask = Mat(3, 3, CV_32F, laplacian);
      printmask(mask);
      break;
    case 'z':
      menu();
      mask = Mat(5,5,CV_32F,lapgauss);
      printmask(mask);
      lg = !lg;
      break;
    case 'c':
      menu();
      mask = Mat(3,3,CV_32F,noMask);
      addLG = !addLG;
      lp = addLG;
      break;
    case 'p':
      menu();
      mask = Mat(3,3,CV_32F,noMask);
      lp = !lp;
      break;
    case 'd':
      menu();
      mask = Mat(3,3,CV_32F,noMask);
      addLG2 = !addLG2;
      lg = addLG2;
      break;
    case 'f':
      imwrite("in.png",frame);
      imwrite("out.png",result);
    default:
      break;
    }
  }
  return 0;
}
