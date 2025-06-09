# Descriptions of test cases

- `test_readMazeTerminal`: Testing reading maze from terminal, normal case. Read a 7 by 5 maze from terminal.


## Team Member 1 (Dhiya Batrisya Binti Norfaiz)

- `manual_maze_read_terminal`:  
  Reads a valid 7×9 maze entered manually from the terminal. The maze has a single valid entrance and no disconnected areas or loops. It passes validation and does not require any fixes.

- `manual_maze_missing_exit`:  
  Tests a maze that lacks an exit. The system correctly detects this and automatically places a valid exit along the rightmost column of the bottom rows, following the assignment rules.

- `manual_maze_with_loop`:  
  ests a maze that contains a loop or disconnected segment. The fix routine connects any isolated paths to the main route by calculating the shortest Manhattan path and integrating it, thus ensuring a valid perfect maze structure.



## Team Member 2 (Sahar)



## Team Member 3 – (Shahira)

- `test_manualTeleport_testmode`:  
  Tests manual teleportation in test mode. The agent should teleport to a random walkable position.

- `test_escapeRoute_success`:  
  Valid maze with a clear path. The agent should follow and display the escape route using lime carpet.

- `test_noEscape_trapped`:  
  Maze has isolated walkable zones and no path to exit. Should trigger “you are trapped” message.
