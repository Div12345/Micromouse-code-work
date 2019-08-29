//Last Update -> 29-8-19
//Author : Divyesh Narayanan

/*
Fast Run Procedure -
A.  1.  After reaching the center ReFlood the entire maze from the start to center but this time using the FloodFill values
    available and follow the path of least resistance to center
    2.  Repeat the same process after reaching the start from the center
    OPTION A REJECTED AS THE FLOODFILL VALUE FOLLOWING FAILS AT JUNCTIONS OF EQUAL VALUES IN THE BEGINNING

In the case the there will be problems in following the path of least resistance in the junctions where one of equals was chosen, alternative method -
B.  1. From the FloodFill function save the next coordinates in a Queue(FIFO)
    2. For the return save the next coordinates in a Stack(LIFO)
    After the return run is completed -
    3. Take the Queue, check for repetition and truncate the intermediate coordinates if any - 
        a. Get the no.of items in both
        b. Use it to create a 2D array and transfer the coordinates to it
        c. Then run a recursive search algo to find repetitions and truncate the intermediate coords, along with one of the repetition, whenever found
        e. Then use the arrays to create a queue of instruction for the path to center and a stack for the return
        f. Check the no. of items and take the one with lesser items
        g.  i.  If the return stack is the one with lesser items then invert all the instructions, store in a queue and use it for the fast run
            ii. If the center run queue is the one with lesser items then use it as it is for the fast run
    4. Execute the fast run with the set of instructions

*/
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

struct instruction //initial data structure with a coord and a heading
{
  coord Pos;
  byte Heading;
};

//Navigation info
int Path=1;//To identify in FloodFill to which queue the instruction must be updated
int PathFinal = 0; //Quickest path identified in IdentifyPath

QueueList<instruction> PathOne;
QueueList<instruction> PathTwo;

int r=0;
int br=1;

struct finalInstruct //Data structure for usage of the final queue
{
    int turnAngle;//Angle by which the bot must turn
    byte rep;//Repetitions of the command
};

QueueList<finalInstruct> FinalQueue;

struct entry
{
  byte distance;
  byte walls;
};

coord currCoord = {0,0};
coord globalCoord = {0,0};
coord globalEnd = {0,0};
byte globalHeading = 1;

/* Requirement of the variable desiredlen -
To keep track of the length of the "desired" array as declared in the loop, passed through the floodfill algo and used in function isEnd.
This is required because the size of the array will be 4 when going to the center and 1 while returning,
and the function needs to know which is the case.
*/
int desiredlen = 4;

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
//debug
Serial.println("Started"); 
//Serial.println(Test[6][11]);
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
  for(int i=0; i<desiredlen;i++){
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
  //debug
  //Serial.println(a);
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
  */
  return leastDir;
}
/*
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

    //debug
    /*Serial.println("Next Coordinate - ");
    Serial.print(nextCoord.x);
    Serial.print(",");
    Serial.println(nextCoord.y);
    Serial.println("-------");
    */
    //After executing the instruction update the values of the local and global variables
      currCoord = nextCoord;
      heading = nextHeading;
    //Update the queue of instructions
      instruction iP;
      iP.Pos.x=nextCoord.x;
      iP.Pos.y=nextCoord.y;
      iP.Heading=nextHeading;

      //debug
    Serial.print(iP.Heading);
    Serial.print(" ; ");
    Serial.print(iP.Pos.x);
    Serial.print(",");
    Serial.println(iP.Pos.y);
      
      if(Path==1)
      PathOne.push(iP);
      if(Path==2)
      PathTwo.push(iP);

    //Update the global position variables
      globalHeading = heading;
      globalCoord = currCoord;
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

