//Last Update -> 26-6-19
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
byte globalHeading = 1;

byte wallDistance = 100;//Change after checking - Basically a threshold past which it means that there isn't a wall immediately in that direction
#define X 16
#define Y 16

entry maze[Y][X];
//N,S,E,W
byte headings[] = {1,2,4,8};

byte Test[16][16] = {{14,10,3,2,3,2,2,6,10,3,3,3,3,3,3,6},{12,12,10,5,10,5,12,12,12,11,2,3,3,3,3,5},{12,9,5,10,5,10,4,12,12,11,1,3,3,3,3,6},{8,6,10,5,10,5,12,12,12,14,10,3,3,3,3,5},{12,9,5,10,5,10,5,9,5,8,4,10,3,3,3,6},{8,3,3,1,3,0,7,14,10,5,12,8,7,10,3,4},{9,2,7,10,6,9,7,9,1,2,4,13,10,5,10,5},{14,12,10,5,8,3,6,10,6,13,9,3,5,10,1,6},{8,4,9,6,12,10,5,9,1,2,2,7,10,1,3,5},{12,9,7,12,8,5,14,10,6,13,12,10,0,2,2,6},{12,10,3,5,12,10,4,13,8,3,4,12,12,13,13,12},{12,9,3,6,12,12,8,6,9,6,13,12,9,3,6,13},{9,6,11,4,12,12,12,9,6,9,6,9,3,6,9,6},{10,5,10,5,12,8,4,14,9,6,9,2,6,9,2,4},{8,7,9,6,12,12,12,8,3,5,14,13,9,3,5,12},{9,3,3,5,9,1,5,9,3,3,1,3,3,3,3,5}};

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

Serial.begin(9600);
Serial.println(Test[6][11]);
delay(3000);
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

  int minVal =  256;
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
  }/*
  Serial.println();
  Serial.println(minVal);
  Serial.println();
  */
  return minVal;
}

