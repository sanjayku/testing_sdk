Index: holetest.c
===================================================================
Index: tic-tac-toy/RUN
===================================================================
Cannot display: file marked as a binary type.
svn:mime-type = application/octet-stream
Index: tic-tac-toy/RUN
===================================================================
--- tic-tac-toy/RUN	(revision 0)
+++ tic-tac-toy/RUN	(working copy)

Property changes on: tic-tac-toy/RUN
___________________________________________________________________
Added: svn:executable
## -0,0 +1 ##
+*
\ No newline at end of property
Added: svn:mime-type
## -0,0 +1 ##
+application/octet-stream
\ No newline at end of property
Index: tic-tac-toy/newtic.c
===================================================================
--- tic-tac-toy/newtic.c	(revision 2519)
+++ tic-tac-toy/newtic.c	(working copy)
@@ -1,3 +1,4 @@
+/*          PROGRAM FOR TIC TAC TOE GAME                         */
 #include<stdio.h>
 void player_o();
 void player_x();
@@ -10,32 +11,74 @@
 {
 printf(" %d  %d\n",i,j);
 if(i==0&&j==0){
-i=0;j=1;
+	if(disp_status[1][0]=='o'&&disp_status[2][0]=='_'){
+		i=2;j=0;}
+	else if(disp_status[2][0]=='o'&&disp_status[1][0]=='_'){
+		i=1;j=0;}
+	else if(disp_status[0][1]=='o'&&disp_status[0][2]=='_'){
+		i=0;j=2;}
+	else if(disp_status[0][2]=='o'&&disp_status[0][1]=='_'){
+		i=0;j=1;}
+	else if(disp_status[1][1]=='o'&&disp_status[2][2]=='_'){
+		i=2;j=2;}
+	else if(disp_status[2][2]=='o'&&disp_status[1][1]=='_'){
+		i=1;j=1;}
 }
+else if(i==0&&j==1) {
+	if(disp_status[1][1]=='o'&&disp_status[2][1]=='_'){
+		i=2;j=1;
+	}
+	if(disp_status[0][0]=='o'&&disp_status[0][2]=='_'){
+		i=0;j=2;
+	}
+	if(disp_status[0][2]=='o'&&disp_status[0][0]=='_'){
+		i=0;j=0;
+	}
+	}
 else if(i==1&&j==0) {
-	i=2;
-	j=0;
+	if(disp_status[1][1]=='o'&&disp_status[1][2]=='_'){
+		i=1;j=2;}
+	else
+	if(disp_status[0][0]=='o'&&disp_status[2][0]=='_'){
+		i=2;j=0;}
+	else
+	if(disp_status[2][0]=='o'&&disp_status[0][0]=='_'){
+		i=0;j=0;}
+	else
+	if(disp_status[1][1]=='_') {
+		i=1;j=1;}
 	
 }
-else if(i==0&&j ==1) {
-	if(disp_status[0][2]=='o'||disp_status[0][0]=='_'){
+else if(i==1&&j==2) {
+	if(disp_status[1][1]=='o'&& disp_status[1][0]=='_'){
+		i=1;j=0;}
+	else
+	if(disp_status[0][2]=='_'){
+		i=0;j=2 ;}
+	else
+	if(disp_status[2][2]=='_'){
+		i=2;j=2 ;}
+	}
+	
+else if(i==1&&j==1) {
+	if(disp_status[0][2]=='o'&&disp_status[2][0]=='_'){
+		i=2;j=0;}
+	else if(disp_status[2][2]=='o'&&disp_status[0][0]=='_'){
 		i=0;j=0;}
-	else if(disp_status[0][0]=='o'||disp_status[0][2]=='_'){
-		i=0;j=2;}
-	return;
-	}	
-
-else if(i==1&&j==1) {
-	if(disp_status[1][2]=='o'|| disp_status[1][0]=='_') {
+	else if(disp_status[0][0]=='o'&&disp_status[2][2]=='_'){
+		i=2;j=2;}
+	else
+	if(disp_status[1][2]=='o'&& disp_status[1][0]=='_') {
 		i=1;j=0;}
-	else if(disp_status[1][0]=='o'||disp_status[1][2]=='_'){
+	else if(disp_status[1][0]=='o'&&disp_status[1][2]=='_'){
 		i=1;j=2;}
-	else if(disp_status[0][1]=='o'||disp_status[2][1]=='_'){
+	else if(disp_status[0][1]=='o'&&disp_status[2][1]=='_'){
 		i=2;j=1;}
-	else if(disp_status[2][1]=='o'||disp_status[0][1]=='_'){
+	else if(disp_status[2][1]=='o'&&disp_status[0][1]=='_'){
 		i=0;j=1;}
 		
 }
+
 else if(i==2&&j==0) {
 	if(disp_status[1][1]=='o'||disp_status[0][2]=='_'){
 		i=0;j=2;}
@@ -58,7 +101,7 @@
 		i=2;j=2;}
 	}
 }
-void display()
+void display()		/* displays the table for every input */
 {
 int m , n ;
 	for(m=0;m<3;m++) {
@@ -69,7 +112,7 @@
 	}
 }
 
-void player_o()
+void player_o()		/* function for player 0 */
 {
 printf(" enter the row and column at which o should be kept \n");
 scanf(" %d %d",&i,&j);
@@ -80,7 +123,7 @@
 			player_o();
 	}
 }
-void player_x()
+void player_x()		/* function for computer */
 {
 printf(" enter the row and column at which x should be kept \n");
 computer();
@@ -92,7 +135,7 @@
 		player_x();
 	}
 }
-void check()
+void check()		/*it will check who wins after every input */
 {
 int cnt1,cnt2;
 int m,n;
Index: tic-tac-toy/tictac.c
===================================================================
--- tic-tac-toy/tictac.c	(revision 2516)
+++ tic-tac-toy/tictac.c	(working copy)
@@ -58,8 +58,6 @@
 				win_check=1;
 
 			} else if(disp_status[i][j]=='o') {
-				cnt2++;
-					if(cnt2==3)
 					win_check=2;
 			} 
 		}
Index: tic-tac-toy/tictac2.c
===================================================================
--- tic-tac-toy/tictac2.c	(revision 2516)
+++ tic-tac-toy/tictac2.c	(working copy)
@@ -8,22 +8,36 @@
 void computer()
 {
 printf(" %d  %d\n",i,j);
-if(i==0 && j==0) {
-i=1;	j=1;
-}
-if(i==1 && j==0) {
-	i=2;
-	j=0;
-}
-if(i==0 && j==1) {
-	i=0;
-	j=2;
-}
-if(i==1 && j==1) {
 
-}
+switch(1)
+{
+	case 'if(i==0&&j==0)': i=0;j=1;
+				if(disp_status[i][j]=='o') {
+					i=0;j=2;
+					}else if(disp_status[j][i]=='o'){
+					i=2;j=0;
+						}
+				break;
+	case 'if(i==1&&j==1)': i=0;j=1;
+				if(disp_status[i][j]=='o'||disp_status[i][j]!='_') {
+				 i=2;j=1;}
+				else if(disp_status[1][2]=='o') {
+					i=1;j=0;
+					break;	
+					}
+				else if(disp_status[0][2]=='o'){ 
+					i=2;j=0;
+					break;
+					}
+				else if(disp_status[2][0]=='o'){
+					i=0;j=2;
+					break;
+					}
+				else break;
+ 
 
 }
+}
 void display()
 {
 int i , j ;
@@ -53,8 +67,7 @@
 	if(disp_status[i][j]=='_') {
 		disp_status[i][j]='x';
 	}
-	else {  printf(" invalid entry\n");
-		i=rand()%3;
+	else { 	i=rand()%3;
 		j=rand()%3;		
 		player_x();
 	}
@@ -114,11 +127,10 @@
 		
 }	
 printf(" %d moves\n",moves);
-	
-}
+}	
+
 main()
 {
-
 display();
 
 printf(" player 1 will start the game \n");
@@ -144,7 +156,7 @@
 		break;
 	}
 
-	player_x(i,j);
+//	player_x(i,j);
 	moves++;
 	display();
 	check();
Index: tic-tac-toy/tictactoe.c
===================================================================
--- tic-tac-toy/tictactoe.c	(revision 0)
+++ tic-tac-toy/tictactoe.c	(working copy)
@@ -0,0 +1,236 @@
+/*          PROGRAM FOR TIC TAC TOE GAME                         */
+#include<stdio.h>
+void player_o();
+void player_x();
+void check();
+void computer();
+char disp_status[3][3]={{95,95,95},{95,95,95},{95,95,95}};
+int win_check,moves;
+int moves,i,j;
+void computer()
+{
+printf(" %d  %d\n",i,j);
+if(i==0&&j==0){
+	if(disp_status[1][0]=='o'&&disp_status[2][0]=='_'){
+		i=2;j=0;}
+	else if(disp_status[2][0]=='o'&&disp_status[1][0]=='_'){
+		i=1;j=0;}
+	else if(disp_status[0][1]=='o'&&disp_status[0][2]=='_'){
+		i=0;j=2;}
+	else if(disp_status[0][2]=='o'&&disp_status[0][1]=='_'){
+		i=0;j=1;}
+	else if(disp_status[1][1]=='o'&&disp_status[2][2]=='_'){
+		i=2;j=2;}
+	else if(disp_status[2][2]=='o'&&disp_status[1][1]=='_'){
+		i=1;j=1;}
+}
+else if(i==0&&j==1) {
+	if(disp_status[1][1]=='o'&&disp_status[2][1]=='_'){
+		i=2;j=1;
+	}
+	if(disp_status[0][0]=='o'&&disp_status[0][2]=='_'){
+		i=0;j=2;
+	}
+	if(disp_status[0][2]=='o'&&disp_status[0][0]=='_'){
+		i=0;j=0;
+	}
+	}
+else if(i==1&&j==0) {
+	if(disp_status[1][1]=='o'&&disp_status[1][2]=='_'){
+		i=1;j=2;}
+	else
+	if(disp_status[0][0]=='o'&&disp_status[2][0]=='_'){
+		i=2;j=0;}
+	else
+	if(disp_status[2][0]=='o'&&disp_status[0][0]=='_'){
+		i=0;j=0;}
+	else
+	if(disp_status[1][1]=='_') {
+		i=1;j=1;}
+	
+}
+else if(i==1&&j==2) {
+	if(disp_status[1][1]=='o'&& disp_status[1][0]=='_'){
+		i=1;j=0;}
+	else
+	if(disp_status[0][2]=='_'){
+		i=0;j=2 ;}
+	else
+	if(disp_status[2][2]=='_'){
+		i=2;j=2 ;}
+	}
+	
+else if(i==1&&j==1) {
+	if(disp_status[0][2]=='o'&&disp_status[2][0]=='_'){
+		i=2;j=0;}
+	else if(disp_status[2][2]=='o'&&disp_status[0][0]=='_'){
+		i=0;j=0;}
+	else if(disp_status[0][0]=='o'&&disp_status[2][2]=='_'){
+		i=2;j=2;}
+	else
+	if(disp_status[1][2]=='o'&& disp_status[1][0]=='_') {
+		i=1;j=0;}
+	else if(disp_status[1][0]=='o'&&disp_status[1][2]=='_'){
+		i=1;j=2;}
+	else if(disp_status[0][1]=='o'&&disp_status[2][1]=='_'){
+		i=2;j=1;}
+	else if(disp_status[2][1]=='o'&&disp_status[0][1]=='_'){
+		i=0;j=1;}
+		
+}
+
+else if(i==2&&j==0) {
+	if(disp_status[1][1]=='o'||disp_status[0][2]=='_'){
+		i=0;j=2;}
+	else if(disp_status[2][1]=='o'||disp_status[2][2]=='_'){
+		i=2;j=2;}
+	}
+else if(i==0&&j==2) {
+	if(disp_status[0][1]=='o'||disp_status[0][0]=='_'){
+		i=0;j=0;}
+	if(disp_status[1][1]=='o'||disp_status[2][0]=='_'){
+		i=2;j=0;}
+
+}
+else if(i==2&&j==1) {
+	if(disp_status[1][1]=='o'||disp_status[0][1]=='_'){
+		i=0;j=1;}
+	if(disp_status[2][2]=='o'||disp_status[2][0]=='_'){
+		i=2;j=0;}
+	if(disp_status[2][0]=='o'||disp_status[2][2]=='_'){
+		i=2;j=2;}
+	}
+}
+void display()		/* displays the table for every input */
+{
+int m , n ;
+	for(m=0;m<3;m++) {
+		for(n=0;n<3;n++) {
+			printf( "%c ",disp_status[m][n]);
+		}			
+		printf("\n");
+	}
+}
+
+void player_o()		/* function for player 0 */
+{
+printf(" enter the row and column at which o should be kept \n");
+scanf(" %d %d",&i,&j);
+	if(disp_status[i][j]=='_') {
+		disp_status[i][j]='o';
+	}
+	else {  printf(" invalid entry\n");
+			player_o();
+	}
+}
+void player_x()		/* function for computer */
+{
+printf(" enter the row and column at which x should be kept \n");
+computer();
+	if(disp_status[i][j]=='_') {
+		disp_status[i][j]='x';
+	}
+	else {  i=rand()%3;
+		j=rand()%3;		
+		player_x();
+	}
+}
+void check()		/*it will check who wins after every input */
+{
+int cnt1,cnt2;
+int m,n;
+if(moves<=9) {
+
+	for(m=0;m<3;m++) {
+		cnt1=0,cnt2=0;
+		for(n=0;n<3;n++) {
+			if(disp_status[m][n]=='x') {
+				cnt1++;
+				if(cnt1==3)
+				win_check=1;
+
+			} else if(disp_status[m][n]=='o') {
+				cnt2++;
+					if(cnt2==3)
+					win_check=2;
+			} 
+		}
+	}
+	for(n=0;n<3;n++) {
+		cnt1=0,cnt2=0;
+		for(m=0;m<3;m++) {
+			if(disp_status[m][n]=='x') {
+				cnt1++;
+				if(cnt1==3)
+				win_check=1;
+
+			} else if(disp_status[m][n]=='o') {
+				cnt2++;
+				if(cnt2==3)
+				win_check=2;
+				}
+		}
+	}
+	cnt1=0;cnt2=0;
+	for(m=0;m<3;m++) {
+			if(disp_status[m][m]=='x') {
+				cnt1++;
+				if(cnt1==3)
+				win_check=1;
+			} else if(disp_status[m][m]=='o') {
+				cnt2++;
+				if(cnt2==3)
+				win_check=2;
+			}
+	}
+		 if((disp_status[0][2]=='x' && disp_status[2][0] == 'x') && disp_status[1][1]=='x')
+		win_check=1;
+	else if((disp_status[0][2]=='o' && disp_status[2][0]== 'o') && disp_status[1][1] == 'o')
+		win_check=2;
+
+		
+}	
+printf(" %d moves\n",moves);
+	
+}
+main()
+{
+display();
+
+printf(" player 1 will start the game \n");
+
+while(1) {
+	
+	player_o();
+	display();
+	moves++;
+	check();
+	
+		if(win_check==2) {
+		printf(" player o win the game \n");
+		printf(" game ends here \n");
+		break;
+	}
+		 if((moves==9)&&(win_check!=1 && win_check!=2))
+                win_check=3;
+
+
+		if(win_check==3) {
+		printf(" game drawn \n");
+		printf(" game ends here \n");
+		break;
+	}
+	player_x();
+	moves++;
+	display();
+	check();
+	if(win_check==1) {
+		printf(" player x win the game \n");
+		printf(" game ends here \n");
+		break;
+	}
+	
+	}
+}
+
+
