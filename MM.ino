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

coord curCoord;
curCoord.x=0;
curCoord.y=0;

struct instruction
{
  float desiredPos;
  float desiredHeading;
};

struct entry
{
  byte distance;
  byte walls;
};


byte globalHeading = 4;
byte DesiredHeading = 4;
byte wallDistance = 100;//Change after checking - Basically a threshold past which it means that there isn't a wall immediately in that direction
#define X 16
#define Y 16

entry maze[Y][X];
//N,S,E,W
byte headings[] = {1,2,4,8};



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
 instantiate();


}

//Take readings from the IRs and store in the global IR array - (0,front),(1,right),(2,left)

byte readIR()
{

byte r= updateWalls();
return r;
}

//If the value of the bit is 1 it means that there is a wall
byte updateWalls()
{
  readIR();
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

//Get the most optimistic distance between two coordinates in a grid
int calcDist(byte posx, byte posy, byte desireX, byte desireY){
  byte dist = (byte) (abs(desireY-posy)+abs(desireX-posx));
  return dist;
}

//Get the most optimistic distance between a given coordinate and a
//2x2 square in the center of a maze of dimension dim (dim must be even)
byte calcCenter(byte posx, byte posy, byte dim){
  byte center = dim/2;
  byte dist = 0;

  if(posy<center){
    if(posx<center){
      //You're in the bottom left of the maze
      dist=calcDist(posx, posy, (center-1), (center-1));
    }else{
      //You're in the bottom right of the maze
      dist=calcDist(posx,posy,center,(center-1));
    }
  }else{
    if(posx>=center){
      //You're in the top right of the maze
      dist=calcDist(posx,posy,center,center);
    }else{
      //You're in the top left of the maze
      dist=calcDist(posx,posy, (center-1),center);
    }
  }
return dist;
}


void instantiate(){
  for(int j = 0; j<Y; j++){
    for(int i = 0; i<X; i++){
      maze[j][i].distance = calcCenter(i, j, X);
      maze[j][i].walls = 0;//0000
      //x->i, y->j
      //If this is the left column (0,y)
      if(i==0){
        maze[j][i].walls = 8;//1000
      }
      //if this is the bottom row
      if(j==0){
        maze[j][i].walls = 2;//0010
      }
      //if this is the top row
      if(j==(Y-1)){
        maze[j][i].walls = 1;//0001
      }
      //If this is the right column
      if(i==(X-1)){
        maze[j][i].walls = 4;//0100
      }
    }
  }
      maze[0][0].walls = 10;//1010
      maze[Y-1][0].walls = 9;//1001
      maze[0][X-1].walls = 6;//0110
      maze[X-1][Y-1].walls = 5;//0101


}

//Take a coordinate and test if it is within the allowable bounds
boolean checkBounds(coord Coord){
  if((Coord.x >= X) || (Coord.y >= Y) || (Coord.x < 0) || (Coord.y < 0)){return false;}else{return true;}
}

//Given a coordinate, test and return if the coordinate is bounded on three sides
boolean isDead(coord coord){
  boolean deadEnd = false;
  if(checkBounds(coord)){
    byte bounds = maze[coord.y][coord.x].walls;
    //bounds is the integer from the exploratory maze that represents the known walls of the coordinate
    if((bounds == 7)||(bounds == 11)||(bounds == 13) || (bounds == 14)){deadEnd=true;}
  }
  return deadEnd;
}

boolean isEnd(coord Coord, coord DesiredArray[]){
  boolean End = false;
  for(int i=0; i<sizeof(DesiredArray);i++){
    coord Desired = DesiredArray[i];
    if(checkBounds(Coord)){
      if((Coord.x == Desired.x)&&(Coord.y==Desired.y)){
        End = true;
      }
    }
  }
  return End;
}
/*
//It will return an array which points to the distance values of the neighbouring cells in an ascending order
void Neigh(coord c,int a[])
{
byte NeighDist[4];//0-4 -> NSEW
NeighDist[0]=maze[c.y+1][c.x].distance;//n
NeighDist[1]=maze[c.y+1][c.x].distance;//s
NeighDist[2]=maze[c.y+1][c.x].distance;//e
NeighDist[3]=maze[c.y+1][c.x].distance;//w
*/
}