void IdentifyPath(coord desired[]){
    int len1=0, len2=0;
    len1 = PathOne.count();
    len2 = PathTwo.count();
    
    //debug
    Serial.print("Length of Unrefined Path One = ");
    Serial.println(len1);
    Serial.print("Length of Unrefined Path Two = ");
    Serial.println(len2);
    //debug Serial.println("UnRefined Path 1 - ");
    byte PathOneA[len1][3],PathTwoA[len2][3];
    for(int a=0;a<len1;a++)
    {
        instruction i1 = PathOne.pop();
        PathOneA[a][0] = i1.Pos.x;
        PathOneA[a][1] = i1.Pos.y;
        PathOneA[a][2] = i1.Heading;
        //debug
        /*Serial.print(i1.Pos.x);
        Serial.print(",");
        Serial.print(i1.Pos.y);
        Serial.print(" ; ");
        Serial.println(i1.Heading);
        */
    }
    
    //debug Serial.println("UnRefined Path 2 - ");
    for(int a=0;a<len2;a++)
    {
        instruction i2 = PathTwo.pop();
        PathTwoA[a][0] = i2.Pos.x;
        PathTwoA[a][1] = i2.Pos.y;
        PathTwoA[a][2] = i2.Heading;
        //debug
        /*Serial.print(i2.Pos.x);
        Serial.print(",");
        Serial.print(i2.Pos.y);
        Serial.print(" ; ");
        Serial.println(i2.Heading);
        */    
    }

    //PathOne Optimization
    r=0;
    for(int a=0;a<(len1-1);a++)
    {
        if(a!=r)
        continue;

        for(int b=(a+1);b<len1;b++)
        {
            if((PathOneA[a][0]==PathOneA[b][0])&&(PathOneA[a][1]==PathOneA[b][1]))
            r=b;
        }

        //Create an instruction
        instruction iA;
        //Assign values to the instruction
        iA.Pos.x = PathOneA[a][0];
        iA.Pos.y = PathOneA[a][1];
        iA.Heading = PathOneA[a][2];
        //Insert the instruction into the PathOne queue for a refined set of instructions
        PathOne.push(iA);
        r++;
        
    }

    //Last instuction has to be added manually
    instruction iL1;
    iL1.Pos.x = PathOneA[len1-1][0];
    iL1.Pos.y = PathOneA[len1-1][1];
    iL1.Heading = PathOneA[len1-1][2];
    //Insert the instruction into the PathTwo queue for a refined set of instructions
    PathOne.push(iL1);
    
    //debug print the set of instructions
    /*Serial.println("First Set of instructions - ");
    while(!(PathOne.isEmpty()))
    {
    instruction iA = PathOne.pop();
    Serial.print(iA.Pos.x);
    Serial.print(",");
    Serial.print(iA.Pos.y);
    Serial.print(" ; ");
    Serial.println(iA.Heading);
    }
    */

    //reset r
    r=0;

    //PathTwo Optimization
    for(int a=0;a<(len2-1);a++)
    {
        if(a!=r)
        continue;

        for(int b=(a+1);b<len2;b++)
        {
            if((PathTwoA[a][0]==PathTwoA[b][0])&&(PathTwoA[a][1]==PathTwoA[b][1]))
            r=b;
        }

        //Create an instruction
        instruction iA;
        //Assign values to the instruction
        iA.Pos.x = PathTwoA[a][0];
        iA.Pos.y = PathTwoA[a][1];
        iA.Heading = PathTwoA[a][2];
        //Insert the instruction into the PathTwo queue for a refined set of instructions
        PathTwo.push(iA);
        r++;
        
    }
    //Last instuction has to be added manually
    instruction iL2;
    iL2.Pos.x = PathTwoA[len2-1][0];
    iL2.Pos.y = PathTwoA[len2-1][1];
    iL2.Heading = PathTwoA[len2-1][2];
    //Insert the instruction into the PathTwo queue for a refined set of instructions
    PathTwo.push(iL2);

    //debug print the set of instructions
    /*Serial.println("Second Set of instructions - ");
    while(!(PathTwo.isEmpty()))
    {
    instruction iA = PathTwo.pop();
    Serial.print(iA.Pos.x);
    Serial.print(",");
    Serial.print(iA.Pos.y);
    Serial.print(" ; ");
    Serial.println(iA.Heading);
    }
*/
    //identify which queue has lesser instructions
    int Rlen1 = PathOne.count();
    int Rlen2 = PathTwo.count();

    //debug
    Serial.print("Length of Refined Path One = ");
    Serial.println(Rlen1);
    Serial.print("Length of Refined Path Two = ");
    Serial.println(Rlen2);

    //debug Serial.println("Refined Path 1 - ");
    byte PathOneT[Rlen1][3],PathTwoT[Rlen2][3];
    for(int a=0;a<Rlen1;a++)
    {
        instruction i1 = PathOne.pop();
        PathOneT[a][0] = i1.Pos.x;
        PathOneT[a][1] = i1.Pos.y;
        PathOneT[a][2] = i1.Heading;
        //debug
        Serial.print(i1.Heading);
        Serial.print(" ; ");
        Serial.print(i1.Pos.x);
        Serial.print(",");
        Serial.println(i1.Pos.y);
        
    }
    
    //debug 
    Serial.println("Refined Path 2 - ");
    for(int a=0;a<Rlen2;a++)
    {
        instruction i2 = PathTwo.pop();
        PathTwoT[a][0] = i2.Pos.x;
        PathTwoT[a][1] = i2.Pos.y;
        PathTwoT[a][2] = i2.Heading;
        //debug
        
        Serial.print(i2.Heading);
        Serial.print(" ; ");
        Serial.print(i2.Pos.x);
        Serial.print(",");
        Serial.println(i2.Pos.y);
        
            
    }
    
    int counter =0;//For keeping track of the no. of continuous rep.s of the same dir. for creation of final instructions
    //debug review direc initial value
    byte direc=1;//For keeping track of the direction of the above stated purpose
    int turn =0;//Angle to be turned
    if(Rlen1>Rlen2)
    {   Serial.println("First path - ");
        PathFinal = 1;
        //If the Shortest Path is PathOne, create the final run instructions
        direc = globalHeading;
        Serial.println(direc);
        turn = identifyAngle(direc, PathOneT[0][2]);
        Serial.println(turn);
        direc = PathOneT[0][2];
        Serial.println(direc);
        counter=1;
        br=1;
        for(int a=1;a<Rlen1;a++)
        {   
          if(br==1){
            coord coor;
          coor.x= PathOneT[a][0];
          coor.y= PathOneT[a][1];
          if(isEnd(coor, desired))
          br++;
            //Turn angle must also be identified
            int iturn = identifyAngle(direc, PathOneT[a][2]);
            Serial.println(iturn);
            Serial.println(PathOneT[a][2]);
            if(iturn==0)
            { 
              counter++;  
              //Last instruction must be added manually in case turn is not made in the last move
              if((a==Rlen1-1)||(br!=1))
                {
                  //Interim data object
                  finalInstruct fi;
                  fi.turnAngle = turn;
                  fi.rep = counter;
                  //debug
                  Serial.print(fi.turnAngle);
                  Serial.print(" , ");
                  Serial.println(fi.rep);
                  //Add to queue
                  FinalQueue.push(fi);
                }
            }

            else//if the direction is changing then the previous instruction must be added to the final queue
            {
                //Interim data object
                finalInstruct fi;
                fi.turnAngle = turn;
                fi.rep = counter;
                //debug
                Serial.print(fi.turnAngle);
                Serial.print(" , ");
                Serial.println(fi.rep);
                //Add to queue
                FinalQueue.push(fi);

                //Reset variables
                counter = 1;
                turn = iturn;
                direc = PathOneT[a][2];
            }
        }
        }
    }
    else
    {   //Access the array from the last and invert the directions, then run the same algo as the first path
        PathFinal = 2;
        //If the Shortest Path is PathOne, create the final run instructions
        direc = globalHeading;
        Serial.println(direc);
        turn = identifyAngle(direc, oppHeading(PathTwoT[Rlen2-1][2]));
        //Serial.println(turn);
        direc = oppHeading(PathTwoT[Rlen2-1][2]);
        //Serial.println(direc);
        counter=1;
        br=1;
        for(int a=Rlen2-2;a>=0;a--)
        { if(br==1)
          {coord coor;
          coor.x= PathTwoT[a][0];
          coor.y= PathTwoT[a][1];
          if(isEnd(coor, desired))
          br++;
            //Turn angle must also be identified
            int iturn = identifyAngle(direc, oppHeading(PathTwoT[a][2]));
            //Serial.println(iturn);
            //Serial.println(oppHeading(PathOneT[a][2]));
            if(iturn==0)
            {    
              counter++;
              //Last instruction must be added manually in case turn is not made in the last move
              if((a==0)||(br!=1))
                {
                  //Interim data object
                  finalInstruct fi;
                  //Turn angle must be inverted for the sake of easiness to convert to final queue
                  fi.turnAngle = turn;
                  fi.rep = counter;
                  //debug
                  Serial.print(fi.turnAngle);
                  Serial.print(" , ");
                  Serial.println(fi.rep);
                  //Add to queue
                  FinalQueue.push(fi);
                }
            }

            else//if the direction is changing then the previous instruction must be added to the final queue
            {
                //Interim data object
                finalInstruct fi;
                //Turn angle must be inverted for the sake of easiness to convert to final queue
                fi.turnAngle = turn;
                fi.rep = counter;
                //debug
                Serial.print(fi.turnAngle);
                Serial.print(" , ");
                Serial.println(fi.rep);
                //Add to queue
                FinalQueue.push(fi);

                //Reset variables
                counter = 1;
                turn = iturn;
                direc = oppHeading(PathTwoT[a][2]);
            }
        
        }
        }
    }  
}

