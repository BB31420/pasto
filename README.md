# pasto
pasto is a simple command line approach for pasting contents from the clipboard 


## Building
* ```gcc -o clipboard clipboard.c -lX11```
* ```chmod +x pasto.sh```
* Update your shell profile to include: ```alias pasto="~/path/to/pasto.sh"```

## Usage
* create a directory with a file with clipboard content:
```pasto newdirectory newfile.txt```
* overwrite a file in current directory:
```pasto newfile.txt -o```
