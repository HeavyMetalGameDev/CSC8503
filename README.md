# Summary
CSC8503 was a module focused on programming physics, networking and AI into a game in C++.
This is the full codebase, including the code provided by the University. For more curated code samples, see my website at [heavymetalgamedev.github.io](heavymetalgamedev.github.io).
Furthermore, this codebase does not fully reflect my ability as a programmer, particularly when it comes to function length.
A lot of the messy code is a result of adding networking retroactively, in only a few days, which did not leave much time for cleaning up the code.

The main features of my game were:
## Physics
- Collisions for Sphere-Sphere , Sphere-AABB, Sphere-OBB, AABB-AABB, Capsule-Sphere, Capsule-AABB
- Ray collisions for all types. 
- Physics materials with changeable coefficient of restitution, vertical damping and horizontal damping. 
- Collision layers so objects can ignore collisions with each other. 
- Raycast collision layers. 
- Quad tree broad phase for faster processing. 
- Static and dynamic objects, where collisions between static objects are not considered. 
- Sleeping and awake objects, where collisions between sleeping objects are ignored, and static objects are also ignored. 
- A grappling hook that uses raycasts to allow the player to grapple towards static objects, and pull dynamic objects towards them. 
- Trigger volumes used for a jump pad, and used to allow the player to pick up and throw objects when the trigger is over a dynamic object. 

## AI 
- A state machine enemy that will patrol between points, run towards the player if it can see them, shoot the player if it is close enough, pathfind to get to the player if it cannot see them, and then return to patrolling if the player hides for long enough. 
- Pathfinding that can be executed multiple times by the enemy to navigate the world. 
- A simple behaviour tree powered enemy that stays near the treasure and provides resistance when it detects that the player is trying to steal the treasure, until it eventually gives up. 
- Both of these enemies work in single player and in multiplayer, where they are controlled by the server in multiplayer. 
- Pushdown automata used for the main menu, including the ability to pause the game in singleplayer. 

## Networking
- The game can be started as a client or a server. 
- When the player joins a server, the server sends a message to all players and the correct number of player objects are added. 
- Player inputs are sent to the server and processed. 
- The server executes all physics, and the results are sent to the clients. 
- Game information such as player health and points are sent from the server to the clients when events occur that change those values. 

## Other
- Simple component system for easily adding functionality to game objects. This is not a full Entity Component System but for the purposes of the coursework it is sufficient. 
