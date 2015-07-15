/*
 * @file                : matrix multiplication n x n
 * @brief               : This is the source code for implementing the
                          matrix multiplication   
 * @author              : Sanjay kumar U
 * @Copyright           : (c) 2014 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.
 */

#include<stdio.h>

main()
{
	int **mat1,**mat2,**res,m,i,j,k,x;
	printf (" enter the order of matrix \n");
	scanf ("%d",&m);
	mat1 = (int **)malloc(m*sizeof(int *));    /* dynamic memory allocation 2D */
	mat2 = (int **)malloc(m*sizeof(int *));
	res  = (int **)malloc(m*sizeof(int *));
	for(i=0;i<m;i++) {
		*(mat1+i)=(int *) malloc(m*sizeof(int));
		*(mat2+i)=(int *) malloc(m*sizeof(int));
		*(res+i) =(int *) malloc(m*sizeof(int));
	}
 
	printf ("give the values of mat1 \n");
	for(i=0;i<m;i++)
		for(j=0;j<m;j++)
			scanf(" %d",(*(mat1+i)+j));

	printf("give the values of mat2 \n");
	for(i=0;i<m;i++)
	        for(j=0;j<m;j++)
        	        scanf(" %d",&mat2[i][j]);
	printf("the values of mat1 \n");
	for(i=0;i<m;i++) {
       		 for(j=0;j<m;j++)
               		 printf(" %3d",mat1[i][j]);
        		        printf(" \n");
	}
	 printf("the values of mat2 \n");
	for(i=0;i<m;i++){
       		 for(j=0;j<m;j++)
	                printf(" %3d",mat2[i][j]);
       			         printf(" \n");
}


	for(i=0;i<m;i++){			/* code which performs multiplication */
		for(j=0;j<m;j++){
			x=0;
			for(k=0;k<m;k++){
				x+=(mat1[j][k]*mat2[k][i]);
			}

			*(*(res+j)+i)=x;			/* storing  result into res pointer */
		}
	}
	printf("the values of res \n");
		for(i=0;i<m;i++){
		        for(j=0;j<m;j++)
            		    printf(" %3d",res[i][j]);
                printf(" \n");
}

}                
