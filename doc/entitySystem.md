# Entity System

The entity system is coming reasonably soon to Engine2D so it's worth doing a write up.

## Entity Structure
Although I like a component based entity system I haven't liked how much overhead they introduce to coding. Something more like a event driven architecture, so Entity A has a set of events it can receive from the global game or subentries, this will be based on a much faster system compared to the Events system right now, since entity logic is static as soon as the entity is initialised they store data in small objects and share common code among all entities of the same type

## Coding a Entity
Since the relevant systems to Entity coding are too much up in the air to give a coding sample with any kind of backing I'm just going to say that entities will be derived off the package system in order to support transitions between C++ and Scripting, You'll set a flag in mark a Package as a entity which will change the way it's loaded and registered

## Identifying Entities
Entities will be identified by a EID which is a small structure made up of 2 parts

```
struct EID { // Entity ID
	unsigned short class; // gives 65536 classes
	unsigned short uid; // gives 65536 entities per class} // 4 bytes in total
struct PEID { // Portable Entity ID
	unsigned int packageID
	EID ent;} // 8 bytes in total
```

There are 3 different variables there:

- **class**: Portable as long as your in the same save game, identifies the data type the entity is
- **uid**: The real entity ID, unique per EntityManager/class combination
- **packageID**: A 8 hex char packageID, totally unique for any published package, package ID's starting with 0x0 through 0x3 are reserved for development and can't be loaded unless the engine is running in Developer Mode. The package details can be fetched from the main server using a simple REST api

EID's are used referring to entities stored in a save game. The entity set used by the save game is saved in metadata though new packages can be added old packages can't be removed. PEID's are used for detached entities like player save games and others that may be imported to a server missing packages

## Entity Commuinications
Entity communications is based on ethernet networking, the easy way is to think of the EID as the mac address of the entity.

## Entity Storage
Entity storage runs into quite a few problems at large scale, entities need to be freed to disk as the server runs out of memory so having a lightweight structure is the best bet for effective saving and loading. So the in memory and on disk structure both use a physical filesystem type structure. Attached entities are organised into a tree and a modified using transactions to avoid corruption.