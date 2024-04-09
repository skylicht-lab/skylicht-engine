import os

from BuildAsset import compress

def main():
    # Create Bin folder
    if (os.path.exists("../Bin/") is False):
        os.mkdir("../Bin/")

    compress("BuiltIn")

if __name__ == '__main__':
    main()