/*
INPUT: Coordinate to update, and a direction representing the wall to add
OUTPUT: Update to coordinate adding the wall provided as an argument

void coordUpdate(coord coordinate, byte wallDir){
  if(checkBounds(coordinate)){
    if((maze[coordinate.y][coordinate.x].walls & wallDir) != wallDir){
      maze[coordinate.y][coordinate.x].walls += wallDir;
    }
  }
}
*/
void floodFillUpdate(coord currCoord, coord desired[])
{
StackList<coord> entries;

//Update wall value of current cell
  maze[currCoord.y][currCoord.x].walls=Test[currCoord.y][currCoord.x];

  //Check for the presence of a Lower Open Neighbours for the current cell
  if(checkNeighs(currCoord)<maze[currCoord.y][currCoord.x].distance)
  //Since a Lower Valued Neighbour exists, proceed to finding the heading direction in the main Loop
  {//Serial.println("FF complete without any updates");
  return;
  }
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

      /*Serial.println();

      Serial.println("Updated Distance Values");
 
      for(int i=Y-1;i>=0;i--)
      {for(int j=0;j<X;j++)
      {Serial.print(maze[i][j].distance);
       Serial.print("\t");
      }
     Serial.println();
      }
      */
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
  int leastNextVal = 256;
  byte oppHead = oppHeading(heading);
  byte leastDir = heading;
  byte a= maze[currCoord.y][currCoord.x].walls;
  Serial.println(a);
  //If there is a bitwise equivalence between the current heading and the cell's value, then the next cell is accessible
  //Setting this first gives first preference to proceeding straight
  if((a & heading) != heading){
    //Define a coordinate for the next cell based on this heading and set the leastNextVal to its value
    /*Serial.println();
    Serial.print("First Open Heading - ");
    Serial.println(heading);
    Serial.println();
    */
    coord leastnextTemp = bearingCoord(currCoord, heading);

    if(checkBounds(leastnextTemp)){
      leastNext = leastnextTemp;
      leastNextVal = maze[leastNext.y][leastNext.x].distance;
    }
  }

  for(int i=0; i<4; i++){
    byte dir = headings[i];
    //if this dir is accessible
    if((a & dir) != dir){
      //define the coordinate for this dir
      coord dirCoord = bearingCoord(currCoord,dir);
      /*Serial.println();
      Serial.print("Open Heading - ");
      Serial.println(dir);
      Serial.println();
*/
      if(checkBounds(dirCoord)){
        //if this dir is more optimal than continuing straight
        byte b= maze[dirCoord.y][dirCoord.x].distance;
        //Serial.println("Bound Check Fine");
        if(b < leastNextVal){
          //update the value of leastNextVal
          leastNextVal = maze[dirCoord.y][dirCoord.x].distance;
          //update the value of leastnext to this dir
          leastNext = dirCoord;
          leastDir = dir;
          /*Serial.println();
          Serial.print("Lesser Open Heading - ");
          Serial.println(dir);
          Serial.println();*/
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
  /*Serial.println();
  Serial.println(leastDir);
  */return leastDir;
}

//Returns an instruction which contains the information on how it has to turn and how much to move forward according to given info
//Make changes to this according to gyroscope and change 7.74 according to the distance to be moved forward
/*instruction createInstruction(coord currCoord, coord nextCoord, byte nextHeading){
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
*/
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
    
    Serial.println("Next Coordinate - ");
    Serial.print(nextCoord.x);
    Serial.print(",");
    Serial.println(nextCoord.y);
    Serial.println("-------");
  
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

/*void reflood(){
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


*/

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Test Maze");
  for(int i=Y-1;i>=0;i--)
    {for(int j=0;j<X;j++)
      {Serial.print(Test[i][j]);
       Serial.print("\t");
      }
     Serial.println();
    }
  Serial.println();

  Serial.println("Initial Wall Values");

  for(int i=Y-1;i>=0;i--)
    {for(int j=0;j<X;j++)
      {Serial.print(maze[i][j].walls);
       Serial.print("\t");
      }
     Serial.println();
    }
  Serial.println();

  Serial.println("Initial Distance Values");

  for(int i=Y-1;i>=0;i--)
    {for(int j=0;j<X;j++)
      {Serial.print(maze[i][j].distance);
       Serial.print("\t");
      }
     Serial.println();
    }
  Serial.println();
  
  coord desired[] = {{X/2-1,Y/2-1},{X/2-1,Y/2},{X/2,Y/2-1},{X/2,Y/2}};
  floodFill(desired, globalCoord, false);

  Serial.println("Reached the Center, Current coord - ");
  Serial.print(globalCoord.y);
  Serial.print(",");
  Serial.println(globalCoord.x);
  
  Serial.println("Final Wall Values");
  
  for(int i=Y-1;i>=0;i--)
    {for(int j=0;j<X;j++)
      {Serial.print(maze[i][j].walls);
       Serial.print("\t");
      }
     Serial.println();
    }
  Serial.println();

  Serial.println("Final Distance Values");
 
  for(int i=Y-1;i>=0;i--)
    {for(int j=0;j<X;j++)
      {Serial.print(maze[i][j].distance);
       Serial.print("\t");
      }
     Serial.println();
    }
      
  delay(5000);
  
  //Return to the start
  globalCoord.y=8;
  globalCoord.x=8;
  coord returnCoord[] = {{0,0}};
  resetToCoord(returnCoord[0]);

  Serial.println("Initial Distance Values");

  for(int i=Y-1;i>=0;i--)
    {for(int j=0;j<X;j++)
      {Serial.print(maze[i][j].distance);
       Serial.print("\t");
      }
     Serial.println();
    }
  Serial.println();
  
  floodFill(returnCoord, globalCoord, false);

  Serial.println("Reached the Start");
  Serial.println("Final Wall Values");
  
  for(int i=Y-1;i>=0;i--)
    {for(int j=0;j<X;j++)
      {Serial.print(maze[i][j].walls);
       Serial.print("\t");
      }
     Serial.println();
    }
  Serial.println();

  Serial.println("Final Distance Values");
 
  for(int i=Y-1;i>=0;i--)
    {for(int j=0;j<X;j++)
      {Serial.print(maze[i][j].distance);
       Serial.print("\t");
      }
     Serial.println();
    }
      
  delay(50000);
  
}