int identifyAngle(byte oldDir, byte newDir)
{
    if(((oldDir==1)&&(newDir==4))||((oldDir==4)&&(newDir==2))||((oldDir==2)&&(newDir==8))||((oldDir==8)&&(newDir==1)))
    return 90;

    else if(((oldDir==4)&&(newDir==1))||((oldDir==2)&&(newDir==4))||((oldDir==8)&&(newDir==2))||((oldDir==1)&&(newDir==8)))
    return -90;

    else if(((oldDir==1)&&(newDir==1))||((oldDir==2)&&(newDir==2))||((oldDir==4)&&(newDir==4))||((oldDir==8)&&(newDir==8)))
    return 0;
    
    else if(((oldDir==2)&&(newDir==1))||((oldDir==1)&&(newDir==2))||((oldDir==8)&&(newDir==4))||((oldDir==4)&&(newDir==8)))
    return 180;

    

}

int invertAngle(int a)
{
    if(a==90)
    return -90;
    else if(a==(-90))
    return 90;
    else
    return 180;
}

void FastRun()
{//debug - For now print the final instructions
  Serial.println("Final - ");
    while(!(FinalQueue.isEmpty()))
    {
    finalInstruct fi = FinalQueue.pop();
    Serial.print(fi.turnAngle);
    Serial.print(",");
    Serial.println(fi.rep);
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  // debug
  /*Serial.println("Test Maze");
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
  */
  Path=1;
  desiredlen = 4;
  coord desired[] = {{X/2-1,Y/2-1},{X/2-1,Y/2},{X/2,Y/2-1},{X/2,Y/2}};
  //debug
  Serial.println("First Run Instructions - ");
  floodFill(desired, globalCoord, false);
  Serial.println("Done");
  // debug
  /*Serial.println("Reached the Center, Current coord - ");
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
  */
  //Return to the start
  Path=2;
  coord returnCoord[] = {{0,0}};
  resetToCoord(returnCoord[0]);
  desiredlen = 1;
  // debug
  /*Serial.println("Initial Distance Values");

  for(int i=Y-1;i>=0;i--)
    {for(int j=0;j<X;j++)
      {Serial.print(maze[i][j].distance);
       Serial.print("\t");
      }
     Serial.println();
    }
  Serial.println();
  */
  //debug
  Serial.println("Second Run Instructions - ");
  floodFill(returnCoord, globalCoord, false);
  Serial.println("Done");
  
  //debug
  /*
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
  */
  desiredlen = 4;
  Serial.println("Starting Path ID");
  //Post FloodFill refining of the queues holding the paths and identifying the shortest path
    IdentifyPath(desired);
  //Now that the final run instruction queue is prepared, go for the final fast run
    FastRun();

  delay(50000);
  
}
