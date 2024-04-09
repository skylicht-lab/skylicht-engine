import os

from BuildAsset import compress

def main():
    # Create Bin folder
    if (os.path.exists("../Bin/") is False):
        os.mkdir("../Bin/")

    # Walk and build bundle
    directory = "."
    for filename in os.listdir(directory):
        if os.path.isdir(filename):
            print("Compress directory: %s" % filename)
            compress(filename)
            continue
        else:
            continue


if __name__ == '__main__':
    main()
