//Last Update -> 10-6-19
//Author : Divyesh Narayanan

#include <QueueList.h>
#include <StackList.h>

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

struct instruction
{
  float desiredPos;
  float desiredHeading;
};

//Navigation info
QueueList<instruction> instructions;

struct entry
{
  byte distance;
  byte walls;
};

coord currCoord = {0,0};
coord globalCoord = {0,0};
coord globalEnd = {0,0};
byte globalHeading = 4;

byte wallDistance = 100;//Change after checking - Basically a threshold past which it means that there isn't a wall immediately in that direction
#define X 16
#define Y 16

entry maze[Y][X];
//N,S,E,W
byte headings[] = {1,2,4,8};

void moveDist(float pos)
{
  
}
void turn(float d)
{

}

/*
void initialMap()
{

}
*/
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
//The following function takes values from the IR sensors and then sends it to floodfill which updates the global maze wall array
byte readIR()
{
//Code for reading the current wall presence and update to IR array



byte r=updateWalls();
return r;
}

//If the value of the bit is 1 it means that there is a wall
byte updateWalls()
{

  //Analysis of the results of the current wall reading to condense it into a single byte and return it to readIR

  byte wallReading = 0;
  byte north = 0;
  byte south = 0;
  byte east = 0;
  byte west = 0;
  switch(globalHeading){
    case 1:
      //if the forward sensor is tripped
      if(IR[0]<wallDistance){
        //set north to 1
        north = 1;
      }
      //if the right sensor is tripped
      if(IR[1]<wallDistance){
        //set east to 4
        east = 4;
      }
      //if the left sensor is tripped
      if(IR[2]<wallDistance){
        //set west to 8
        west = 8;
      }
      //Set the wallReading to the sum
      wallReading = (north+east+west);
      break;
    case 2:
      //if the forward sensor is tripped
      if(IR[0]<wallDistance){
        //set south to 2
        south = 2;
      }
      //if the right sensor is tripped
      if(IR[1]<wallDistance){
        //set west to 8
        west = 8;
      }
      //if the left sensor is tripped
      if(IR[2]<wallDistance){
        //set east to 4
        east = 4;
      }
      //Set the wallReading to the sum
      wallReading = (south+east+west);
      break;
    case 4:
      //if the forward sensor is tripped
      if(IR[0]<wallDistance){
        //set east to 4
        east = 4;
      }
      //if the right sensor is tripped
      if(IR[1]<wallDistance){
        //set south to 2
        south = 2;
      }
      //if the left sensor is tripped
      if(IR[2]<wallDistance){
        //set north to 1
        north = 1;
      }
      //Set the wallReading to the sum
      wallReading = (north+south+east);
      break;
    case 8:
      //if the forward sensor is tripped
      if(IR[0]<wallDistance){
        //set east to 8
        west = 8;
      }
      //if the right sensor is tripped
      if(IR[1]<wallDistance){
        //set north to 1
        north = 1;
      }
      //if the left sensor is tripped
      if(IR[2]<wallDistance){
        //set south to 1
        south = 2;
      }
      //Set the wallReading to the sum
      wallReading = (west+north+south);
      break;
    }
    return wallReading;
}

