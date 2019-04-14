//Initialize gyro,accelero,Motor driver pins,IR pins.
//Initialize the 3 IRs
#define irfo 2
#define irfi A0
#define irro 3
#define irri A1
#define irlo 4
#define irli A2

//Motor Driver
#define enL 5             //PWM 3,5,6,9,10,11
#define enR 6             //PWM
#define L1 7
#define L2 8
#define R1 9
#define R2 10

//NXP


float IR[3];

struct coord
{
  byte x;
  byte y;
};

coord desired[] = {{X-1,Y-1},{X-1,Y},{X,Y-1},{X,Y}};

struct entry
{
  int distance;
  byte walls;
};

struct instruction
{
  float desiredPos;
  float desiredHeading;
};

byte globalHeading = 4;
int wallDistance = ;//Change after checking - Basically a threshold past which it means that there isn't a wall immediately in that direction

void initialMap()
{

}

void setup() {
//IR
pinMode(irfo,OUTPUT);
pinMode(irlo,OUTPUT);
pinMode(irro,OUTPUT);
pinMode(irfi,INPUT);
pinMode(irli,INPUT);
pinMode(irri,INPUT);

//MOTOR DRIVER
pinMode(enL,OUTPUT);
pinMode(enR,OUTPUT);
pinMode(L1,OUTPUT);
pinMode(L2,OUTPUT);
pinMode(R1,OUTPUT);
pinMode(R2,OUTPUT);

//NXP

//Initialize map

}

//Take readings from the IRs and store in the global IR array - (0,front),(1,right),(2,left)

void readIR()
{
}

//If the value of the bit is 1 it means that there is a wall
byte readwalls()
{
  byte wallReading = 15;
  byte north = 0;
  byte south = 0;
  byte east = 0;
  byte west = 0;
  switch(globalHeading){
    case 1:
      //if the forward sensor is tripped
      if(IR[0]>wallDistance){
        //set north to 1
        north = 1;
      }
      //if the right sensor is tripped
      if(IR[1]>wallDistance){
        //set east to 4
        east = 4;
      }
      //if the left sensor is tripped
      if(IR[2]>wallDistance){
        //set west to 8
        west = 8;
      }
      //Subtract the sum of north east and west from the value of wall reading
      wallReading -= (north+east+west);
      break;
    case 2:
      //if the forward sensor is tripped
      if(IR[0]>wallDistance){
        //set south to 2
        south = 2;
      }
      //if the right sensor is tripped
      if(IR[1]>wallDistance){
        //set west to 8
        west = 8;
      }
      //if the left sensor is tripped
      if(IR[2]>wallDistance){
        //set east to 4       
        east = 4;
      }
      //subtract the sum from 15
      wallReading-=(south+east+west);
      break;
    case 4:
      //if the forward sensor is tripped
      if(IR[0]>wallDistance){
        //set east to 4
        east = 4;
      }
      //if the right sensor is tripped
      if(IR[1]>wallDistance){
        //set south to 2
        south = 2;
      }
      //if the left sensor is tripped
      if(IR[2]>wallDistance){
        //set north to 1
        north = 1;
      }
      //subtract the sum from 15
      wallReading-=(north+south+east);
      break;
    case 8:
      //if the forward sensor is tripped
      if(IR[0]>wallDistance){
        //set east to 8
        west = 8;
      }
      //if the right sensor is tripped
      if(IR[1]>wallDistance){
        //set north to 1
        north = 1;
      }
      //if the left sensor is tripped
      if(IR[2]>wallDistance){
        //set south to 1
        south = 2;
      }
      //subtract the sum from 15
      wallReading-=(west+north+south);
      break;
    }
    return wallReading;
}



}
void loop() {
  // put your main code here, to run repeatedly:
readwalls();//Identify the position of the walls and update the cell's wall values, also update the wall sharing neighbour's wall value
if(discontinuity)// if the cell's open neighbours all have distance values higher than the current cell
{updateDistance;//Set distance value of current cell to (Lowest distance of open neighbours+1) 
checkNeighbourViolate;
}
identifyLowestCell();//Identify the open neighbour having the least distance value
MovetoNextCell();//Turn and forward into the lowest distance value cell

}
