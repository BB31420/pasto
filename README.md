# pasto
pasto is a simple command line approach for pasting contents from the clipboard 


## Building
* Clone the repo and navigate to its directory
* Build the c file:
*      gcc -o pasto pasto.c -lX11
* Update your shell profile to include with the appropriate path:
*      echo 'alias pasto="~/path/to/pasto"' >> ~/.bashrc
* Reload source to apply changes:
*     source ~/.bashrc


## Usage
* Create a directory with a file with clipboard content:
```pasto newdirectory/newfile.py```
* Overwrite a file in current directory:
```pasto newfile -o```


### Flags
* -o: Overwrite an existing file.  
* -a: Append the clipboard content to the file.
* -n: Open the file after creation/modification.
    
