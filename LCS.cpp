//
//  tutorial.cpp
//  Created by Sarah Parsons on 11/23/19.
//
//

#include <tsgl.h>
#include <Line.h>
#include <Canvas.h>
#include <Color.h>
#include <iostream>
#include <vector>
#include <string>
#include <omp.h>
#include <stdlib.h>
using namespace tsgl;
using namespace std;

/**
* Generate random string of capital and lowercase letters given desired length of string
*
* @param s pointer to character array
* @param len integer of desired length of string
* 
 * @return a randomly generated string of length len
*/
string gen_random(char *s, const int len) {
    static const char alphanum[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
    
    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    s[len] = 0;
    string a = s;
    return a;
    
}

int main(int argc, char* argv[]) {
    // "Yes"/"No" path parameter for outputting trace at onset of program; default is "No"
    string path = (argc > 2) ? argv[2] : "No";

    //construct canvas with a width of 1200 and height of 700, white background color
    Canvas c(0, 0, 1200, 700, "LCS Problem");
    c.setBackgroundColor(WHITE);
    
    
    
    //c.setFont("/assets/freefont/FreeMonoBold.otf");
    
    // initialize button for keyboard interaction used to trace to false
    bool leftMouseButtonPressed = false;
    
    //Bind the left mouse button so that when it's pressed
    //the boolean is set to true.
    c.bindToButton(TSGL_A, TSGL_PRESS,
                   [&leftMouseButtonPressed]() {
                       leftMouseButtonPressed = true;
                   }
                   );
    
    //Bind the left mouse button again so that when it's released
    //the boolean is set to false.
    c.bindToButton(TSGL_A, TSGL_RELEASE,
                   [&leftMouseButtonPressed]() {
                       leftMouseButtonPressed = false;
                   }
                   );

    // define parameters for 2D grid size (500 x 500)
    c.start();
    int x1 = 300;
    int x2 = 800;
    int y1 = 650;
    int y2 = 150;
    int size = x2 - x1;
    
    // draw title and 2D grid (lime) to canvas
    c.drawText("LCS Problem", 450, 50, 30, RED);
    c.drawRectangle(x1,y1,x2,y2,LIME,true);
    
    // set number of threads as a parameter from command line; default is 2
    int np = (argc > 1) ? atoi(argv[1]) : 2;
    omp_set_num_threads(np);
    
    // draw # threads to canvas
    c.drawText("# Threads: " + to_string(np),75, 140, 22);

    // used to generate random strings, if preferred
    const int str_len = 5;
    char s[str_len];
    char p[str_len];
    string a, b;
    
    // customize strings, if desired, via command line (otherwise, default is 'Happy' and 'Holidays!')
    a = (argc > 3) ? argv[3] : "Happy"; //gen_random(s,str_len);
    b = (argc > 4) ? argv[4] : "Holidays!"; //gen_random(p, str_len);
    
    std::cout<<"a: " << a << endl;
    std::cout<<"b: " << b << endl;
    
    
    const int length_a = a.size();
    const int length_b = b.size();
    std::cout << "length_a: " << length_a << "\n";
    std::cout << "length_b: " << length_b << "\n";
    
    // determine max string, min string
    string max_str;
    string min_str;
    if(length_a > length_b){
        max_str = a;
        min_str = b;}
    else {
        max_str = b;
        min_str = a;}

    
    double tic, seconds;
    tic = omp_get_wtime();
    
    // draw strings to canvas
    c.drawText("String a: " + a,75, 50, 22);
    c.drawText("String b: " + b, 75, 80, 22);

    // compute the number of diagonals
    int num_diag = length_a+length_b-1;
    std::cout << "# Diagonals: " << num_diag << endl;
    c.drawText("# Diagonals: " + to_string(num_diag), 75,110,22);


    int length;
    const int max_len = std::max(length_a,length_b);
    const int min_len = std::min(length_a, length_b);
    
    
    int rows = min_len + 1;
    int cols = max_len + 1;
    
    // define column width and row width based on max and min string lengths
    int col_wid = size/(max_len);
    int row_wid = size/(min_len);
    //cout << col_wid << row_wid << endl;
    
    // draw x- and y-axes and lines within grid based on column and row widths
    for (int i=0; i<min_len; i++){
        c.drawLine(x1,y2+((i)*(row_wid)),x2,y2+((i)*(row_wid)));
    }
    
    for (int j=0; j<max_len; j++){
        c.drawLine(x1+(j*col_wid),y1,x1+(j*col_wid),y2);
    }
    
    //Top String
    for(int i=0; i<max_len; i++) {
        c.drawText(to_string(max_str[i]),x1+((i-0.5+1)*col_wid),y2-(row_wid/8),23, RED);
    }
    
    //Left String
    for(int r=0;r<min_len;r++){
        c.drawText(to_string(min_str[r]),x1-(col_wid/3),y2+((r-0.5+1)*row_wid),23, RED);
    }

    
    
    //int **Diag2 = new int[diag_rows][diag_cols];
    //int Diag2[diag_rows][diag_cols];
    
    // initialize Diag2 vector to 0s
    vector<vector<int>> Diag2(rows,vector<int>(cols));
    for(int l=0;l<min_len+1;l++){
        for(int u=0;u<max_len+1;u++){
            Diag2[l][u]=0;
        }
    }
    
    // Check if canvas is open, Iterate through each diagonal and parallelize the computations of each cell in the diagonal, compute LCS of every cell, store in Diag2, draw value to canvas and draw trace lines/circles if path parameter = 'Yes'
    if(c.isOpen()){
    for(int d=1; d < num_diag+1; d++){
        //changed to compare to max length of strings, not length_a
        if(d <= max_len){
            length = std::min(d,min_len);
        }
        else{length = length_a+length_b-d;}
        // parallelize the cells for each diagonal
        #pragma omp parallel for
        for(int p = 1; p < length+1; p++){
            // diagonal is in the upper triangle
            if(d <= min_len) {
                int x = d-p+1;
                int y = p;
                if(x<=0 || y<=0){Diag2[x][y]=0;
                   c.sleepFor(1);
                    c.drawText(to_string(Diag2[x][y]),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),20);}
                else if(a[x-1]==b[y-1]) {
                    Diag2[x][y] = 1 + Diag2[x - 1][y - 1];
                    if(path == "Yes"){
                        c.sleepFor(1);
                        c.drawCircle(x1+((y-1)*(col_wid)),y2+((x-1)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                        c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                        c.sleepFor(1);
                        c.drawLine(x1+((y-1)*(col_wid)),y2+((x-1)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                    }

                    c.sleepFor(1);
                    c.drawText(to_string(Diag2[x][y]),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),20);
                                    }
                else {
                    Diag2[x][y] = std::max(Diag2[x-1][y], Diag2[x][y - 1]);
                    c.sleepFor(1);
                    c.drawText(to_string(Diag2[x][y]),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),20);
                    if(path == "Yes"){
                        if(std::max(Diag2[x-1][y], Diag2[x][y - 1]) == Diag2[x][y-1]){
                            c.drawCircle(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                            c.drawLine(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                            
                        }
                        else{
                            c.drawLine((x1+((y-0.5)*(col_wid))),y2+((x-1.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                            c.drawCircle(x1+((y-.5)*(col_wid)),y2+((x- 1.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                        }
                        c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                    }
                }
    
            }
            // diagonal is in the middle of the grid
            else if(min_len < d <= max_len){
                int x = d-(d-min_len + p)+1;
                int y = d - min_len + p;
                //std::cout << a[x-1] << b[y-1] << endl;
                if(x<=0 || y<=0){Diag2[x][y]=0;
                    c.sleepFor(1);
                    c.drawText(to_string(Diag2[x][y]),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),20);}
                else if(a[x-1]==b[y-1]) {
                    Diag2[x][y] = 1 + Diag2[x - 1][y - 1];
                    if(path == "Yes"){
                        c.sleepFor(1);
                        c.drawCircle(x1+((y-1.5)*(col_wid)),y2+((x-1.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                        c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                        c.sleepFor(1);
                        c.drawLine(x1+((y-1.5)*(col_wid)),y2+((x-1.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                    }

                    c.sleepFor(1);
                    c.drawText(to_string(Diag2[x][y]),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),20);
                                    }
                else {
                    Diag2[x][y] = std::max(Diag2[x-1][y], Diag2[x][y - 1]);
                    c.sleepFor(1);
                    c.drawText(to_string(Diag2[x][y]),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),20);
                    if(path == "Yes"){
                        if(std::max(Diag2[x-1][y], Diag2[x][y - 1]) == Diag2[x][y-1]){
                            c.drawCircle(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                            c.drawLine(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                            
                        }
                        else{
                            c.drawLine((x1+((y-0.5)*(col_wid))),y2+((x-1.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                            c.drawCircle(x1+((y-.5)*(col_wid)),y2+((x- 1.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                        }
                        c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                    }
                    
                }
                
            }
            // diagonal is in the lower triangle
            else{
                int x = min_len-p+1;
                int y = d-min_len+p;
                //std::cout << a[x-1] << b[y-1] << endl;
                if(x<=0 || y<=0){Diag2[x][y]=0;
                    c.sleepFor(1);
                    c.drawText(to_string(Diag2[x][y]),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),20);}
                else if(a[x-1]==b[y-1]) {
                    Diag2[x][y] = 1 + Diag2[x - 1][y - 1];
                    if(path == "Yes"){
                        c.sleepFor(1);
                        c.drawCircle(x1+((y-1.5)*(col_wid)),y2+((x-1.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                        c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                        c.sleepFor(1);
                        c.drawLine(x1+((y-1.5)*(col_wid)),y2+((x-1.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                    }
                    c.sleepFor(1);
                    c.drawText(to_string(Diag2[x][y]),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),20);
                    
                }
                else {
                    Diag2[x][y] = std::max(Diag2[x - 1][y], Diag2[x][y - 1]);
                    c.sleepFor(1);
                    c.drawText(to_string(Diag2[x][y]),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),20);
                    if(path == "Yes"){
                        if(std::max(Diag2[x-1][y], Diag2[x][y - 1]) == Diag2[x][y-1]){
                            c.drawCircle(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                            c.drawLine(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                            
                        }
                        else{
                            c.drawLine((x1+((y-0.5)*(col_wid))),y2+((x-1.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                            c.drawCircle(x1+((y-.5)*(col_wid)),y2+((x- 1.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                        }
                        c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                    }
                }
                
            }
        }
    }
    }
    // draw LCS Value to canvas
    c.drawText("LCS Value: " + to_string(Diag2[min_len][max_len]), 75, 170, 24,RED);

    std::cout << endl;
    std::cout << "LCS Value: " << Diag2[min_len][max_len] << endl;
    std::cout << endl;
    
    seconds = omp_get_wtime() - tic;
    string seconds_str = to_string(seconds);
    cout << "time: " << seconds << " seconds\n";
    //c.drawText("Time: " + seconds_str.substr(0,seconds_str.find(".")) + seconds_str.substr(seconds_str.find("."), seconds_str.find(".")+1) + " seconds", 75, 360, 18);
    
    
    // trace LCS Value upon user keying 'A' if trace not already drawn
    while(c.isOpen()){
        c.sleep();
        if(leftMouseButtonPressed==true && path == "No"){
            //Iterate through each diagonal and parallelize the computations of each cell in the diagonal, compute LCS of every cell, store in Diag2, draw value to canvas and draw trace lines/circles
                for(int d=1; d < num_diag+1; d++){
                    if(d <= max_len){
                        length = std::min(d,min_len);
                    }
                    else{length = length_a+length_b-d;}
                    #pragma omp parallel for
                    for(int p = 1; p < length+1; p++){
                        if(d <= min_len) {
                            int x = d-p+1;
                            int y = p;
                            
                            if(a[x-1]==b[y-1]) {
                                Diag2[x][y] = 1 + Diag2[x - 1][y - 1];
                                c.drawCircle(x1+((y-1)*(col_wid)),y2+((x-1)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                                c.sleepFor(1);
                                c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                                c.sleepFor(1);
                                c.drawLine(x1+((y-1)*(col_wid)),y2+((x-1)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                                
                                
                            }
                            
                            else {
                                Diag2[x][y] = std::max(Diag2[x-1][y], Diag2[x][y - 1]);
                                if(std::max(Diag2[x-1][y], Diag2[x][y - 1]) == Diag2[x][y-1]){
                                    c.drawCircle(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawLine(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                                    
                                }
                                else{
                                    c.drawCircle(x1+((y-.5)*(col_wid)),y2+((x- 1.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawLine((x1+((y-0.5)*(col_wid))),y2+((x-1.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                                   
                                }
                                
                                
                            }
                        }
                        
                        
                        else if(min_len < d <= max_len){
                            int x = d-(d-min_len + p)+1;
                            int y = d - min_len + p;
                            
                            if(a[x-1]==b[y-1]) {
                                Diag2[x][y] = 1 + Diag2[x - 1][y - 1];
                                c.drawCircle(x1+((y-1.5)*(col_wid)),y2+((x-1.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                                c.sleepFor(1);
                                c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                                c.sleepFor(1);
                                c.drawLine(x1+((y-1.5)*(col_wid)),y2+((x-1.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                            }
                            
                            
                            else {
                                Diag2[x][y] = std::max(Diag2[x-1][y], Diag2[x][y - 1]);
                                if(std::max(Diag2[x-1][y], Diag2[x][y - 1]) == Diag2[x][y-1]){
                                    c.drawCircle(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawLine(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                                  
                                    
                                }
                                else{
                                    c.drawCircle(x1+((y-.5)*(col_wid)),y2+((x- 1.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawLine((x1+((y-0.5)*(col_wid))),y2+((x-1.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                             
                                }
                             
                                
                            }
                            
                        }
                        
                        
                        else{
                            int x = min_len-p+1;
                            int y = d-min_len+p;
                            
                            if(a[x-1]==b[y-1]) {
                                Diag2[x][y] = 1 + Diag2[x - 1][y - 1];
                                c.drawCircle(x1+((y-1.5)*(col_wid)),y2+((x-1.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                                c.sleepFor(1);
                                c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                                c.sleepFor(1);
                                c.drawLine(x1+((y-1.5)*(col_wid)),y2+((x-1.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                                
                            }
                            else {
                                Diag2[x][y] = std::max(Diag2[x - 1][y], Diag2[x][y - 1]);
                                if(std::max(Diag2[x-1][y], Diag2[x][y - 1]) == Diag2[x][y-1]){
                                    c.drawCircle(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawLine(x1+((y-1.5)*(col_wid)),y2+((x-.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                              
                                    
                                }
                                else{
                                    c.drawCircle(x1+((y-.5)*(col_wid)),y2+((x- 1.5)*(row_wid)),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawCircle((x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),(col_wid/6),30,ColorInt(255,0,0,50));
                                    c.sleepFor(1);
                                    c.drawLine((x1+((y-0.5)*(col_wid))),y2+((x-1.5)*(row_wid)),(x1+((y-0.5)*(col_wid))),(y2+((x-0.5)*(row_wid))),RED);
                                    
                                }
                           
                                
                            }
                            
                            
                        }
                    }
                }
            }
        }


    c.wait();
}


