# Gid
## Overview
- This project is a custom implementation of Git for me to get used to Neovim and C++. It provides basic functionality for initializing a repository, adding and committing changes, viewing commit history, and retrieving specific commits. The implementation is written in C++ and aims to replicate core Git functionality. 

- It does not include algorithms like Myers diff which i found difficult to implement myself, so it's not that efficient but hey, it's my git. 

- It uses an upgraded version of my own [Parser](https://github.com/ahmetdem/Parser), also the SHA256 Algorithm is from [here](https://github.com/System-Glitch/SHA256).

## Features
- Initialization: Initialize a new Git repository in the current directory.
- Staging Changes: Add changes to the staging area (index) for committing.
- Committing Changes: Commit staged changes to the repository.
- Viewing Commit History: Display a log of commits made in the repository.
- Retrieving Commits: Retrieve specific commits by their hash.

## Usage
To compile the project, use the provided Makefile:
```bash
make
```

This will build the main executable gid. After compiling, you can run the program using:
```bash
./gid
```

## Command Line Options 

- init: Initialize a new repository.
- add: Stage changes for committing.
- commit: Commit staged changes.
- log: Display commit history.
- retrieve <commit_hash>: Retrieve a specific commit by its hash.
- --help: Display usage information.

## Example Usage
### Initializing a Repository
To initialize a new Git repository, navigate to the desired directory and run:
```bash 
./gid init
```
This will create a new Gid repository in the current directory.
You also may need to add gid executable to the path.

### Adding Changes
After making changes to your files, add them to the staging area using:
```bash
./gid add
```
This will stage the changes for committing.

### Committing Changes
To commit the staged changes to the repository, use:
```bash
./gid commit
```
This will create a new commit with a unique hash.
### Viewing Commit History
To view the commit history, use:
```bash
/gid log
```

This will display a list of all commits made in the repository, including their hashes, authors, timestamps, messages, and tree hashes.

### Retrieving Specific Commit
To retrieve a specific commit, provide its hash as an argument:
```bash
./gid retrieve <commit_hash>
```
This will retrieve the repository state at the specified commit and store it in a folder named "repo" in the above directory. Note that retrieving another repository will overwrite the "repo" folder.

