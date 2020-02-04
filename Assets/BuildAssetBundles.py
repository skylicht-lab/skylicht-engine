import zipfile
import os

compressExt = ["xml", "hlsl", "glsl", "png", "dae", "tga"]


def needCompress(filename):
    for fileType in compressExt:
        if filename.endswith(fileType):
            return True
    return False


def compress(dirName):
    outputZip = "../Bin/" + dirName + ".zip"
    z = zipfile.ZipFile(outputZip, "w", zipfile.ZIP_DEFLATED)
    for root, dirs, files in os.walk(dirName):
        for file in files:
            if needCompress(file):
                print("%s <-- %s - %s" % (outputZip, root, file))
                z.write(os.path.join(root, file))
    z.close


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
