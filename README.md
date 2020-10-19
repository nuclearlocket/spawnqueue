# spawnqueue
Unreal Engine 4 Multiplayer Spawn Queue system for Dedicated Server

Spawn Queue system with everything needed to run Deathmatch on Dedicated Server. Spawn Players into the world at specific location according to their team and spawn group. Drop CustomPlayerStart in your level to define spawn locations and create Game Mode from the features of this plugin.

This plugin is designed for a multiplayer game and most features are designed to be in Game Mode. Major features are independent, server join queue requires players to be registered to acquire privilege levels. This means that you can use what you need or migrate it into your project piece by piece. You can also use Demo Project as your starting layout.

Features:

SpawnQueue Class, add players to time based queue and automatically spawn players into the world at specific spawn points. Also includes Server Join Queue system, manage server maximum players, reserved slots and privileges.
ServerPlayers Class. Save player info on server and distribute PlayerID and later on search info using PlayerID or Player Controller.
CustomPlayerStart Actor (replicated). Spawn point with blocking detection and visualization, includes team and spawn group settings.
SimpleLoc Actor.  Actor for teleporting players to Lobby and Server Join locations.


Supported Target Build Platforms:  Win32, Win64, Linux.

Documentation: https://twopieces.levelparadox.com/

Demo Builds: https://drive.google.com/drive/folders/1YtXwRk825zUoN1Tb6RhMR2OKk8vS7wCP?usp=sharing
