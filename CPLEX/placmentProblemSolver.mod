/*********************************************
 * OPL 22.1.1.0 Model
 *********************************************/


int n = ...; range FixedObjects = 1..n;
int p = ...; range PlacedObjects = 1..p;

range dimension = 1..2;


// Parameters
int Weights[FixedObjects] = ...;
int FixedObjectCoords[FixedObjects][dimension] = ...;
int MinDistances[FixedObjects][PlacedObjects] = ...;
int MinDistObjects[PlacedObjects][PlacedObjects] = ...;
int AreaDimensions[dimension] = ...;


int W = 2 * (AreaDimensions[1] + AreaDimensions[2]);
int H = W;
int M = MinDistObjects[1][2];

// Variables
dvar float L;  // Objective variable
dvar int+ x[PlacedObjects];  // x-coordinates for placed objects
dvar int+ y[PlacedObjects];  // y-coordinates for placed objects
dvar boolean z[FixedObjects][PlacedObjects];
dvar boolean t[FixedObjects][PlacedObjects];
dvar boolean s[PlacedObjects][PlacedObjects];
dvar boolean u[PlacedObjects][PlacedObjects];


// Objective function
maximize L;

// Constraints
subject to
{
   // Constraints for maintaining minimum distance between fixed and placed objects
	forall(i in FixedObjects, j in PlacedObjects)
	{
		x[j] - FixedObjectCoords[i][1] + y[j] - FixedObjectCoords[i][2] + z[i][j]*H + t[i][j]*W >= MinDistances[i][j];
		x[j] - FixedObjectCoords[i][1] + y[j] - FixedObjectCoords[i][2] + z[i][j]*H + t[i][j]*W >= L / Weights[i];
							
        -x[j] + FixedObjectCoords[i][1] + y[j] - FixedObjectCoords[i][2] + (1-z[i][j])*H + t[i][j]*W >= MinDistances[i][j];
        -x[j] + FixedObjectCoords[i][1] + y[j] - FixedObjectCoords[i][2] + (1-z[i][j])*H + t[i][j]*W >= L / Weights[i];
                
     	x[j] - FixedObjectCoords[i][1] - y[j] + FixedObjectCoords[i][2] + z[i][j]*H + (1-t[i][j])*W >= MinDistances[i][j];
     	x[j] - FixedObjectCoords[i][1] - y[j] + FixedObjectCoords[i][2] + z[i][j]*H + (1-t[i][j])*W >= L / Weights[i];
     	     	 	
     	-x[j] + FixedObjectCoords[i][1] - y[j] + FixedObjectCoords[i][2] + (1-z[i][j])*H + (1-t[i][j])*W >= MinDistances[i][j];
     	-x[j] + FixedObjectCoords[i][1] - y[j] + FixedObjectCoords[i][2] + (1-z[i][j])*H + (1-t[i][j])*W >= L / Weights[i];
   	}

    
    // Constraints for maintaining minimum distance between placed objects
    forall( i in PlacedObjects, j in PlacedObjects : i != j ) 
    {
        x[j] - x[i] + y[j] - y[i] + s[i][j]*H + u[i][j]*W >= M;
        -x[j] + x[i] + y[j] - y[i] + (1-s[i][j])*H + u[i][j]*W >= M;
        x[j] - x[i] - y[j] + y[i] + s[i][j]*H + (1-u[i][j])*W >= M;
        -x[j] + x[i] - y[j] + y[i] + (1-s[i][j])*H + (1-u[i][j])*W >= M;
    }
    
    // Constraints for ensuring all placed objects lie within the specified area
    forall(j in PlacedObjects)
    {
        y[j] - x[j] >= 0;
        x[j] + y[j] >= 0;
        y[j] + x[j] <= AreaDimensions[1];
        y[j] - x[j] <= AreaDimensions[2];
    }
}

execute
{
    writeln("L: ", L);
    writeln("Object coordinates: ");
    for (var j in PlacedObjects) {
        writeln("Object ", j, ": (", x[j], ", ", y[j] , ")");
    }
}