//Setting the distance values for the return to (0,0) from the center
void resetToCoord(coord desiredCoord){
  for(int j = 0; j<Y; j++){
    for(int i = 0; i<X; i++){
      maze[j][i].distance = calcDist(i, j, desiredCoord.x, desiredCoord.y);
    }
  }
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

//Initial wall value assignment
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
bool checkBounds(coord Coord){
  if((Coord.x >= X) || (Coord.y >= Y) || (Coord.x < 0) || (Coord.y < 0)){return false;}else{return true;}
}

//Given a coordinate, test and return if the coordinate is bounded on three sides
bool isDead(coord Coord){
  bool deadEnd = false;
  if(checkBounds(Coord)){
    byte bounds = maze[Coord.y][Coord.x].walls;
    //bounds is the integer from the exploratory maze that represents the known walls of the coordinate
    if((bounds == 7)||(bounds == 11)||(bounds == 13) || (bounds == 14)){deadEnd=true;}
  }
  return deadEnd;
}

bool isEnd(coord Coord, coord DesiredArray[]){
  bool End = false;
  for(int i=0; i<((sizeof(DesiredArray))/sizeof(coord));i++){
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
INPUT: a coordinate representing a current position, and a heading
OUTPUT: the coordinates of the next natural position based on the heading and current position
*/
coord bearingCoord(coord currCoord, byte heading){
  coord nextCoord = {0,0};
  switch (heading){
    case 1:
      //code
      nextCoord.x=currCoord.x;
      nextCoord.y=currCoord.y+1;
      break;
    case 2:
      nextCoord.x=currCoord.x;
      nextCoord.y=currCoord.y-1;
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
OUTPUT: Whether or not the given coordinate has a Lower valued Neighbour
*/
int checkNeighs(coord Coord){

  int minVal =  20;
  for(int i=0; i<4; i++){
    byte dir = headings[i];
    //if this dir is accessible
    if((maze[Coord.y][Coord.x].walls & dir) != dir){
      //Get the coordinate of the accessible neighbor
      coord neighCoord = bearingCoord(Coord, dir);
      //Check the value of the accessible neighbor
      if (checkBounds(neighCoord)){
        //if the neighbour is less than the current recording minimum value, update the minimum value
        //If minVal is null, set it right away, otherwise test
        if(maze[neighCoord.y][neighCoord.x].distance < minVal)
        {minVal = maze[neighCoord.y][neighCoord.x].distance;}
      }
    }
  }

  return minVal;
}

/*
INPUT: Coordinate to update, and a direction representing the wall to add
OUTPUT: Update to coordinate adding the wall provided as an argument
*/
void coordUpdate(coord coordinate, byte wallDir){
  if(checkBounds(coordinate)){
    if((maze[coordinate.y][coordinate.x].walls & wallDir) != wallDir){
      maze[coordinate.y][coordinate.x].walls += wallDir;
    }
  }
}

void floodFillUpdate(coord currCoord, coord desired[])
{
StackList<coord> entries;

//Update wall value of current cell
  maze[currCoord.y][currCoord.x].walls=readIR();

//Updating Walls of neighbouring cells sharing walls
for(int i=0; i<4; i++){
    byte dir = headings[i];
    //If there's a wall in this dir
    if((maze[currCoord.y][currCoord.x].walls & dir) == dir){
      //create a temporary working coordinate
      coord workingCoord=bearingCoord(currCoord,dir);
      if(checkBounds(workingCoord))
       {   switch(dir){
            case 1:

              coordUpdate(workingCoord,2);
              break;
            case 2:

              coordUpdate(workingCoord,1);
              break;
            case 4:

              coordUpdate(workingCoord, 8);
              break;
            case 8:

              coordUpdate(workingCoord,4);
              break;
          }
        }
      //If the workingEntry is a valid entry and not a dead end, push it onto the stack
      //isEnd - Make sure the workingCoord is not the center cells during initial run and the (0,0) cell in return run

      /*if(checkBounds(workingCoord)&&(!isEnd(workingCoord, desired))){
        entries.push(workingCoord);
      }*/
    }
  }

  //Check for the presence of a Lower Open Neighbours for the current cell
  if(checkNeighs(currCoord)<maze[currCoord.y][currCoord.x].distance)
  //Since a Lower Valued Neighbour exists, proceed to finding the heading direction in the main Loop
  return;

  else
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
      for(int i=0;i<4;i++){
        byte dir = headings[i];
        if((maze[workingEntry.y][workingEntry.x].walls & dir) != dir){
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

//Returns the direction opposite to the given direction
byte oppHeading(byte dir){
switch(dir){
case(1):
return 2;
case(2):
return 1;
case(4):
return 8;
case(8):
return 4;
}
}

//Identify which cell to move to next
/*
INPUT: A Coord representing the current coordinate and the mouse's current heading
OUTPUT: An optimal direction away from the current coordinate.
*/
byte orient(coord currCoord, byte heading){

  coord leastNext = {0,0};
  //This is the absolute largest value possible (dimension of maze squared)
  int leastNextVal = sizeof(maze)*sizeof(maze);
  byte oppHead = oppHeading(heading);
  byte leastDir = heading;

  //If there is a bitwise equivalence between the current heading and the cell's value, then the next cell is accessible
  //Setting this first gives first preference to proceeding straight
  if((maze[currCoord.x][currCoord.y].walls & heading) != heading){
    //Define a coordinate for the next cell based on this heading and set the leastNextVal to its value
    coord leastnextTemp = bearingCoord(currCoord, heading);

    if(checkBounds(leastnextTemp)){
      leastNext = leastnextTemp;
      leastNextVal = maze[leastNext.y][leastNext.x].distance;
    }
  }

  for(int i=0; i<4; i++){
    byte dir = headings[i];
    //if this dir is accessible
    if((maze[currCoord.y][currCoord.x].walls & dir) != dir){
      //define the coordinate for this dir
      coord dirCoord = bearingCoord(currCoord,dir);

      if(checkBounds(dirCoord)){
        //if this dir is more optimal than continuing straight
        if(maze[dirCoord.y][dirCoord.x].distance < leastNextVal){
          //update the value of leastNextVal
          leastNextVal = maze[dirCoord.y][dirCoord.x].distance;
          //update the value of leastnext to this dir
          leastNext = dirCoord;
          leastDir = dir;
        }
        //So that returning is given least preference in case of it being one of the equal lowest valued direction
        if((maze[dirCoord.y][dirCoord.x].distance == leastNextVal)&&(leastDir==oppHead))
        {
          //update the value of leastNextVal
          leastNextVal = maze[dirCoord.y][dirCoord.x].distance;
          //update the value of leastNext to this dir
          leastNext = dirCoord;
          leastDir = dir;
        }
      }
    }
  }
  return leastDir;
}

//Returns an instruction which contains the information on how it has to turn and how much to move forward according to given info
//Make changes to this according to gyroscope and change 7.74 according to the distance to be moved forward
instruction createInstruction(coord currCoord, coord nextCoord, byte nextHeading){
  float change = 0.0;
  switch(nextHeading){
    case 1:
      if(globalHeading==4){
        change = -90.0;
      }
      if(globalHeading==8){
        change = 90.0;
      }
      if(globalHeading==2){
        change = 180.0;
      }
      break;
    case 2:
      if(globalHeading==4){
        change = 90.0;
      }
      if(globalHeading==8){
        change = -90.0;
      }
      if(globalHeading==1){
        change = 180.0;
      }
      break;
    case 4:
      if(globalHeading==1){
        change = 90.0;
      }
      if(globalHeading==2){
        change = -90.0;
      }
      if(globalHeading==8){
        change = 180.0;
      }
      break;
    case 8:
      if(globalHeading==1){
        change = -90.0;
      }
      if(globalHeading==2){
        change = 90.0;
      }
      if(globalHeading==4){
        change = 180.0;
      }
      break;
  }
  float desiredHeading;
  //float desiredHeading = dispatch.gyroVal+change;
  //fix over or underflow

  if(((desiredHeading<45.0)||(desiredHeading>315.0))){
    desiredHeading=0.0;
  }
  if((desiredHeading>45.0)&&(desiredHeading<135.0)){
    desiredHeading = 90.0;
  }
  if((desiredHeading>135.0)&&(desiredHeading<225.0)){
    desiredHeading = 180.0;
  }
  if((desiredHeading>225.0)&&(desiredHeading<315.0)){
    desiredHeading = 270.0;
  }

  instruction turnMove = {7.74, desiredHeading};
  return turnMove;
}

//Executes the turn and moves forward as specified in the given instruction
void executeInstruction(instruction instruct){
  turn(instruct.desiredHeading);
  moveDist(instruct.desiredPos);
}

void floodFill(coord desired[], coord current, boolean isMoving){
  coord currCoord = current;
  byte heading = globalHeading;
  /*Integer representation of heading
  * 1 = N
  * 4 = E
  * 2 = S
  * 8 = W
  */
  while(maze[currCoord.y][currCoord.x].distance != 0){
    floodFillUpdate(currCoord, desired);
    byte nextHeading = orient(currCoord, heading);
    coord nextCoord = bearingCoord(currCoord, nextHeading);

    if(isMoving){
      //Call createInstruction to push a new instruction to the stack
      instructions.push(createInstruction(currCoord, nextCoord, nextHeading));

      //Pop the next instruction from the instructions queue and execute it
      executeInstruction(instructions.pop());
    }

    //After executing the instruction update the values of the local and global variables
    currCoord = nextCoord;
    heading = nextHeading;
    //If the robot has actually moved, update the global position variables
    if(isMoving){
      globalHeading = heading;
      globalCoord = currCoord;
    }
  }
  //Set the global end as the current coordinate.
  globalEnd = currCoord;
}

//Instantiate the reflood maze with the most optimistic values
void instantiateReflood(){
  for(int j = 0; j<Y; j++){
    for(int i = 0; i<X; i++){
      maze[j][i].distance = calcCenter(i, j, X);
    }
  }
}

void reflood(){
  //Refill the maze for most optimistic values, but now the maze has walls
  instantiateReflood();

  //Run flood fill but without actual motion
  coord desired[] = {{X/2-1,Y/2-1},{X/2-1,Y/2},{X/2,Y/2-1},{X/2,Y/2}};
  coord currCoord = {0,0};
  floodFill(desired, currCoord, false);

  //Now, the robot is still at the start, but the maze distance values have been updated with the walls discovered
  //So we follow the maze creating instructions
  createSpeedQueue();
  //We now have a queue of instructions.

}

//Trace the maze back to the end creating instructions and adding them to the queue
void createSpeedQueue(){
  coord workingCoord = globalCoord;
  byte workingDir = globalHeading;
  int workingDist = 0;
  while((workingCoord.x!=globalEnd.x)&&(workingCoord.y!=globalEnd.y)){
    byte optimalDir = orient(workingCoord, workingDir);
    coord nextCoord = bearingCoord(workingCoord, optimalDir);
    if((nextCoord.x!=globalEnd.x)&&(nextCoord.y!=globalEnd.y))
    {
        byte nextOptimalDir = orient(nextCoord,optimalDir);
        //If the direction is the same, accumulate distance
        if(optimalDir==workingDir){
          workingDist+=7.74;
        }
        if(optimalDir==nextOptimalDir){
          workingDist+=7.74;
          continue;
        }
        else{
          //if the optimal is different from the working, add the working and the accumulated distance
          workingDist+=7.74;
          instruction nextInstruction = {workingDist, optimalDir};
          instructions.push(nextInstruction);
          //Reset the distance to one square and update the workingDir
          workingDist = 0;
          workingDir = optimalDir;
    }
    }
    else
    {
      workingDist+=7.74;
      instruction nextInstruction = {workingDist, optimalDir};
      instructions.push(nextInstruction);
      return;
    }
    //update workingCoord to the next optimal coord
    workingCoord = nextCoord;
  }
}


void loop() {
//Run to the centre
  coord desired[] = {{X/2-1,Y/2-1},{X/2-1,Y/2},{X/2,Y/2-1},{X/2,Y/2}};
  floodFill(desired, globalCoord, true);

//Create an indication that center has been reached

//Return to the start
  coord returnCoord[] = {{0,0}};
  resetToCoord(returnCoord[0]);
  floodFill(returnCoord, globalCoord, true);

//Create an indication that start has been reached

//Speed Run
  //Reflood the maze
  reflood();
  //Pop instructions from the front of the queue until its empty.
  while(!instructions.isEmpty()){
    executeInstruction(instructions.pop());
  }
}
