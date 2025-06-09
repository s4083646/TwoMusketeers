# Descriptions of test cases

- `test_readMazeTerminal`: Testing reading maze from terminal, normal case. Read a 7 by 5 maze from terminal.


## Team Member 3 (Dhiya)

- `test_readMazeTerminal_valid`:  
  Testing reading a maze from terminal, normal case.  
  Read a 7×5 maze with a valid exit and no loops or isolated areas.

- `test_missingExit`:  
  Tests a maze with no exit. The program should detect the issue and prompt to auto-fix it.

- `test_loopDetected`:  
  Tests a maze with a loop structure. Validation should detect the loop and offer to fix it.



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
  - It should output a message like "You are trapped!"

- Validation:
  - Uses menu: 3 → 3 (Solve Maze → Back).
  - Test passes if:
    - No carpet path is shown.
    - Message about being trapped is printed.
