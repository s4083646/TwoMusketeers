# Descriptions of test cases

- `test_readMazeTerminal`: Testing reading maze from terminal, normal case. Read a 7 by 5 maze from terminal.


## Team Member 1 (Dhiya Batrisya Binti Norfaiz)

- `manual_maze_read_terminal`:  
  This test checks if the system correctly reads and accepts a clean, valid maze input from the terminal.
   -The maze has a single valid entrance and a clear path to the exit.
   -There are no disconnected segments or internal loops.
   -Expected behavior: The program accepts the input without requiring any fixes.

- `manual_maze_missing_exit`:  
  This test evaluates the program’s ability to detect when a maze lacks an exit.
   -The input is a sealed maze with no exit point on the outer boundary.
   -The system identifies this issue and automatically opens an exit in the correct position (usually along the rightmost column)according to the spec.
   -Expected behavior: The maze is modified with a new exit, and validation passes afterward.

- `manual_maze_with_loop`:  
  This test involves a maze where one or more paths are disconnected or form internal loops.
   -The validator detects the issue and triggers a fix.
   -The fix routine identifies isolated segments and connects them to the main path using a shortest path approach (Manhattan distance).
   -After fixing, the maze contains a single connected region with only one valid entrance and exit.
   -Expected behavior: The program modifies the maze structure to ensure all regions are reachable and compliant with perfect maze rules.


## Team Member 2 (Sahar)



## Team Member 3 – (Shahira)

- `test_manualTeleport_testmode`:  
- Purpose:  
  Tests manual teleportation in test mode. The agent should teleport to the furthest walkable path.
  
- Input:  
  A maze with multiple open branches and dead ends.

- Expected Behavior:  
  - When manual teleport is triggered, the agent should teleport to the furthest reachable tile using BFS.
  - This ensures the furthest valid walkable point is selected.
  
- Validation:
  - Uses menu sequence: 3 → 2 → 3 (Solve Maze → Show Escape Route → Back).
  - Output should include coordinates of furthest walkable point.
  - Teleportation message should confirm correct position.


--


- `test_escapeRoute_success`:  
- Purpose:  
  Validates that the escape route is correctly shown in a solvable maze.
  
- Input:  
  A maze with one entrance and a valid exit path.

- Expected Behavior:  
  - The agent should display the escape route using lime carpet blocks.
  - No "trapped" message should be shown.

- Validation:
  - Uses menu sequence: 3 → 1 → 3 (Solve Maze → Solve Manually → Back).
  - Output should show a printed path and confirmation message.


--


- `test_noEscape_trapped`:  
- Purpose:  
  Checks handling of mazes with no valid exit.

- Input:  
  A maze where the player is enclosed or no path leads to the exit.

- Expected Behavior:  
  - Program should detect the player is trapped.
  - It should output a message like "[❌] Warning: No valid exit found to place blue carpet!"

- Validation:
  - Uses menu: 3 → 3 (Solve Maze → Back).
  - Test passes if:
    - No carpet path is shown.
    - An error message about no exit is printed.
