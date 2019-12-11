//
//  Knapsack.cpp
//  
//
//  Created by Sarah Parsons on 11/23/19.
//  Last updated on 12/11/19.
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



int main(int argc, char* argv[]) {
    // "Yes"/"No" path parameter for outputting trace at onset of program; default is "No"
    string path = (argc > 2) ? argv[2] : "No";

    //construct canvas with a width of 1200 and height of 700, white background color
    Canvas c(0, 0, 1200, 700, "0-1 Knapsack Problem");
    c.setBackgroundColor(WHITE);
    
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
    int y1 = 625;
    int y2 = 125;
    int size = x2 - x1;
    
    // draw title and 2D grid (yellow) to canvas
    c.drawText("0-1 Knapsack Problem", 375, 50, 30);
    c.drawRectangle(x1,y1,x2,y2,YELLOW,true);
    
    // set number of threads as a parameter from command line; default is 2
    int np = (argc > 1) ? atoi(argv[1]) : 2; //atoi(argv[1]);
    omp_set_num_threads(np);

    // draw # threads to canvas
    c.drawText("# Threads: " + to_string(np),75, 180, 22);

    // define max value and max weight
    const int MAX_VALUE  = 5;
    const int MAX_WEIGHT = 5;

    // initialize Capacity parameter and draw to canvas
    const int Capacity = 8; //(argc > 3) ? atoi(argv[3]) : 8; //atoi(argv[2]);
    cout << "\nCapacity: " << Capacity << "\n";
    c.drawText("Capacity: " + to_string(Capacity),75, 50, 22);
    double tic, seconds;
    
    // initialize number of items and determine column and row widths based on items and Capacity
    int items = 6; //(argc > 4) ? atoi(argv[4]) : 6; //atoi(argv[3]);
    int col_wid = size/(items);
    int row_wid = size/(Capacity);
    cout << col_wid << " " << row_wid << endl;
    
    // draw x- and y-axes and lines within grid based on column and row widths
    c.drawText("Capacity",x2+(col_wid/4),y2-(row_wid/3),20);
    for(int u=1;u<Capacity+1;u++){
        c.drawText(to_string(u),x2+(col_wid/4),y2+((u-0.5)*row_wid),20);
    }
    
    for (int i=0; i<Capacity; i++){
        c.drawLine(x1,y2+((i)*(row_wid)),x2,y2+((i)*(row_wid)));
    }
    
    
    c.drawText("Num_Items",x1-1.5*(col_wid),y1+(row_wid/2),20);
    for(int y=1;y<items+1;y++){
        c.drawText(to_string(y),(x1+((y-0.5)*(col_wid))),y1+(row_wid/2),20);
    }
    for (int j=0; j<items; j++){
        c.drawLine(x1+(j*col_wid),y1,x1+(j*col_wid),y2);
    }
    
    
    //Weights Vector List
    // randomly assign weights based on max weight and draw weights to canvas
    srand(1);
    vector <int> weights(items,0);
    cout << "# of Weights: " << items << endl;
    for (int w =0; w < items; w++) {
        weights[w] = 1 + (rand() % min(MAX_WEIGHT, Capacity));
        c.drawText("Weights: ", 75, 80, 22);
        c.drawText("|" + to_string(weights[w]) + "| ",75+w*25,100,20);
        cout << weights.at(w);}
    cout << endl;
    
    //Values Vector List
    // randomly assign values based on max value and draw values to canvas
    vector <int> values(items,0);
    cout << "# of Values: " << items << endl;
    for (int v =0; v < values.size(); v++) {
        values[v] = 1 + (rand() % MAX_VALUE);
        c.drawText("Values: ", 75, 130, 22);
        c.drawText("|" + to_string(values[v])+"| ",75+v*25,150,20);
        cout << values.at(v);}
    cout << endl;
    
    int dims = Capacity+1;
    
    //Draw top row
    int knap[Capacity+1][Capacity+1];
    //vector<vector<int>> knap(Capacity+1,vector<int>(Capacity+1));
    int k=0;
    int h=0;
    
    for(int h=0; h<weights.size()+1; h++) {
        //knap[k][h]=0;
        if(path=="Yes"){c.drawText("0",x1+((h-0.5)*col_wid),y2-(row_wid/2),20);}
    }
    
    //Initialize table and draw 0 column
    for(int k=0;k<Capacity+1;k++){
        if(path=="Yes"){c.drawText("0",x1-(col_wid/2),y2+((k-0.5)*row_wid),20);}
        // for(int h=0; h<1;h++){
        for(int h=0; h<weights.size()+1;h++){
            knap[k][h]=0;
        }}
    
    
    tic = omp_get_wtime();
    
    // Check that canvas is open, Iterate through each column and evaluate the knapsack value, store value in knap array, draw values to canvas and draw lines and circles for tracing if path parameter set to "Yes"
    if(c.isOpen()){
        for(h=1;h<items+1;h++){
            // parallelize the rows for each column
            #pragma omp parallel for
            for(k=1;k<Capacity+1;k++) {
                if (knap[k][h-1] < (values[h-1] + knap[k-weights[h-1]][h-1]) && k-weights[h-1] > -1) {
                    knap[k][h] = values[h-1] + knap[k - weights[h-1]][h - 1];
                    if(path == "Yes"){
                        int r = rand()%MAX_COLOR;
                        int g = rand()%MAX_COLOR;
                        int b = rand()%MAX_COLOR;
                        c.sleepFor(1);
                        c.drawCircle(x1+((h-1.5)*(col_wid)),y2+(((k-weights[h-1])-0.5)*(row_wid)),(col_wid/6),30,ColorInt(r,g,b,50));
                        c.drawCircle((x1+((h-0.5)*(col_wid))),(y2+((k-0.5)*(row_wid))),(col_wid/6),30,ColorInt(r,g,b,50));
                        c.drawLine(x1+((h-1.5)*(col_wid)),y2+(((k-weights[h-1])-0.5)*(row_wid)),(x1+((h-0.5)*(col_wid))),(y2+((k-0.5)*(row_wid))));
                    }
                c.sleepFor(1);
                c.drawText(to_string(knap[k][h]),(x1+((h-0.5)*(col_wid))),(y2+((k-0.5)*(row_wid))),20);
                    }
                else { knap[k][h] = knap[k][h-1];
                    if(path == "Yes"){
                        int r = rand()%MAX_COLOR;
                        int g = rand()%MAX_COLOR;
                        int b = rand()%MAX_COLOR;
                        c.sleepFor(1);
                        c.drawCircle(x1+((h-1.5)*(col_wid)),y2+((k-0.5)*(row_wid)),(col_wid/6),30,ColorInt(r,g,b,50));
                        c.drawCircle((x1+((h-0.5)*(col_wid))),(y2+((k-0.5)*(row_wid))),(col_wid/6),30,ColorInt(r,g,b,50));
                        c.drawLine(x1+((h-1.5)*(col_wid)),y2+((k-0.5)*(row_wid)),(x1+((h-0.5)*(col_wid))),(y2+((k-0.5)*(row_wid))));

                    }
                c.sleepFor(1);
                c.drawText(to_string(knap[k][h]),(x1+((h-0.5)*(col_wid))),(y2+((k-0.5)*(row_wid))),20);

                    }
                }

        }
    }

    //Output Knap array
    cout << "Knap: " << endl;
    for(int k=0;k<Capacity+1;k++){
        for(int h=0; h <weights.size()+1; h++){
            cout << knap[k][h] << ", ";
        }
        cout << endl;
    }
   
    cout << "Knapsack Value: " << knap[Capacity][items] << endl;
    
    cout << endl;
    
    // draw Knapsack Value to canvas
    c.drawText("Knapsack ",85, 210, 24);
    c.drawText("Value: " + to_string(knap[Capacity][items]),85, 230, 24);
    
    seconds = omp_get_wtime() - tic;
    
    cout << "time: " << seconds << "\n";
    
    // trace Knapsack Value upon user keying 'A' if trace not already drawn
    while(c.isOpen()){
        c.sleep();
        if(leftMouseButtonPressed==true && path == "No"){
            //Draw Top Row
            //vector<vector<int>> knap(dims,vector<int>(dims));
            int knap[Capacity+1][Capacity+1];
            //vector<vector<int>> knap(Capacity+1,vector<int>(Capacity+1));
            int k=0;
            int h=0;
            for(int h=0; h<weights.size()+1; h++) {
                c.drawText("0",x1+((h-0.5)*col_wid),y2-(row_wid/2),20);
            }
            //Initialize Table
            for(int k=0;k<Capacity+1;k++){
                c.drawText("0",x1-(col_wid/2),y2+((k-0.5)*row_wid),20);
                for(int h=0; h<weights.size()+1;h++){
                    knap[k][h]=0;}
                }
            //Iterate through each column and evaluate the knapsack value, draw lines and circles for tracing
            for(h=1;h<items+1;h++){
                #pragma omp parallel for
                for(k=1;k<Capacity+1;k++) {
                    
                    if (knap[k][h-1] < (values[h-1] + knap[k-weights[h-1]][h-1]) && k-weights[h-1] > -1) {
                        knap[k][h] = values[h-1] + knap[k - weights[h-1]][h - 1];
                        int r = rand()%MAX_COLOR;
                        int g = rand()%MAX_COLOR;
                        int b = rand()%MAX_COLOR;
                        c.sleepFor(1);
                        c.drawCircle(x1+((h-1.5)*(col_wid)),y2+(((k-weights[h-1])-0.5)*(row_wid)),(col_wid/6),30,ColorInt(r,g,b,50));
                        c.drawCircle((x1+((h-0.5)*(col_wid))),(y2+((k-0.5)*(row_wid))),(col_wid/6),30,ColorInt(r,g,b,50));
                        c.drawLine(x1+((h-1.5)*(col_wid)),y2+(((k-weights[h-1])-0.5)*(row_wid)),(x1+((h-0.5)*(col_wid))),(y2+((k-0.5)*(row_wid))));
                        //if(k==8){
                         //   cout << x1+((h-1.5)*(col_wid)) << ", " << y2+(((k-weights[h-1])-0.5)*(row_wid)) << "\n";}
                        
                    }
                    else { knap[k][h] = knap[k][h-1];
                        int r = rand()%MAX_COLOR;
                        int g = rand()%MAX_COLOR;
                        int b = rand()%MAX_COLOR;
                        c.sleepFor(1);
                        c.drawCircle(x1+((h-1.5)*(col_wid)),y2+((k-0.5)*(row_wid)),(col_wid/6),30,ColorInt(r,g,b,50));
                        c.drawCircle((x1+((h-0.5)*(col_wid))),(y2+((k-0.5)*(row_wid))),(col_wid/6),30,ColorInt(r,g,b,50));
                        c.drawLine(x1+((h-1.5)*(col_wid)),y2+((k-0.5)*(row_wid)),(x1+((h-0.5)*(col_wid))),(y2+((k-0.5)*(row_wid))));
                        //if(k==8){
                         //   cout << x1+((h-1.5)*(col_wid))<< ", " << y2+((k-0.5)*(row_wid)) << "\n";
                        //}
                        
                    }
                }
                
            }
        }
    }

    
    c.wait();
}