/*
INPUT: a coordinate representing a current position, and a heading
OUTPUT: the coordinates of the next desired position based on the heading and current position
*/
coord bearingCoord(coord currCoord, byte heading){
  coord nextCoord = {0,0};
  switch (heading){
    case 1:
      //code
      nextCoord.x=currCoord.x;
      nextCoord.y=currCoord.y-1;
      break;
    case 2:
      nextCoord.x=currCoord.x;
      nextCoord.y=currCoord.y+1;
      break;
    case 4:
      nextCoord.x=currCoord.x+1;
      nextCoord.y=currCoord.y;
      break;
    case 8:
      nextCoord.x=currCoord.x-1;
      nextCoord.y=currCoord.y;
      break;
  }
  return nextCoord;
}

/*
INPUT: Coord
OUTPUT: An integer that is the least neighbor
*/
int checkNeighs(coord Coord){
  int minVal =  20;
  for(int i=0; i<sizeof(headings); i++){
    byte dir = headings[i];
    //if this dir is accessible
    if((maze[Coord.y][Coord.x].walls & dir) != 0){
      //Get the coordinate of the accessible neighbor
      coord neighCoord = bearingCoord(Coord, dir);
      //Check the value of the accessible neighbor
      if (checkBounds(neighCoord)){
        //if the neighbour is less than the current recording minimum value, update the minimum value
        //If minVal is null, set it right away, otherwise test
        if(maze[neighCoord.y][neighCoord.x].distance < minVal){minVal = maze[neighCoord.y][neighCoord.x].distance;}
      }
    }
  }
  return minVal;
}



void floodfill(coord currCoord, coord desired[])
{
StackList<coord> entries;

//Update wall value of current cell
  maze[currCoord.y][currCoord.x].walls=readIR();

//Updating Walls of neighbouring cells sharing walls
for(int i=0; i<4; i++){
    byte dir = headings[i];
    //If there's a wall in this dir
    if((maze[currCoord.y][currCoord.x].walls & dir) != 0){
      //create a temporary working coordinate
      coord workingCoord = {currCoord.x,currCoord.y};
      switch(dir){
        case 1:
          workingCoord.y=workingCoord.y+1;
          coordUpdate(workingCoord,2);
          break;
        case 2:
          workingCoord.y=workingCoord.y-1;
          coordUpdate(workingCoord,1);
          break;
        case 4:
          workingCoord.x=workingCoord.x+1;
          coordUpdate(workingCoord, 8);
          break;
       case 8:
         workingCoord.x=workingCoord.x-1;
         coordUpdate(workingCoord,4);
         break;
      }
      //If the workingEntry is a valid entry and not a dead end, push it onto the stack
      //isEnd - Make sure the workingCoord is not the center cells during initial run and the (0,0) cell in return run

      if(checkBounds(workingCoord)&&(!isEnd(workingCoord, desired))){
        entries.push(workingCoord);
      }
    }
  }
  //For that current coord will be at the top of the stack we're adding it finally
  entries.push(currCoord);
  //While the entries stack isn't empty
  //When stack becomes empty, all the updates of distance value are completed
  while(!entries.isEmpty()){
    //Pop an entry from the stack
    coord workingEntry = entries.pop();
    //Get the Least open neighbour distance value for the Working Entry
    int neighCheck = checkNeighs(workingEntry);
    //If the least neighbor of the working entry is not one less than the value of the working entry
    if(maze[workingEntry.y][workingEntry.x].distance!=(neighCheck+1)){
      //Update the value of the current cell
      maze[workingEntry.y][workingEntry.x].distance=neighCheck+1;
      //Check it's open neighbours
      for(int i=0;i<sizeof(headings);i++){
        byte dir = headings[i];
        if((maze[workingEntry.y][workingEntry.x].walls & dir) != 0){
          coord nextCoord = bearingCoord(workingEntry,dir);
          if(checkBounds(nextCoord)){
          //isEnd - Make sure the nextCoord is not the center cells during initial run and the (0,0) cell in return run
            if(!isEnd(nextCoord, desired)){
              entries.push(nextCoord);
            }
          }
        }
      }
    }
  }
}

//Identify which cell to move to next



void loop() {
//during run to the centre
coord desired[] = {{X/2-1,Y/2-1},{X/2-1,Y/2},{X/2,Y/2-1},{X/2,Y/2}};
floodFill(desired, globalCoord, true);
floodFillUpdate();

MovetoNextCell();//Turn and forward into the lowest distance value cell

}