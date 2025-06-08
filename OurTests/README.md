# Descriptions of test cases

- `test_readMazeTerminal`: Testing reading maze from terminal, normal case. Read a 7 by 5 maze from terminal.


## Team Member 1 (Dhiya)

- `test_readMazeTerminal_valid`:  
  Testing reading a maze from terminal, normal case.  
  Read a 7×5 maze with a valid entrance and no loops or isolated areas.

- `test_missingEntrance`:  
  Tests a maze with no entrance. The program should detect the issue and prompt to auto-fix it.

- `test_loopDetected`:  
  Tests a maze with a loop structure. Validation should detect the loop and offer to fix it.



## Team Member 2 (Sahar)




## Team Member 3 (Shahira)

- `test_manualTeleport_testmode`:  
  Tests manual teleportation in test mode. The agent should teleport to a random walkable position.

- `test_escapeRoute_success`:  
  Valid maze with a clear path. The agent should follow and display the escape route using lime carpet.

- `test_noEscape_trapped`:  
  Maze has isolated walkable zones and no path to exit. Should trigger “you are trapped” message.
