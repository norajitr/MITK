/*=========================================================================

Program:   Tensor ToolKit - TTK
Module:    $URL: svn://scm.gforge.inria.fr/svn/ttk/trunk/Algorithms/itkTensorImageToDiffusionImageFilter.txx $
Language:  C++
Date:      $Date: 2010-06-07 13:39:13 +0200 (Mo, 07 Jun 2010) $
Version:   $Revision: 68 $

Copyright (c) INRIA 2010. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itk_FreeWaterEliminationFilter_txx_
#define _itk_FreeWaterEliminationFilter_txx_
#endif
#include "itkImageRegionConstIterator.h"


//vnl includes
#include <C:\toolkits\mitk\bin2\MITK-superbuild\ITK-src\Utilities\vxl\core\vnl\vnl_vector_fixed.h>
#include <C:\toolkits\mitk\bin2\MITK-superbuild\ITK-src\Utilities\vxl\core\vnl\vnl_vector.h>
#include <C:\toolkits\mitk\bin2\MITK-superbuild\ITK-src\Utilities\vxl\core\vnl\vnl_matrix.h>
#include <C:\toolkits\mitk\bin2\MITK-superbuild\ITK-src\Utilities\vxl\core\vnl\algo\vnl_svd.h>
#include <C:\toolkits\mitk\bin2\MITK-superbuild\ITK-src\Utilities\vxl\core\vnl\algo\vnl_generalized_eigensystem.h>
#include <math.h>
#include <C:\toolkits\mitk\bin2\MITK-superbuild\ITK-src\Utilities\vxl\core\vnl\algo\vnl_symmetric_eigensystem.h>


namespace itk
{
  

  template <class TDiffusionPixelType, class TTensorPixelType>
  FreeWaterEliminationFilter<TDiffusionPixelType, TTensorPixelType>
  ::FreeWaterEliminationFilter()
  {
  

  }



  template <class TDiffusionPixelType, class TTensorPixelType>
  void
    FreeWaterEliminationFilter<TDiffusionPixelType, TTensorPixelType>
    ::GenerateData ( )
  {

    typedef ImageRegionConstIterator< GradientImagesType > GradientIteratorType;
    typedef typename GradientImagesType::PixelType         GradientVectorType;
    typename GradientImagesType::Pointer gradientImagePointer = NULL;

    // Would have liked a dynamic_cast here, but seems SGI doesn't like it
    // The enum will ensure that an inappropriate cast is not done
    gradientImagePointer = static_cast< GradientImagesType * >(
                              this->ProcessObject::GetInput(0) );

    // Size of the complete image
    typename GradientImagesType::SizeType size = gradientImagePointer->GetLargestPossibleRegion().GetSize();
    
    int nof = m_GradientDirectionContainer->Size();

    int cnt=0;
 
    for(int i=0; i<nof; i++)
    {
      vnl_vector_fixed< double, 3 > vec = m_GradientDirectionContainer->ElementAt(i);

      
      if(vec[0]<0.0001 && vec[1]<0.0001 && vec[2]<0.0001)
      {
        cnt++;
      }
      
    }
    
    int numberb0=cnt;
    
    
    
    
    


    int b= 1000;
    int in_g=0;
    int in_b=0;
    int temporaryb0sum=0;
    int cnt_dir=0;
            
    int etbt[6] = { 0, 4, 8, 1, 5, 2 };
    float temporary[3];
    float tensor[9];
    int badnumberpresent=0;
    int badnumberpast=0;
    int wasdetectedbad=0;
    int LOW_D=0.01;
    int HIGH_D = 5;
    int oldnumberofbads=1000000000000;
    int newnumberofbads;
    int diff=1;
    
   
    int flag;

    
    vnl_matrix<float> H(nof-numberb0, 6);
    vnl_matrix<float> tem_tensor(3,3);

   
  

    vnl_vector< float> higher_thresh(nof-numberb0);
    vnl_vector< float> lower_thresh(nof-numberb0 );
    vnl_vector< float> diagonal(nof-numberb0);
    vnl_vector< float> b_vec(nof-numberb0);
    vnl_vector< float > b0index(nof);


    vnl_matrix<float> directions(nof-numberb0,3);




    int cnt_d=0;
    for(int i=0; i<nof; i++)
    {
      vnl_vector_fixed< double, 3 > vec = m_GradientDirectionContainer->ElementAt(i);

      
      if(vec[0]<0.0001 && vec[1]<0.0001 && vec[2]<0.0001)
      {
        b0index[i]=i;
        
      }
      else
      {
        directions[cnt_d][0]=vec[0];
        directions[cnt_d][1]=vec[1];
        directions[cnt_d][2]=vec[2];
        b0index[i]=-1;
        
      }
    }
    
     
   

   int ****table;// table in which all images except b0 are store
    table = new int***[nof-numberb0];
    for (int g=0; g<nof-numberb0; g++)
    {
      table[g] = new int**[size[0]];
      for ( int i = 0 ; i < size[0] ; i++ ) 
      {
        table[g][i] = new int*[size[1]] ;
        for ( int j = 0 ; j < size[1] ; j++ )
        {
          table[g][i][j] = new int[size[2]] ;
        }
      }
    }


    int ****table1;// table in which all images except b0 are store
    table1 = new int***[nof-numberb0];
    for (int g=0; g<nof-numberb0; g++)
    {
      table1[g] = new int**[size[0]];
      for ( int i = 0 ; i < size[0] ; i++ ) 
      {
        table1[g][i] = new int*[size[1]] ;
        for ( int j = 0 ; j < size[1] ; j++ )
        {
          table1[g][i][j] = new int[size[2]] ;
        }
      }
    }

      int ****b0image;// table with all b0images
    b0image = new int***[numberb0];
    for (int g=0; g<numberb0; g++)
    {
      b0image[g] = new int**[size[0]];
      for ( int i = 0 ; i < size[0] ; i++ ) 
      {
        b0image[g][i] = new int*[size[1]] ;
        for ( int j = 0 ; j < size[1] ; j++ )
        {
          b0image[g][i][j] = new int[size[2]] ;
        }
      }
    }
      
    in_g=0;
    in_b=0;

      for(int x=0; x<size[0]; x++)
      {
        for(int y=0; y<size[1]; y++)
        {
          for(int z=0; z<size[2]; z++)
          {	

            itk::Index<3> ix;
            ix[0] = x;
            ix[1] = y;
            ix[2] = z;

            GradientVectorType pixel2 = gradientImagePointer->GetPixel(ix);

             for(int g=0; g<nof; g++)
               {
                if(g == b0index[g]){flag=1;}
                else{flag=0;}
               
                
               if(flag==0)
               {
                table[in_g][x][y][z] = pixel2[g];// data without b0
                in_g++;
               }
               else
               {
               b0image[in_b][x][y][z] = pixel2[g];// only b0 image
               in_b++;
              
              }
          
        }
             in_g=0;
             in_b=0;
      }
    }
  }


    int ***b0_mean_image;// table with mean b0 image
    b0_mean_image = new int**[size[0]];
    for (int g=0; g<size[0]; g++)
    {
      b0_mean_image[g] = new int*[size[1]];
      for ( int i = 0 ; i < size[1] ; i++ ) 
      {
        b0_mean_image[g][i] = new int[size[2]] ;
       
      }
    }
    
    for(int x =0;x<size[0];x++)
    {
        for(int y=0;y<size[1];y++)
        {
            for( int z=0;z<size[2];z++)
            {
                temporaryb0sum=0;
                for (int i=0;i<numberb0;i++)
                {
                    temporaryb0sum=temporaryb0sum+b0image[i][x][y][z];         
                }
                b0_mean_image[x][y][z]=temporaryb0sum/numberb0;  
            }
        }
    }

    int ****sorted_eigs;// table in which all images except b0 are stored
    sorted_eigs = new int***[3];
    for (int g=0; g<3; g++)
    {
      sorted_eigs[g] = new int**[size[0]];
      for ( int i = 0 ; i < size[0] ; i++ ) 
      {
        sorted_eigs[g][i] = new int*[size[1]] ;
        for ( int j = 0 ; j < size[1] ; j++ )
        {
          sorted_eigs[g][i][j] = new int[size[2]] ;
        }
      }
    }
    
    int ****atten;// table in which all images except b0 are stored
    atten = new int***[nof-numberb0];
    for (int g=0; g<nof-numberb0; g++)
    {
      atten[g] = new int**[size[0]];
      for ( int i = 0 ; i < size[0] ; i++ ) 
      {
        atten[g][i] = new int*[size[1]] ;
        for ( int j = 0 ; j < size[1] ; j++ )
        {
          atten[g][i][j] = new int[size[2]] ;
        }
      }
    }
    
    int ****D;
    D = new int***[6];
    for (int g=0; g<6; g++)
    {
      D[g] = new int**[size[0]];
      for ( int i = 0 ; i < size[0] ; i++ ) 
      {
        D[g][i] = new int*[size[1]] ;
        for ( int j = 0 ; j < size[1] ; j++ )
        {
          D[g][i][j] = new int[size[2]] ;
        }
      }
    }
    
  
    
 

  

    cnt_dir=0;

    for(int g=0; g<nof; g++)
    {
      GradientDirectionType gradient = m_GradientDirectionContainer->ElementAt(g);

      
               
                   
        if(gradient[0]>0.0001 || gradient[1]>0.0001 || gradient[2]>0.0001)
        {
           directions[cnt_dir][0] = gradient[0];
           directions[cnt_dir][1] = gradient[1];
           directions[cnt_dir][2] = gradient[2];
           
           cnt_dir++;
        }
                 
     

   }// collection of all gradient directions except b0

  
   

 

  vnl_matrix<float> dirsTimesDirsTrans = directions*directions.transpose();




  for (int i=0;i<nof-numberb0;i++)
  {    
    diagonal[i]=dirsTimesDirsTrans[i][i];    
  }
              
  float max_diagonal = diagonal.max_value();//15:31 ok
   
  directions=directions*((float)b/1000.0)*(1.0/sqrt(max_diagonal));

  dirsTimesDirsTrans = directions*directions.transpose();

             
  for (int i=0;i<nof-numberb0;i++)
  {   
    b_vec[i]= dirsTimesDirsTrans[i][i];   
    std::cout << b_vec[i] << "\n";
  }
  std::cout << std::endl;

  for (int i = 0; i < nof-numberb0; i++) 
  {
    for (int j = 0; j < 3; j++) {temporary[j] = -directions[i][j];}
 
    for (int j = 0; j < 3; j++) 
    {
      for (int k = 0; k < 3; k++) {tensor[k + 3 * j] = temporary[k] * directions[i][j];}
    }
    for (int j = 0; j < 6; j++) {H[i][j] = tensor[etbt[j]];}
    for (int j = 0; j < 3; j++) {H[i][3 + j] *= 2.0;}
  }

 
 
  vnl_matrix<float> inputtopseudoinverse=H.transpose()*H;

  


  //vnl_svd<float> h_svd(H);
  vnl_symmetric_eigensystem<float> eig( inputtopseudoinverse);
  vnl_matrix<float> pseudoInverse = eig.pinverse()*H.transpose();

  for(int i = 0; i < nof-numberb0; i++)
  {
    for(int j = 0; j < nof-numberb0; j++)
    {
      std::cout << pseudoInverse[i][j] << " ";
    }
    std::cout << std::endl;
  }

  int negativitycheck=0;
   for ( int x=0;x<size[0];x++)
    {
      for ( int y=0;y<size[1];y++)
      {
        for ( int z=0;z<size[2];z++)
        { 
          if(b0_mean_image[x][y][z]<0)
          {
            negativitycheck++;
          }
        }
      }
    }

   for ( int x=0;x<size[0];x++)
    {
      for ( int y=0;y<size[1];y++)
      {
        for ( int z=0;z<size[2];z++)
        { 
          if(b0_mean_image[x][y][z]<0)
          {
          check_the_neighbors(x,y,z,b0_mean_image,size);
          }
        }
      }
    }
    int negativitycheckafter=0;

    for ( int x=0;x<size[0];x++)
    {
      for ( int y=0;y<size[1];y++)
      {
        for ( int z=0;z<size[2];z++)
        { 
          if(b0_mean_image[x][y][z]<0)
          {
            negativitycheckafter++;
          }
        }
      }
    }
    std::cout << "before: " << negativitycheck << "\nafter: " << negativitycheckafter << std::endl;
  
   /*for ( int g=0;g<nof-numberb0;g++)
  {
    for ( int x=0;x<size[0];x++)
    {
      for ( int y=0;y<size[1];y++)
      {
        for ( int z=0;z<size[2];z++)
        { 
          if(table[g][x][y][z]<=0)
          {
          check_the_neighbors(x,y,z,table[g],size);
          }
        }
      }
    }
  }// removing voxels smaller than 0 from the data / i am not using this threshold for this step cause in my opinion its sensles - if data are below zero it would be below threshold for sure.



 


  for (int i=1;i<nof-numberb0;i++)
   {
      higher_thresh[i]=exp(-b_vec[i]*LOW_D);
      lower_thresh[i]= exp(-b_vec[i]*HIGH_D);
   }

   while(diff>0)
   {

     for ( int x=0;x<size[0];x++)
     {
         for ( int y=0;y<size[1];y++)
         {
             for ( int z=0;z<size[2];z++)
             {
                 remove_negative_eigs(x,y,z,table,nof,size,H,D,atten,higher_thresh,lower_thresh, wasdetectedbad,b0_mean_image,sorted_eigs,numberb0);
             }
         }
     }
     newnumberofbads=wasdetectedbad;
     diff=oldnumberofbads-newnumberofbads;
     oldnumberofbads=wasdetectedbad;
   }

*/
  }// end of class

 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class TDiffusionPixelType, class TTensorPixelType>
  void
    FreeWaterEliminationFilter<TDiffusionPixelType, TTensorPixelType>
  ::SetGradientImage( GradientDirectionContainerType *gradientDirection,
                          const GradientImagesType *gradientImage )
  {
    // Make sure crazy users did not call both AddGradientImage and
    // SetGradientImage
    if( m_GradientImageTypeEnumeration == GradientIsInManyImages )
      {
      itkExceptionMacro( << "Cannot call both methods:"
      << "AddGradientImage and SetGradientImage. Please call only one of them.");
      }

    this->m_GradientDirectionContainer = gradientDirection;

    
    unsigned int numImages = gradientDirection->Size();
    this->m_NumberOfBaselineImages = 0;
/* normalization
    for(GradientDirectionContainerType::Iterator it = this->m_GradientDirectionContainer->Begin();
        it != this->m_GradientDirectionContainer->End(); it++)
      {
      if(it.Value().one_norm() <= 0.0)
        {
        this->m_NumberOfBaselineImages++;
        }
      else // Normalize non-zero gradient directions
        {
        it.Value() = it.Value() / it.Value().two_norm();
        }
      }
*/
    this->m_NumberOfGradientDirections = numImages - this->m_NumberOfBaselineImages;

    // ensure that the gradient image we received has as many components as
    // the number of gradient directions
    if( gradientImage->GetVectorLength() != this->m_NumberOfBaselineImages + this->m_NumberOfGradientDirections )
      {
      itkExceptionMacro( << this->m_NumberOfGradientDirections << " gradients + " << this->m_NumberOfBaselineImages
                         << "baselines = " << this->m_NumberOfGradientDirections + this->m_NumberOfBaselineImages
                         << " directions specified but image has " << gradientImage->GetVectorLength()
        << " components.");
      }

    this->ProcessObject::SetNthInput( 0,
        const_cast< GradientImagesType* >(gradientImage) );
    m_GradientImageTypeEnumeration = GradientIsInASingleImage;
  }

  template <class TDiffusionPixelType, class TTensorPixelType>
  void
  FreeWaterEliminationFilter<TDiffusionPixelType, TTensorPixelType>
  ::check_the_neighbors(int x, int y, int z, int ***table, itk::Size<3> size)   
  {
   
     int temp_sum=0;
     int temp_number=0;
     int init_i;
     int init_j;
     int init_c;
     int limit_i;
     int limit_j;
     int limit_c;
    
     if(x==0)
     {
       init_i=x;
       limit_i=x+2;
     }
     else if(x==size[0])
     {
       init_i=x-1;
       limit_i=x+1;
     }
     else
     {
       init_i=x-1;
       limit_i=x+2;
     }
     
     for (int i=init_i;i<limit_i;i+=2)
         
     {// 1
       if(y==0)
          {
             init_j=y;
             limit_j=y+2;
           }
       else if(y==size[1])
           {
              init_j=y-1;
              limit_j=y+1;
            }
       else
           {
              init_j=y-1;
              limit_j=y+2;
            }

        for (int j=init_j;j<limit_j;j+=2)
        {//2

           if(z==0)
              {
               init_c=z;
               limit_c=z+2;
               }
           else if(z==size[2])
              {
                init_c=z-1;
                limit_c=z+1;
              }
           else
               {
                init_c=z-1;
                limit_c=z+2;
                }
           for (int c=init_c;c<limit_c;c+=2)
           {//3
               
               
               if(table[i][j][c]<=0)
               {
                temp_sum=temp_sum+ 0;
               }
               else
               {
                 temp_sum=temp_sum+table[i][j][c];
                 temp_number++;
               }
            }//3
         }//2
    }//1

   if (temp_number==0)
    {
    table[x][y][z]=0;
    }
     else
    {
      table[x][y][z]=temp_sum/temp_number;
     }
   

   }// end of void

  template <class TDiffusionPixelType, class TTensorPixelType>
  void 
  FreeWaterEliminationFilter<TDiffusionPixelType, TTensorPixelType>
  ::remove_negative_eigs(int x, int y, int z, int ****table, 
                            int nof, itk::Size<3> size, 
                            vnl_matrix<float> pseudoInverse, int ****D, int ****atten, 
                            vnl_vector<float> higher_thresh, 
                            vnl_vector<float> lower_thresh, int wasdetectedbad, 
                            int ***b0_mean_image, int ****sorted_eigs, int numberb0)
  {//0
   int temp_sum=0;
   int temp_number=0;
   int init_i;
   int init_j;
   int init_c;
   int limit_i;
   int limit_j;
   int limit_c;
   int temp_product;
   int flag;
   vnl_matrix<float> temporary(nof-1,3);

   vnl_matrix<int> temp_tensor(3,3);

   vnl_vector<float> atten_log(nof-numberb0);
   vnl_vector<float> temp_atten(nof-numberb0);
   vnl_vector<float> temp_D(6);
   vnl_vector<int> eigen_vals(3);
   
   for (int u=0;u<nof-numberb0;u++)
   {
     atten[u][x][y][z]=table[u][x][y][z]/b0_mean_image[x][y][z];
     atten_log[u]=log((float)atten[u][x][y][z]);
     temp_atten[u]=atten[u][x][y][z];
   }
   temp_D = pseudoInverse*temp_atten;
   
   for (int u=0;u<6;u++){D[u][x][y][z]=temp_D[u];}
   
   /*int a= temp_D[0];
   int aa= temp_D[3];
   int aaa= temp_D[5];
   int b= temp_D[3];
   int bb= temp_D[1];
   int bbb= temp_D[4];
   int d= temp_D[5];
   int dd= temp_D[4];
   int ddd= temp_D[2];/// check it- 16.02 its ok.
   
   
   int l1;
   int l2;
   int l3;*/

   temp_tensor[0][0]= temp_D[0];
   temp_tensor[1][0]= temp_D[3];
   temp_tensor[2][0]= temp_D[5];
   temp_tensor[0][1]= temp_D[3];
   temp_tensor[1][1]= temp_D[1];
   temp_tensor[2][1]= temp_D[4];
   temp_tensor[0][2]= temp_D[5];
   temp_tensor[1][2]= temp_D[4];
   temp_tensor[2][2]= temp_D[2];/// check it- 16.02 its ok.


 

   
   //vnl_symmetric_eigensystem <int> eigen_tensor(temp_tensor);
   
   /*
   eigen_vals[0]=eigen_tensor.get_eigenvalue(1);
   eigen_vals[1]=eigen_tensor.get_eigenvalue(2);
   eigen_vals[2]=eigen_tensor.get_eigenvalue(3);

   /*for (int i=0;i<3;i++)
   {
     
       
         eigen_vals[i]=eigen_tensor.get_eigenvalue(i);
      
   }*/

   //vnl_symetric_eigensystem_compute_eigenvals <int> (a,aa,aaa,b,bb,bbb,d,dd,ddd,&l1,&l2,&l3);

   int l1=eigen_vals[0];
   int l2=eigen_vals[1];
   int l3=eigen_vals[2];

   if( l1>0.01 && l2>0.01 &&l3>0.01){flag=0;}
   else{flag=1;}
   
   sorted_eigs[0][x][y][z]=l1;
   sorted_eigs[1][x][y][z]=l2;
   sorted_eigs[2][x][y][z]=l3;
           
   if(flag==1)// 1
   {
     wasdetectedbad=wasdetectedbad++;
       
     for (int u=0;u<nof-numberb0;u++)//2  
    {
      if(table[u][x][y][z]<lower_thresh[u] || table[u][x][y][z]>higher_thresh[u])   
      {// 3
        if(x==0)
        {
          init_i=x;
          limit_i=x+2;
        }
        else if(x==size[0])
        {
          init_i=x-1;
          limit_i=x+1;
        }
        else
        {
          init_i=x-1;
          limit_i=x+2;
        }

        for (int i=init_i;i<limit_i;i+2)//4
        {
        if(y==0)
        {
          init_j=y;
          limit_j=y+2;
        }
        else if(y==size[1])
        {
          init_j=y-1;
          limit_j=y+1;
        }
        else
        {
          init_j=y-1;
          limit_j=y+2;
        }
        for (int j=init_j;j<limit_j;j+2)//5
        {
        if(z==0)
        {
          init_c=z;
          limit_c=z+2;
        }
        else if(z==size[2])
        {
          init_c=z-1;
          limit_c=z+1;
        }
        else
        {
          init_c=z-1;
          limit_c=z+2;
        }
        for (int c=init_c;c<limit_c;c+2)
        {// 6
          if(table[u][i][j][c]<=0)
          {
           temp_sum=temp_sum+ 0;
          }
          else
          {
           temp_sum=temp_sum + table[u][i][j][c];
           temp_number++;
          }
        }//5

        if (temp_number==0){table[u][x][y][z]=0;}
        else{ table[u][x][y][z]=temp_sum/temp_number;}     

      } //end if//4
           
    } // end of for//3
   
  } // end of if//2
 }//1
}// end of void//0

} // end of namespace


}



